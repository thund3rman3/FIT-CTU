from typing import Literal
import random
from pathlib import Path
from typing import Tuple, List, Dict, Optional, Union, Any, Sequence, cast

import cv2
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.figure import Figure

import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import Dataset, DataLoader
import torchvision.models as models
from torchvision.transforms import v2
from torchvision import tv_tensors


def set_seed(seed: int = 42) -> None:
    """Sets random seeds for reproducibility across random, numpy, and torch."""
    random.seed(seed)
    np.random.seed(seed)
    torch.manual_seed(seed)
    if torch.cuda.is_available():
        torch.cuda.manual_seed_all(seed)


# ==============================================================================
# 1. Dataset Preparation & Inpainting
# ==============================================================================

def check_image_format(image: np.ndarray) -> np.ndarray:
    """
    Ensures image is in uint8 format and converts to grayscale if needed.
    """
    img = np.asarray(image)
    if img.dtype != np.uint8:
        if img.max() <= 1.0:
            img = (img * 255).astype(np.uint8)
        else:
            img = img.astype(np.uint8)

    if img.ndim == 3:
        img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    return img


def get_image_and_mask(image, mask_pts):
    if image is None or mask_pts is None:
        raise ValueError("Neither 'image' nor 'mask_pts' can be None.")

    img = check_image_format(image)
    mask = check_image_format(mask_pts)

    binary_mask = (mask > 127).astype(np.uint8) * 255
    return img, binary_mask


def inpaint_cilia_image(
    image: np.ndarray,
    mask_pts: np.ndarray,
    inpaint_radius: float = 3.0,
    dilate_kernel_size: int = 3,
    iterations: int = 1,
    method: int = cv2.INPAINT_TELEA
) -> np.ndarray:
    """
    Inpaints microtubules in a cilia cell image using corresponding mask_pts.

    Parameters:
        image: Grayscale or BGR cell image (np.ndarray, uint8).
        mask_pts: Binary or point mask marking microtubule locations.
        inpaint_radius: Radius around inpainted pixels used by Telea algorithm.
        dilate_kernel_size: Kernel size for morphological dilation of mask points.
        iterations: Number of dilation iterations.
        method: OpenCV inpainting algorithm (default: cv2.INPAINT_TELEA).

    Returns:
        np.ndarray: Inpainted image matching input shape and uint8 dtype.
    """
    image, binary_mask = get_image_and_mask(image, mask_pts)

    if dilate_kernel_size > 0:
        kernel = cv2.getStructuringElement(
            cv2.MORPH_ELLIPSE, (dilate_kernel_size, dilate_kernel_size)
        )
        binary_mask = cv2.dilate(binary_mask, kernel, iterations=iterations)

    # TODO

    inpainted = cv2.inpaint(image, binary_mask, inpaint_radius, method)
    return inpainted


def make_pairs(cells_dir: Union[Path, str],
               masks_pts_dir: Union[Path, str],
               val_ratio: float = 0.2) -> Tuple[List[Tuple[Path, Path]], List[Tuple[Path, Path]]]:
    c_dir = Path(cells_dir).resolve()
    m_dir = Path(masks_pts_dir).resolve()

    if not c_dir.exists() or not m_dir.exists():
        raise FileNotFoundError(
            f"One or both directories not found: {cells_dir}, {masks_pts_dir}")

    pairs: List[Tuple[Path, Path]] = []
    for cf in sorted(c_dir.iterdir()):
        if not cf.is_file():
            continue
        mask_name = cf.name.replace("cell", "mask_pts")
        mf = m_dir / mask_name
        if mf.exists():
            pairs.append((cf, mf))

    if not pairs:
        raise RuntimeError(
            f"No matching (cell, mask_pts) pairs found in {c_dir} and {m_dir}!")

    random.shuffle(pairs)
    split_idx = int(len(pairs) * (1.0 - val_ratio))
    train_pairs = pairs[:split_idx]
    val_pairs = pairs[split_idx:]
    return train_pairs, val_pairs


def build_samples(pair_list: List[Tuple[Path, Path]],
                  img_size: Optional[int] = None) -> List[Dict[str, Any]]:
    samples: List[Dict[str, Any]] = []
    for cf, mf in pair_list:
        cell_img = cv2.imread(str(cf), cv2.IMREAD_GRAYSCALE)
        mask_img = cv2.imread(str(mf), cv2.IMREAD_GRAYSCALE)

        # Defensive null-check to prevent type mismatches with OpenCV loaders
        if cell_img is None:
            print(f"Warning: Failed to load cell image: {cf}")
            continue
        if mask_img is None:
            print(f"Warning: Failed to load mask image: {mf}")
            continue

        # Pre-resize to target size for optimal training performance
        if img_size is not None and img_size > 0:
            cell_resized = cv2.resize(
                cell_img, (img_size, img_size), interpolation=cv2.INTER_AREA
            )
            mask_resized = cv2.resize(
                mask_img, (img_size, img_size), interpolation=cv2.INTER_NEAREST
            )
        else:
            cell_resized, mask_resized = cell_img, mask_img

        inpainted = inpaint_cilia_image(cell_resized, mask_resized)

        samples.append({
            "image": cell_resized,
            "mask": mask_resized,
            "inpainted": inpainted,
            "id": cf.stem
        })
    return samples


def prepare_classification_dataset(
    cells_dir: Union[Path, str],
    masks_pts_dir: Union[Path, str],
    val_ratio: float = 0.2,
    img_size: Optional[int] = 256,
) -> Tuple[List[Dict[str, Any]], List[Dict[str, Any]]]:

    train_pairs, val_pairs = make_pairs(
        cells_dir, masks_pts_dir, val_ratio=val_ratio)

    train_samples = build_samples(train_pairs, img_size=img_size)
    val_samples = build_samples(val_pairs, img_size=img_size)

    return train_samples, val_samples


def process_img(img: np.ndarray, preprocess, aug_transforms: Optional[Any] = None, to_rgb: bool = False) -> torch.Tensor:
    if to_rgb:
        if img.ndim == 2:
            img = np.stack([img, img, img], axis=-1)
        elif img.ndim == 3 and img.shape[2] == 1:
            img = np.concatenate([img, img, img], axis=-1)
        img = np.ascontiguousarray(img, dtype=np.uint8)
        img_tensor = torch.from_numpy(img).permute(2, 0, 1)  # (3, H, W)
    else:
        if img.ndim == 3 and img.shape[2] == 1:
            img = img.squeeze(-1)
        elif img.ndim == 3 and img.shape[2] == 3:
            img = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
        img = np.ascontiguousarray(img, dtype=np.uint8)
        img_tensor = torch.from_numpy(img).unsqueeze(0)  # (1, H, W)

    img_tv = tv_tensors.Image(img_tensor)
    if aug_transforms is not None:
        img_tv = aug_transforms(img_tv)
    return torch.as_tensor(preprocess(img_tv))


class CiliaClassificationDataset(Dataset):
    def __init__(
        self,
        samples: List[Dict[str, Any]],
        img_size: Optional[int] = 256,
        augment: bool = False,
        to_rgb: bool = False
    ):
        self.samples = samples
        self.img_size = img_size
        self.augment = augment
        self.to_rgb = to_rgb

        if self.to_rgb:
            self.normalize = v2.Compose([
                v2.ToDtype(torch.float32, scale=True),
                v2.Normalize(mean=[0.485, 0.456, 0.406],
                             std=[0.229, 0.224, 0.225])
            ])
        else:
            self.normalize = v2.Compose([
                v2.ToDtype(torch.float32, scale=True)
            ])

        self.aug_transforms = v2.Compose([
            v2.RandomHorizontalFlip(p=0.5),
            v2.RandomVerticalFlip(p=0.5),
        ]) if augment else None

    def __len__(self) -> int:
        return len(self.samples)

    def __getitem__(self, idx: int) -> Dict[str, torch.Tensor]:
        item = self.samples[idx]
        return {
            "image": process_img(item["image"], self.normalize, self.aug_transforms, self.to_rgb),
            "inpainted": process_img(item["inpainted"], self.normalize, self.aug_transforms, self.to_rgb)
        }
# ==============================================================================
# 2. Model Architectures
# ==============================================================================


class ConvBlock(nn.Module):
    def __init__(
        self,
        in_channels: int,
        out_channels: int,
        kernel_size: int = 3,
        pool_type: Literal["max", "avg", "adaptive"] = "max",
    ):
        super().__init__()
        padding = kernel_size // 2

        if pool_type == "max":
            pool = nn.MaxPool2d(2)
        elif pool_type == "avg":
            pool = nn.AvgPool2d(2)
        elif pool_type == "adaptive":
            pool = nn.AdaptiveAvgPool2d((1, 1))
        else:
            raise ValueError(f"Unsupported pool_type: {pool_type}")

        self.block = nn.Sequential(
            nn.Conv2d(in_channels, out_channels,
                      kernel_size=kernel_size, padding=padding, bias=False),
            nn.BatchNorm2d(out_channels),
            nn.ReLU(inplace=False),
            pool,
        )

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        return self.block(x)


class CustomCNN(nn.Module):
    def __init__(
        self,
        in_channels: int = 1,
        num_classes: int = 2,
        base_channels: int = 32,
        num_blocks: int = 4,
        kernel_size: int = 3,
        pool_type: Literal["max", "avg"] = "max",
        hidden_dim: int = 128,
        dropout: float = 0.3,
    ):
        super().__init__()
        if num_blocks < 1:
            raise ValueError("num_blocks at least 1")

        blocks = []
        curr_in = in_channels
        curr_out = base_channels

        for idx in range(num_blocks):
            is_last = idx == num_blocks - 1
            blocks.append(
                ConvBlock(
                    in_channels=curr_in,
                    out_channels=curr_out,
                    kernel_size=kernel_size,
                    pool_type="adaptive" if is_last else pool_type,
                )
            )
            curr_in = curr_out
            if not is_last:
                curr_out *= 2

        self.features = nn.Sequential(*blocks)

        self.classifier = nn.Sequential(
            nn.Flatten(),
            nn.Dropout(dropout),
            nn.Linear(curr_out, hidden_dim),
            nn.ReLU(inplace=False),
            nn.Dropout(dropout),
            nn.Linear(hidden_dim, num_classes),
        )

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        feat = self.features(x)
        return self.classifier(feat)


def get_pretrained_resnet(
    num_classes: int = 2,
    fine_tune: bool = True
) -> models.ResNet:
    """
    Loads ResNet18 with ImageNet weights, adapting final fully connected layer.
    """
    model = models.resnet18(weights=models.ResNet18_Weights.DEFAULT)
    if not fine_tune:
        for param in model.parameters():
            param.requires_grad = False

    # Extract input feature dimension safely for static type checkers
    in_features: int = getattr(model.fc, "in_features", 512)

    # Cast model to Any to avoid static type checker error (model.fc is typed as nn.Linear in torchvision)
    cast(Any, model).fc = nn.Sequential(
        nn.Dropout(0.2),
        nn.Linear(in_features, num_classes)
    )
    return model


# ==============================================================================
# 3. Training & Evaluation Loops
# ==============================================================================

def train_classifier(
    model: nn.Module,
    train_loader: DataLoader,
    val_loader: DataLoader,
    num_epochs: int = 15,
    lr: float = 1e-3,
    weight_decay: float = 1e-4,
    device: Union[str, torch.device] = "cpu"
) -> Dict[str, List[float]]:
    """
    Trains classification model and returns loss and accuracy history.
    """
    dev = torch.device(device)
    model = model.to(dev)
    loss_fn = nn.CrossEntropyLoss()
    optimizer = torch.optim.Adam(
        model.parameters(), lr=lr, weight_decay=weight_decay
    )

    history: Dict[str, List[float]] = {
        "train_loss": [],
        "train_acc": [],
        "val_loss": [],
        "val_acc": []
    }

    print(f"{'Epoch':<6} | {'Train Loss':<11} | {'Train Acc':<10} | {'Val Loss':<10} | {'Val Acc':<10}")
    print("-" * 56)

    for epoch in range(1, num_epochs + 1):
        model.train()
        running_loss = 0.0
        correct = 0
        total = 0

        for batch in train_loader:
            orig_imgs = batch["image"].to(dev)
            inp_imgs = batch["inpainted"].to(dev)

            imgs = torch.cat([orig_imgs, inp_imgs], dim=0)
            labels = torch.cat([
                torch.zeros(orig_imgs.size(0), dtype=torch.long, device=dev),
                torch.ones(inp_imgs.size(0), dtype=torch.long, device=dev),
            ], dim=0)

            optimizer.zero_grad()
            outputs = model(imgs)
            loss = loss_fn(outputs, labels)
            loss.backward()
            optimizer.step()

            running_loss += float(loss.item()) * imgs.size(0)
            preds = outputs.argmax(dim=1)
            correct += int((preds == labels).sum().item())
            total += int(labels.size(0))

        train_loss = running_loss / max(total, 1)
        train_acc = correct / max(total, 1)

        # Validation phase
        model.eval()
        val_loss = 0.0
        val_correct = 0
        val_total = 0

        with torch.no_grad():
            for batch in val_loader:
                orig_imgs = batch["image"].to(dev)
                inp_imgs = batch["inpainted"].to(dev)

                imgs = torch.cat([orig_imgs, inp_imgs], dim=0)
                labels = torch.cat([
                    torch.zeros(orig_imgs.size(
                        0), dtype=torch.long, device=dev),
                    torch.ones(inp_imgs.size(0), dtype=torch.long, device=dev),
                ], dim=0)

                outputs = model(imgs)
                loss = loss_fn(outputs, labels)

                val_loss += float(loss.item()) * imgs.size(0)
                preds = outputs.argmax(dim=1)
                val_correct += int((preds == labels).sum().item())
                val_total += int(labels.size(0))

        val_loss /= max(val_total, 1)
        val_acc = val_correct / max(val_total, 1)

        history["train_loss"].append(train_loss)
        history["train_acc"].append(train_acc)
        history["val_loss"].append(val_loss)
        history["val_acc"].append(val_acc)

        print(f"{epoch:<6} | {train_loss:<11.4f} | {train_acc:<10.4f} | {val_loss:<10.4f} | {val_acc:<10.4f}")

    return history


def compute_classification_metrics(
    model: nn.Module,
    val_loader: DataLoader,
    device: Union[str, torch.device] = "cpu"
) -> Dict[str, Any]:
    """
    Computes confusion matrix, precision, recall, F1-score, and accuracy.
    """
    dev = torch.device(device)
    model.to(dev)
    model.eval()
    all_preds_list: List[int] = []
    all_targets_list: List[int] = []

    with torch.no_grad():
        for batch in val_loader:
            orig_imgs = batch["image"].to(dev)
            inp_imgs = batch["inpainted"].to(dev)

            imgs = torch.cat([orig_imgs, inp_imgs], dim=0)
            labels = torch.cat([
                torch.zeros(orig_imgs.size(0), dtype=torch.long, device=dev),
                torch.ones(inp_imgs.size(0), dtype=torch.long, device=dev),
            ], dim=0)

            outputs = model(imgs)
            preds = outputs.argmax(dim=1).cpu().numpy().tolist()
            all_preds_list.extend(preds)
            all_targets_list.extend(labels.cpu().numpy().tolist())

    all_preds = np.array(all_preds_list, dtype=np.int32)
    all_targets = np.array(all_targets_list, dtype=np.int32)

    # 0 = Original, 1 = Inpainted
    tp = int(np.sum((all_preds == 1) & (all_targets == 1)))
    tn = int(np.sum((all_preds == 0) & (all_targets == 0)))
    fp = int(np.sum((all_preds == 1) & (all_targets == 0)))
    fn = int(np.sum((all_preds == 0) & (all_targets == 1)))

    cm = np.array([[tn, fp], [fn, tp]], dtype=np.int32)
    acc = float((tp + tn) / max(tp + tn + fp + fn, 1))
    prec = float(tp / max(tp + fp, 1))
    rec = float(tp / max(tp + fn, 1))
    f1 = float(2 * prec * rec / max(prec + rec, 1e-8))

    return {
        "confusion_matrix": cm,
        "accuracy": acc,
        "precision": prec,
        "recall": rec,
        "f1": f1,
        "tp": tp, "tn": tn, "fp": fp, "fn": fn,
        "all_preds": all_preds,
        "all_targets": all_targets
    }


# ==============================================================================
# 4. Model Interpretability (XAI)
# ==============================================================================

class GradCAM:
    """
    Grad-CAM implementation for PyTorch CNN architectures.
    """

    def __init__(self, model: nn.Module, target_layer: nn.Module):
        self.model = model
        self.target_layer = target_layer
        self.gradients: Optional[torch.Tensor] = None
        self.activations: Optional[torch.Tensor] = None
        self._hook_handles: List[Any] = []
        self._register_hooks()

    def _register_hooks(self) -> None:
        def forward_hook(module: nn.Module, input: Any, output: torch.Tensor) -> None:
            self.activations = output.detach()
            if output.requires_grad:
                def hook_grad(grad: torch.Tensor) -> None:
                    self.gradients = grad.detach()
                output.register_hook(hook_grad)

        h = self.target_layer.register_forward_hook(forward_hook)
        self._hook_handles.append(h)

    def generate(self, input_tensor: torch.Tensor, target_class: Optional[int] = None) -> np.ndarray:
        """
        Generates 2D Grad-CAM heatmap normalized to [0, 1].
        input_tensor: shape (1, C, H, W)
        """
        self.model.eval()
        self.model.zero_grad()

        inp = input_tensor.clone().detach().requires_grad_(True)
        output = self.model(inp)
        if target_class is None:
            target_class = int(output.argmax(dim=1).item())

        score = output[0, target_class]
        score.backward()

        if self.gradients is None or self.activations is None:
            raise RuntimeError(
                "Gradients or activations were not captured. Check target_layer.")

        # Global average pooling of gradients
        weights = torch.mean(self.gradients, dim=(
            2, 3), keepdim=True)  # (1, K, 1, 1)
        cam = torch.sum(weights * self.activations, dim=1,
                        keepdim=True)  # (1, 1, H, W)
        cam = F.relu(cam)

        cam = F.interpolate(
            cam,
            size=(input_tensor.shape[2], input_tensor.shape[3]),
            mode="bilinear",
            align_corners=False
        )
        cam_np = cam[0, 0].cpu().numpy()

        return normalize_map(cam_np)

    def remove_hooks(self) -> None:
        for h in self._hook_handles:
            h.remove()
        self._hook_handles.clear()


def normalize_map(map: np.ndarray) -> np.ndarray:
    s_min, s_max = float(map.min()), float(map.max())
    if s_max > s_min:
        map = (map - s_min) / (s_max - s_min)
    else:
        map = np.zeros_like(map)
    return map


def compute_saliency_map(
    model: nn.Module,
    input_tensor: torch.Tensor,
    target_class: Optional[int] = None
) -> np.ndarray:
    """
    Computes Vanilla Saliency map (magnitude of gradient wrt input).
    """
    model.eval()
    inp = input_tensor.clone().detach().requires_grad_(True)
    output = model(inp)
    if target_class is None:
        target_class = int(output.argmax(dim=1).item())

    score = output[0, target_class]
    score.backward()

    if inp.grad is None:
        return np.zeros((input_tensor.shape[2], input_tensor.shape[3]), dtype=np.float32)

    # Max absolute gradient across channels
    saliency = torch.max(torch.abs(inp.grad[0]), dim=0)[
        0].detach().cpu().numpy()

    return normalize_map(saliency)


def compute_integrated_gradients(
    model: nn.Module,
    input_tensor: torch.Tensor,
    target_class: Optional[int] = None,
    steps: int = 30,
    baseline: Optional[torch.Tensor] = None
) -> np.ndarray:
    """
    Computes Integrated Gradients attribution map.
    """
    model.eval()
    if baseline is None:
        baseline = torch.zeros_like(input_tensor)

    if target_class is None:
        with torch.no_grad():
            output = model(input_tensor)
            target_class = int(output.argmax(dim=1).item())

    accumulated_grads = torch.zeros_like(input_tensor)
    diff = input_tensor - baseline

    for alpha in np.linspace(0.0, 1.0, steps):
        interpolated = (baseline + float(alpha) *
                        diff).clone().detach().requires_grad_(True)
        out = model(interpolated)
        score = out[0, target_class]
        score.backward()
        if interpolated.grad is not None:
            accumulated_grads += interpolated.grad

    avg_grads = accumulated_grads / max(steps, 1)
    ig = torch.abs(diff * avg_grads)
    ig_map = torch.max(ig[0], dim=0)[0].detach().cpu().numpy()

    return normalize_map(ig_map)


def overlay_cam(
    image: np.ndarray,
    cam_heatmap: np.ndarray,
    alpha: float = 0.5,
    colormap: int = cv2.COLORMAP_JET
) -> np.ndarray:
    """
    Overlays a normalized [0, 1] 2D heatmap on an RGB uint8 image.
    """
    h, w = image.shape[:2]
    cam_resized = cv2.resize((cam_heatmap * 255).astype(np.uint8), (w, h))
    color_map = cv2.applyColorMap(cam_resized, colormap)
    color_map_rgb = cv2.cvtColor(color_map, cv2.COLOR_BGR2RGB)

    img_uint8 = image.copy()
    if img_uint8.dtype != np.uint8:
        if img_uint8.max() <= 1.0:
            img_uint8 = (img_uint8 * 255).astype(np.uint8)
        else:
            img_uint8 = img_uint8.astype(np.uint8)

    if img_uint8.ndim == 2:
        img_uint8 = cv2.cvtColor(img_uint8, cv2.COLOR_GRAY2RGB)
    elif img_uint8.ndim == 3 and img_uint8.shape[2] == 1:
        img_uint8 = cv2.cvtColor(img_uint8, cv2.COLOR_GRAY2RGB)

    overlay = cv2.addWeighted(img_uint8, 1.0 - alpha,
                              color_map_rgb, alpha, 0.0)
    return overlay


# ==============================================================================
# 5. Siamese Network & Contrastive Learning
# ==============================================================================

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


def compute_pca_2d(embeddings: np.ndarray) -> np.ndarray:
    """
    Projects embeddings to 2D using SVD (standard PCA).
    """
    emb = np.asarray(embeddings, dtype=np.float32)
    mean = np.mean(emb, axis=0)
    centered = emb - mean
    _, _, vt = np.linalg.svd(centered, full_matrices=False)
    proj = centered @ vt[:2].T
    return proj


# ==============================================================================
# 6. High-level Visualizations (Inline in Notebook, No Popups)
# ==============================================================================

def save_figure(fig: Figure, save_path: Optional[Union[Path, str]] = None) -> None:
    if save_path:
        sp = Path(save_path)
        sp.parent.mkdir(parents=True, exist_ok=True)
        fig.savefig(str(sp), dpi=200, bbox_inches="tight")


def plot_dataset_samples(
        samples: List[Dict[str, Any]],
        num_examples: int = 3,
        save_path: Optional[Union[Path, str]] = None):
    """
    Plots original cells, corresponding masks, and inpainted results.
    """
    n = min(num_examples, len(samples))
    fig, axes = plt.subplots(n, 3, figsize=(14, 3.2 * n))
    if n == 1:
        axes = np.expand_dims(axes, 0)

    for i in range(n):
        s = samples[i]

        axes[i, 0].imshow(s["image"], cmap="gray")
        axes[i, 0].set_title(f"Original Cilia ({s['id']})")
        axes[i, 0].axis("off")

        axes[i, 1].imshow(s["mask"], cmap="gray")
        axes[i, 1].set_title("Mask")
        axes[i, 1].axis("off")

        axes[i, 2].imshow(s["inpainted"], cmap="gray")
        axes[i, 2].set_title("Inpainted Cilia")
        axes[i, 2].axis("off")

    plt.tight_layout()
    save_figure(fig, save_path)


def plot_training_comparison(
        hist_custom: Dict[str, List[float]],
        hist_resnet: Dict[str, List[float]],
        save_path: Optional[Union[Path, str]] = None):
    """
    Plots training and validation loss and accuracy comparison.
    """
    epochs_custom = range(1, len(hist_custom["train_loss"]) + 1)
    epochs_resnet = range(1, len(hist_resnet["train_loss"]) + 1)

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

    # Loss
    ax1.plot(epochs_custom,
             hist_custom["train_loss"], "b--", label="Custom CNN (Train)")
    ax1.plot(epochs_custom, hist_custom["val_loss"],
             "b-", lw=2, label="Custom CNN (Val)")
    ax1.plot(epochs_resnet,
             hist_resnet["train_loss"], "r--", label="ResNet18 (Train)")
    ax1.plot(epochs_resnet, hist_resnet["val_loss"],
             "r-", lw=2, label="ResNet18 (Val)")
    ax1.set_title("Loss Curves Comparison", fontsize=13, fontweight="bold")
    ax1.set_xlabel("Epoch", fontsize=11)
    ax1.set_ylabel("Cross Entropy Loss", fontsize=11)
    ax1.grid(True, linestyle="--", alpha=0.5)
    ax1.legend(fontsize=10)

    # Accuracy
    ax2.plot(epochs_custom,
             hist_custom["train_acc"], "b--", label="Custom CNN (Train)")
    ax2.plot(epochs_custom, hist_custom["val_acc"],
             "b-", lw=2, label="Custom CNN (Val)")
    ax2.plot(epochs_resnet,
             hist_resnet["train_acc"], "r--", label="ResNet18 (Train)")
    ax2.plot(epochs_resnet, hist_resnet["val_acc"],
             "r-", lw=2, label="ResNet18 (Val)")
    ax2.set_title("Accuracy Curves Comparison", fontsize=13, fontweight="bold")
    ax2.set_xlabel("Epoch", fontsize=11)
    ax2.set_ylabel("Accuracy", fontsize=11)
    ax2.grid(True, linestyle="--", alpha=0.5)
    ax2.legend(fontsize=10)

    plt.tight_layout()
    save_figure(fig, save_path)


def plot_confusion_matrices(
        metrics_custom: Dict[str, Any],
        metrics_resnet: Dict[str, Any],
        class_names: Optional[List[str]] = None,
        save_path: Optional[Union[Path, str]] = None):
    """
    Plots side-by-side confusion matrices with metric annotations.
    """
    if class_names is None:
        class_names = ["Original", "Inpainted"]

    fig, axes = plt.subplots(1, 2, figsize=(13, 5))

    for ax, metrics, title in zip(
        axes,
        [metrics_custom, metrics_resnet],
        ["Custom CNN", "Pretrained ResNet18"]
    ):
        cm = metrics["confusion_matrix"]
        ax.imshow(cm, interpolation="nearest", cmap=plt.cm.Blues)
        ax.set_title(
            f"{title}\nAcc: {metrics['accuracy']:.3f} | F1: {metrics['f1']:.3f}",
            fontsize=12, fontweight="bold"
        )
        tick_marks = np.arange(len(class_names))
        ax.set_xticks(tick_marks)
        ax.set_xticklabels(class_names, fontsize=10)
        ax.set_yticks(tick_marks)
        ax.set_yticklabels(class_names, fontsize=10)
        ax.set_ylabel("True Label", fontsize=11)
        ax.set_xlabel("Predicted Label", fontsize=11)

        thresh = cm.max() / 2.0
        for i in range(cm.shape[0]):
            for j in range(cm.shape[1]):
                ax.text(
                    j, i, format(int(cm[i, j]), "d"),
                    ha="center", va="center",
                    color="white" if cm[i, j] > thresh else "black",
                    fontsize=12, fontweight="bold"
                )

    plt.tight_layout()
    save_figure(fig, save_path)


def plot_xai_samples(
        model: nn.Module,
        samples: List[Dict[str, Any]],
        target_layer: nn.Module,
        img_size: Optional[int] = 128,
        to_rgb: bool = True,
        device: Union[str, torch.device] = "cpu",
        save_path: Optional[Union[Path, str]] = None):
    """
    Generates and plots multi-column XAI analysis:
    Original/Inpainted image | Inpainting difference | Grad-CAM Overlay | Saliency Map | Integrated Gradients
    """
    dev = torch.device(device)
    model.to(dev)
    grad_cam = GradCAM(model, target_layer)

    sz = 128 if img_size is None else img_size
    if to_rgb:
        transform = v2.Compose([
            v2.Resize((sz, sz), interpolation=v2.InterpolationMode.BILINEAR),
            v2.ToDtype(torch.float32, scale=True),
            v2.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])
        ])
    else:
        transform = v2.Compose([
            v2.Resize((sz, sz), interpolation=v2.InterpolationMode.BILINEAR),
            v2.ToDtype(torch.float32, scale=True)
        ])

    # Střídá zobrazení Original (0) a Inpainted (1) pro jednotlivé vzorky
    expanded_items = []
    for idx, s in enumerate(samples):
        target_c = idx % 2  # 0 = Original, 1 = Inpainted
        img_raw = s["image"] if target_c == 0 else s["inpainted"]
        expanded_items.append((s, img_raw, target_c))

    n_samples = len(expanded_items)
    fig, axes = plt.subplots(n_samples, 5, figsize=(17, 3.4 * n_samples))
    if n_samples == 1:
        axes = np.expand_dims(axes, 0)

    for idx, (s, img_raw, target_c) in enumerate(expanded_items):
        inp_tensor = process_img(
            img_raw, transform, to_rgb=to_rgb).unsqueeze(0).to(dev)

        model.eval()
        with torch.no_grad():
            logits = model(inp_tensor)
            probs = F.softmax(logits, dim=1).cpu().numpy()[0]
            pred_class = int(np.argmax(probs))
            confidence = float(probs[pred_class])

        cam = grad_cam.generate(inp_tensor, target_class=target_c)
        saliency = compute_saliency_map(
            model, inp_tensor, target_class=target_c)
        ig = compute_integrated_gradients(
            model, inp_tensor, target_class=target_c, steps=25)

        overlay = overlay_cam(img_raw, cam, alpha=0.55)
        diff = np.abs(s["image"].astype(float) - s["inpainted"].astype(float))

        lbl_name = "Original" if target_c == 0 else "Inpainted"
        pred_name = "Original" if pred_class == 0 else "Inpainted"

        axes[idx, 0].imshow(img_raw, cmap="gray")
        axes[idx, 0].set_title(
            f"Input: {lbl_name} ({s['id']})\nPred: {pred_name} ({confidence*100:.1f}%)", fontsize=10)
        axes[idx, 0].axis("off")

        axes[idx, 1].imshow(diff, cmap="magma")
        axes[idx, 1].set_title("Inpaint Region / Diff", fontsize=10)
        axes[idx, 1].axis("off")

        axes[idx, 2].imshow(overlay)
        axes[idx, 2].set_title("Grad-CAM Overlay", fontsize=10)
        axes[idx, 2].axis("off")

        axes[idx, 3].imshow(saliency, cmap="inferno")
        axes[idx, 3].set_title("Saliency Map", fontsize=10)
        axes[idx, 3].axis("off")

        axes[idx, 4].imshow(ig, cmap="viridis")
        axes[idx, 4].set_title("Integrated Gradients", fontsize=10)
        axes[idx, 4].axis("off")

    grad_cam.remove_hooks()
    plt.tight_layout()
    save_figure(fig, save_path)


def plot_siamese_results(
        history_siamese: Dict[str, List[float]],
        pos_dists: np.ndarray,
        neg_dists: np.ndarray,
        threshold: float = 0.5,
        save_path: Optional[Union[Path, str]] = None):
    """
    Plots Siamese network loss curve and pair distance distributions.
    """
    epochs = range(1, len(history_siamese["train_loss"]) + 1)
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

    # Loss curve
    ax1.plot(epochs, history_siamese["train_loss"],
             "b-o", lw=2, label="Train Contrastive Loss")
    ax1.plot(epochs, history_siamese["val_loss"],
             "r-s", lw=2, label="Val Contrastive Loss")
    ax1.set_title("Siamese Network Training Loss",
                  fontsize=13, fontweight="bold")
    ax1.set_xlabel("Epoch", fontsize=11)
    ax1.set_ylabel("Contrastive Loss", fontsize=11)
    ax1.grid(True, linestyle="--", alpha=0.5)
    ax1.legend(fontsize=11)

    # Distance histograms
    ax2.hist(pos_dists, bins=30, alpha=0.6, color="green",
             label="Positive Pairs (Same Class)", density=True)
    ax2.hist(neg_dists, bins=30, alpha=0.6, color="red",
             label="Negative Pairs (Diff Class)", density=True)
    ax2.axvline(threshold, color="black", linestyle="--", lw=2,
                label=f"Decision Threshold ({threshold:.2f})")
    ax2.set_title("Pair Distance Distributions in Embedding Space",
                  fontsize=13, fontweight="bold")
    ax2.set_xlabel("Euclidean Distance $d(z_1, z_2)$", fontsize=11)
    ax2.set_ylabel("Density", fontsize=11)
    ax2.grid(True, linestyle="--", alpha=0.5)
    ax2.legend(fontsize=10)

    plt.tight_layout()
    if save_path:
        sp = Path(save_path)
        sp.parent.mkdir(parents=True, exist_ok=True)
        fig.savefig(str(sp), dpi=200, bbox_inches="tight")


def plot_embedding_space_pca(
        embeddings: np.ndarray,
        labels: np.ndarray,
        class_names: Optional[List[str]] = None,
        save_path: Optional[Union[Path, str]] = None):
    """
    Projects embeddings to 2D via PCA and plots scatter visualization.
    """
    if class_names is None:
        class_names = ["Original", "Inpainted"]

    pca_2d = compute_pca_2d(embeddings)

    fig = plt.figure(figsize=(8, 6))
    colors = ["#1f77b4", "#ff7f0e"]
    markers = ["o", "^"]

    for c in [0, 1]:
        mask = (labels == c)
        plt.scatter(
            pca_2d[mask, 0], pca_2d[mask, 1],
            c=colors[c], marker=markers[c],
            label=class_names[c], alpha=0.75, edgecolors="k", s=50
        )

    plt.title("2D PCA Projection of Siamese Embedding Space",
              fontsize=13, fontweight="bold")
    plt.xlabel("Principal Component 1", fontsize=11)
    plt.ylabel("Principal Component 2", fontsize=11)
    plt.grid(True, linestyle="--", alpha=0.5)
    plt.legend(fontsize=11)
    plt.tight_layout()

    save_figure(fig, save_path)


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


def extract_embeddings(
    model: SiameseNetwork,
    samples: List[Dict[str, Any]],
    img_size: Optional[int] = 128,
    device: Union[str, torch.device] = "cpu"
) -> Tuple[np.ndarray, np.ndarray]:
    dev = torch.device(device)
    model.to(dev)
    model.eval()

    sz = 128 if img_size is None else img_size
    transform = v2.Compose([
        v2.Resize((sz, sz), interpolation=v2.InterpolationMode.BILINEAR),
        v2.ToDtype(torch.float32, scale=True),
        v2.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])
    ])

    expanded = _expand_samples_for_siamese(samples)
    embs: List[np.ndarray] = []
    labels: List[int] = []

    with torch.no_grad():
        for s in expanded:
            t = process_img(s["image"], transform,
                            to_rgb=True).unsqueeze(0).to(dev)
            e = model.forward_one(t).cpu().numpy()[0]
            embs.append(e)
            labels.append(int(s["label"]))

    return np.array(embs, dtype=np.float32), np.array(labels, dtype=np.int32)


def plot_siamese_pair_examples(
        model: SiameseNetwork,
        pairs: List[Tuple[Dict[str, Any], Dict[str, Any], float]],
        num_examples: int = 4,
        img_size: Optional[int] = 128,
        threshold: float = 0.5,
        device: Union[str, torch.device] = "cpu",
        save_path: Optional[Union[Path, str]] = None):
    dev = torch.device(device)
    model.to(dev)
    model.eval()

    sz = 128 if img_size is None else img_size
    transform = v2.Compose([
        v2.Resize((sz, sz), interpolation=v2.InterpolationMode.BILINEAR),
        v2.ToDtype(torch.float32, scale=True),
        v2.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])
    ])

    n = min(num_examples, len(pairs))
    fig, axes = plt.subplots(n, 2, figsize=(8, 3.2 * n))
    if n == 1:
        axes = np.expand_dims(axes, 0)

    for i in range(n):
        s1, s2, true_label = pairs[i]
        im1, im2 = s1["image"], s2["image"]

        t1 = process_img(im1, transform, to_rgb=True).unsqueeze(0).to(dev)
        t2 = process_img(im2, transform, to_rgb=True).unsqueeze(0).to(dev)

        with torch.no_grad():
            res = model(t1, t2)
            e1, e2 = res if isinstance(res, tuple) else (res, res)
            dist = float(F.pairwise_distance(e1, e2).item())

        pred_similar = dist < threshold
        is_correct = (pred_similar == (true_label == 1.0))
        relation_str = "Similar (Positive)" if true_label == 1.0 else "Dissimilar (Negative)"
        status_str = "CORRECT" if is_correct else "WRONG"

        axes[i, 0].imshow(im1, cmap="gray")
        axes[i, 0].set_title(f"Sample 1: {s1['label_name']}", fontsize=10)
        axes[i, 0].axis("off")

        axes[i, 1].imshow(im2, cmap="gray")
        axes[i, 1].set_title(
            f"Sample 2: {s2['label_name']}\nTrue: {relation_str}\nDist: {dist:.3f} [{status_str}]",
            fontsize=10, color="green" if is_correct else "red"
        )
        axes[i, 1].axis("off")

    plt.tight_layout()
    save_figure(fig, save_path)
