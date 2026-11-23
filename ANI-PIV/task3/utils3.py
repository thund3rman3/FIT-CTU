from typing import Literal
import random
from pathlib import Path
from typing import Tuple, List, Dict, Optional, Union, Any, Sequence, cast

import cv2
import numpy as np
from matplotlib.figure import Figure

import torch
import torch.nn as nn
from torch.utils.data import Dataset, DataLoader
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
    min_tubules: int = 4,
    max_tubules: int = 9,
    max_area_factor: float = 2.5,
    method: int = cv2.INPAINT_TELEA
) -> np.ndarray:
    image, binary_mask = get_image_and_mask(image, mask_pts)

    num_labels, labels, stats, _ = cv2.connectedComponentsWithStats(
        binary_mask, connectivity=8)

    available_ids = list(range(1, num_labels))

    if len(available_ids) == 0:
        return image.copy()

    upper_bound = min(max_tubules, max(1, len(available_ids) - 1))
    lower_bound = min(min_tubules, upper_bound)
    num_to_select = random.randint(lower_bound, upper_bound)

    selected_ids = random.sample(available_ids, num_to_select)

    selected_mask = np.isin(labels, selected_ids).astype(np.uint8) * 255
    initial_area = int(np.count_nonzero(selected_mask))

    if initial_area == 0:
        return image.copy()

    max_allowed_area = int(initial_area * max_area_factor)
    kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (3, 3))
    dilated_mask = selected_mask.copy()

    while True:
        candidate = cv2.dilate(dilated_mask, kernel, iterations=1)
        new_area = int(np.count_nonzero(candidate))
        if new_area > max_allowed_area or new_area == int(np.count_nonzero(dilated_mask)):
            break
        dilated_mask = candidate

    inpainted = cv2.inpaint(image, dilated_mask, inpaint_radius, method)
    return inpainted

# Save paths of images and masks, create pairs for the whole dataset and divide into training and validation sets.


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

# Load dataset images and masks, apply inpainting, and return one dataset


def build_samples(pair_list: List[Tuple[Path, Path]],
                  img_size: Optional[int] = None) -> List[Dict[str, Any]]:
    samples: List[Dict[str, Any]] = []
    for cf, mf in pair_list:
        cell_img = cv2.imread(str(cf), cv2.IMREAD_GRAYSCALE)
        mask_img = cv2.imread(str(mf), cv2.IMREAD_GRAYSCALE)

        if cell_img is None or mask_img is None:
            print(f"Warning: Failed to load image: {cf}, {mf}")
            continue

        if img_size is not None and img_size > 0:
            cell_resized = cv2.resize(
                cell_img, (img_size, img_size), interpolation=cv2.INTER_NEAREST
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

# Load images and masks, apply inpainting, and return a list for training and validation datasets.


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

# Set dimensions of image, return tensor


def process_img(img: np.ndarray, preprocess, aug_transforms: Optional[Any] = None) -> torch.Tensor:

    if img.ndim == 2:
        img = np.stack([img, img, img], axis=-1)
    elif img.ndim == 3 and img.shape[2] == 1:
        img = np.concatenate([img, img, img], axis=-1)
    img = np.ascontiguousarray(img, dtype=np.uint8)
    img_tensor = torch.from_numpy(img).permute(2, 0, 1)  # (3, H, W)

    img_tv = tv_tensors.Image(img_tensor)
    if aug_transforms is not None:
        img_tv = aug_transforms(img_tv)
    return torch.as_tensor(preprocess(img_tv))


def normalize_map(map: np.ndarray) -> np.ndarray:
    s_min, s_max = float(map.min()), float(map.max())
    if s_max > s_min:
        map = (map - s_min) / (s_max - s_min)
    else:
        map = np.zeros_like(map)
    return map


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


def save_figure(fig: Figure, save_path: Optional[Union[Path, str]] = None) -> None:
    if save_path:
        sp = Path(save_path)
        sp.parent.mkdir(parents=True, exist_ok=True)
        fig.savefig(str(sp), dpi=200, bbox_inches="tight")

# Dataset


class CiliaClassificationDataset(Dataset):
    def __init__(
        self,
        samples: List[Dict[str, Any]],
        img_size: Optional[int] = 256,
        augment: bool = False,
    ):
        self.samples = samples
        self.img_size = img_size
        self.augment = augment

        self.normalize = v2.Compose([
            v2.ToDtype(torch.float32, scale=True),
            v2.Normalize(mean=[0.485, 0.456, 0.406],
                         std=[0.229, 0.224, 0.225])
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
            "image": process_img(item["image"], self.normalize, self.aug_transforms),
            "inpainted": process_img(item["inpainted"], self.normalize, self.aug_transforms)
        }


# Training & Evaluation Loops

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
