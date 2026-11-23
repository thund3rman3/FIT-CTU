import random
from typing import Tuple, List, Dict, Optional, Union, Any

import numpy as np
from matplotlib.figure import Figure

import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import Dataset, DataLoader
from torchvision.transforms import v2

from utils3 import process_img


class SiameseNetwork(nn.Module):
    """
    Siamese Network with shared CNN feature extractor and L2-normalized embedding.
    Supports both pair forward (x1, x2) and single-image forward (x1).
    """

    def __init__(self, in_channels: int = 3, embedding_dim: int = 64):
        super().__init__()
        self.encoder = nn.Sequential(
            nn.Conv2d(in_channels, 32, kernel_size=3, padding=1),
            nn.BatchNorm2d(32),
            nn.ReLU(inplace=False),
            nn.MaxPool2d(2),  # 128 -> 64

            nn.Conv2d(32, 64, kernel_size=3, padding=1),
            nn.BatchNorm2d(64),
            nn.ReLU(inplace=False),
            nn.MaxPool2d(2),  # 64 -> 32

            nn.Conv2d(64, 128, kernel_size=3, padding=1),
            nn.BatchNorm2d(128),
            nn.ReLU(inplace=False),
            nn.AdaptiveAvgPool2d((2, 2))  # 32 -> 2
        )
        self.projector = nn.Sequential(
            nn.Flatten(),
            nn.Linear(128 * 2 * 2, 128),
            nn.ReLU(inplace=False),
            nn.Linear(128, embedding_dim)
        )

    def forward_one(self, x: torch.Tensor) -> torch.Tensor:
        feat = self.encoder(x)
        emb = self.projector(feat)
        return F.normalize(emb, p=2.0, dim=1)

    def forward(
        self,
        x1: torch.Tensor,
        x2: Optional[torch.Tensor] = None
    ) -> Union[torch.Tensor, Tuple[torch.Tensor, torch.Tensor]]:
        if x2 is None:
            return self.forward_one(x1)
        return self.forward_one(x1), self.forward_one(x2)


class ContrastiveLoss(nn.Module):
    """
    Contrastive loss function:
    L(d, y) = y * d^2 + (1 - y) * max(0, margin - d)^2
    y = 1 for similar (positive), y = 0 for dissimilar (negative).
    """

    def __init__(self, margin: float = 1.0):
        super().__init__()
        self.margin = float(margin)

    def forward(self, emb1: torch.Tensor, emb2: torch.Tensor, label: torch.Tensor) -> torch.Tensor:
        dist = F.pairwise_distance(emb1, emb2)
        loss_pos = label * torch.pow(dist, 2)
        loss_neg = (1.0 - label) * \
            torch.pow(torch.clamp(self.margin - dist, min=0.0), 2)
        loss = torch.mean(loss_pos + loss_neg) / 2.0
        return loss


class SiameseDataset(Dataset):
    """
    PyTorch Dataset returning ((img1, img2), label).
    """

    def __init__(
        self,
        pairs: List[Tuple[Dict[str, Any], Dict[str, Any], float]],
        img_size: Optional[int] = 128,
        augment: bool = False
    ):
        self.pairs = pairs
        self.img_size = img_size

        self.normalize = v2.Compose([
            v2.ToDtype(torch.float32, scale=True),
            v2.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])
        ])
        self.aug = v2.Compose([
            v2.RandomHorizontalFlip(p=0.5),
            v2.RandomVerticalFlip(p=0.5)
        ]) if augment else None

    def __len__(self) -> int:
        return len(self.pairs)

    def __getitem__(self, idx: int) -> Tuple[torch.Tensor, torch.Tensor, torch.Tensor]:
        s1, s2, label = self.pairs[idx]
        t1 = process_img(s1["image"], self.normalize, self.aug, to_rgb=True)
        t2 = process_img(s2["image"], self.normalize, self.aug, to_rgb=True)
        return t1, t2, torch.tensor(label, dtype=torch.float32)


def train_siamese_network(
    model: SiameseNetwork,
    train_loader: DataLoader,
    val_loader: DataLoader,
    num_epochs: int = 12,
    lr: float = 1e-3,
    margin: float = 1.0,
    device: Union[str, torch.device] = "cpu"
) -> Dict[str, List[float]]:
    """
    Trains Siamese network with ContrastiveLoss.
    """
    dev = torch.device(device)
    model = model.to(dev)
    criterion = ContrastiveLoss(margin=margin)
    optimizer = torch.optim.Adam(model.parameters(), lr=lr)

    history: Dict[str, List[float]] = {
        "train_loss": [], "val_loss": [], "val_acc": []}

    print(f"{'Epoch':<6} | {'Train Loss':<12} | {'Val Loss':<12} | {'Val Acc (tau=0.5)':<15}")
    print("-" * 55)

    for epoch in range(1, num_epochs + 1):
        model.train()
        train_loss = 0.0
        train_count = 0

        for x1, x2, labels in train_loader:
            x1, x2, labels = x1.to(dev), x2.to(dev), labels.to(dev)
            optimizer.zero_grad()
            emb1, emb2 = model(x1, x2)
            loss = criterion(emb1, emb2, labels)
            loss.backward()
            optimizer.step()

            train_loss += float(loss.item()) * x1.size(0)
            train_count += int(x1.size(0))

        train_loss /= max(train_count, 1)

        # Validation
        model.eval()
        val_loss = 0.0
        val_count = 0
        correct = 0
        threshold = margin / 2.0

        with torch.no_grad():
            for x1, x2, labels in val_loader:
                x1, x2, labels = x1.to(dev), x2.to(dev), labels.to(dev)
                emb1, emb2 = model(x1, x2)
                loss = criterion(emb1, emb2, labels)

                val_loss += float(loss.item()) * x1.size(0)
                val_count += int(x1.size(0))

                dist = F.pairwise_distance(emb1, emb2)
                pred_similar = (dist < threshold).float()
                correct += int((pred_similar == labels).sum().item())

        val_loss /= max(val_count, 1)
        val_acc = correct / max(val_count, 1)

        history["train_loss"].append(train_loss)
        history["val_loss"].append(val_loss)
        history["val_acc"].append(val_acc)

        print(
            f"{epoch:<6} | {train_loss:<12.4f} | {val_loss:<12.4f} | {val_acc:<15.4f}")

    return history


def evaluate_siamese_distances(
    model: SiameseNetwork,
    val_loader: DataLoader,
    device: Union[str, torch.device] = "cpu"
) -> Tuple[np.ndarray, np.ndarray]:
    """
    Returns positive and negative pair distances from validation set.
    """
    dev = torch.device(device)
    model.to(dev)
    model.eval()
    pos_dists: List[float] = []
    neg_dists: List[float] = []

    with torch.no_grad():
        for x1, x2, labels in val_loader:
            x1, x2, labels = x1.to(dev), x2.to(dev), labels.to(dev)
            emb1, emb2 = model(x1, x2)
            dists = F.pairwise_distance(emb1, emb2).cpu().numpy().tolist()
            lbls = labels.cpu().numpy().tolist()

            for d, l in zip(dists, lbls):
                if l == 1.0:
                    pos_dists.append(d)
                else:
                    neg_dists.append(d)

    return np.array(pos_dists, dtype=np.float32), np.array(neg_dists, dtype=np.float32)


def _expand_samples_for_siamese(samples: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
    expanded = []
    for s in samples:
        expanded.append(
            {"image": s["image"], "label": 0, "label_name": f"Original ({s['id']})"})
        expanded.append(
            {"image": s["inpainted"], "label": 1, "label_name": f"Inpainted ({s['id']})"})
    return expanded


def generate_siamese_pairs(
        samples: List[Dict[str, Any]],
        num_pairs: int = 1200) -> List[Tuple[Dict[str, Any], Dict[str, Any], float]]:
    if len(samples) < 2:
        raise ValueError(
            "Need at least 2 cell samples to generate Siamese pairs.")

    expanded = _expand_samples_for_siamese(samples)
    orig_samples = [s for s in expanded if s["label"] == 0]
    inp_samples = [s for s in expanded if s["label"] == 1]

    pairs: List[Tuple[Dict[str, Any], Dict[str, Any], float]] = []
    half_pairs = num_pairs // 2

    # Positive pairs: 50% orig-orig, 50% inp-inp
    for _ in range(half_pairs // 2):
        s1, s2 = random.sample(orig_samples, 2)
        pairs.append((s1, s2, 1.0))
    for _ in range(half_pairs // 2):
        s1, s2 = random.sample(inp_samples, 2)
        pairs.append((s1, s2, 1.0))

    # Negative pairs: orig-inp
    for _ in range(half_pairs):
        s1 = random.choice(orig_samples)
        s2 = random.choice(inp_samples)
        pairs.append((s1, s2, 0.0))

    random.shuffle(pairs)
    return pairs
