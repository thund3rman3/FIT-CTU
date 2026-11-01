import torch
from torch import nn
from torch.utils.data import DataLoader
from torchvision import datasets
from torchvision.transforms import v2
from torchvision.io import read_image, ImageReadMode
from torchvision import tv_tensors
import torch.nn.functional as F

import cv2
import numpy as np
import random
import matplotlib.pyplot as plt

def find_contours_util(image: np.ndarray):
    contours, _ = cv2.findContours(
        image, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    return contours


def draw_contours_util(rgb_image: np.ndarray, contours: list, color: tuple = (0, 0, 255), fill: bool = False) -> np.ndarray:
    thick = cv2.FILLED if fill else 1
    return cv2.drawContours(
        rgb_image.copy(),
        contours,
        -1,  # all contours
        color,
        thickness=thick,
    )

class CiliaDataset(torch.utils.data.Dataset):
    def __init__(self, pairs, img_size=256, augment=False):
        self.pairs = pairs
        self.img_size = img_size

        self.base_transforms = v2.Compose([
            v2.Resize((img_size, img_size),
                      interpolation=v2.InterpolationMode.BILINEAR),
            v2.ToDtype(torch.float32, scale=True)
        ])
        self.aug_transforms = v2.Compose([
            v2.RandomHorizontalFlip(p=0.5),
            v2.RandomVerticalFlip(p=0.5)
        ]) if augment else None

    def __len__(self):
        return len(self.pairs)

    def __getitem__(self, idx):
        cell_path, mask_path = self.pairs[idx]
        cell = cv2.imread(cell_path, cv2.IMREAD_GRAYSCALE)
        mask = cv2.imread(mask_path, cv2.IMREAD_GRAYSCALE)

        mask = (mask > 127).astype(np.float32)

        img_tensor = torch.from_numpy(cell).unsqueeze(0)
        mask_tensor = torch.from_numpy(mask).unsqueeze(0)
        img_tv = tv_tensors.Image(img_tensor)
        mask_tv = tv_tensors.Mask(mask_tensor)

        img_tv, mask_tv = self.base_transforms(img_tv, mask_tv)

        if self.aug_transforms:
            img_tv, mask_tv = self.aug_transforms(
                img_tv, mask_tv)

        return img_tv, mask_tv


class DoubleConv(nn.Module):
    def __init__(self, in_channels, out_channels, kernel_size=3):
        super().__init__()
        padding = kernel_size // 2

        self.conv = nn.Sequential(
            nn.Conv2d(in_channels, out_channels, kernel_size, padding=padding),
            nn.BatchNorm2d(out_channels),
            nn.ReLU(inplace=True),
            nn.Conv2d(out_channels, out_channels, kernel_size, padding=padding),
            nn.BatchNorm2d(out_channels),
            nn.ReLU(inplace=True)
        )

    def forward(self, x):
        return self.conv(x)


class UNet(nn.Module):
    def __init__(self, in_channels=1, out_channels=1, base_channels=32, depth=3, kernel_size=3):
        super().__init__()
        self.depth = depth
        self.kernel_size = kernel_size
        self.encoders = nn.ModuleList()
        self.decoders = nn.ModuleList()
        self.pool = nn.MaxPool2d(2)
        self.up = nn.Upsample(scale_factor=2, mode='bilinear', align_corners=True)

        # 1. Encode
        current_in = in_channels
        current_out = base_channels
        
        for i in range(depth):
            self.encoders.append(DoubleConv(current_in, current_out, kernel_size=kernel_size))
            current_in = current_out
            current_out = current_out * 2

        self.bottleneck = DoubleConv(current_in, current_out, kernel_size=kernel_size)

        # 2. Decode
        for i in range(depth):
            skip_channels = current_in
            up_channels = current_out
            
            self.decoders.append(DoubleConv(up_channels + skip_channels, skip_channels, kernel_size=kernel_size))
            
            current_out = skip_channels
            current_in = skip_channels // 2

        self.out_conv = nn.Conv2d(base_channels, out_channels, kernel_size=1)

    def forward(self, x):
        skip_connections = []

        # Enkodér: Průchod vrstvami a ukládání přeskočených vazeb
        for encoder in self.encoders:
            x = encoder(x)
            skip_connections.append(x)
            x = self.pool(x)

        # Bottleneck
        x = self.bottleneck(x)

        # Dekodér: Zvětšování rozlišení a spojování se skip connections
        skip_connections = skip_connections[::-1]  # Obrátíme pořadí pro dekodér
        
        for i, decoder in enumerate(self.decoders):
            x = self.up(x)
            skip = skip_connections[i]
            if x.shape[2:] != skip.shape[2:]:
                x = F.interpolate(x, size=skip.shape[2:], mode='bilinear', align_corners=True)
            x = torch.cat([x, skip], dim=1)
            x = decoder(x)

        return self.out_conv(x)


def evaluate_model(model, val_dataloader, loss_fn, device, epoch, history, epoch_outs, preview_img, train_loss):
    model.eval()
    
    val_loss, val_dice = 0.0, 0.0
    val_acc, val_prec, val_rec, val_f1 = 0.0, 0.0, 0.0, 0.0
    
    with torch.no_grad():
        for imgs, masks in val_dataloader:
            imgs, masks = imgs.to(device), masks.to(device)
            # Get predictions from the model, returns logits    
            outputs = model(imgs)
            val_loss += loss_fn(outputs, masks).item()
            val_dice += dice_coeff(outputs, masks)
            
            acc, prec, rec, f1 = calculate_metrics(
                outputs, masks)
            val_acc += acc
            val_prec += prec
            val_rec += rec
            val_f1 += f1

        preview_logits = model(preview_img.unsqueeze(0).to(device))
        preview_pred = (torch.sigmoid(preview_logits) > 0.5).float()

    pred_show = (preview_pred[0].cpu().numpy().squeeze() * 255).astype(np.uint8)
    epoch_outs.append(pred_show)

    num_batches = len(val_dataloader)
    
    epoch_metrics = {
        'epoch': epoch + 1,
        'train_loss': train_loss,
        'val_loss': val_loss / num_batches,
        'val_dice': val_dice / num_batches,
        'val_acc': val_acc / num_batches,
        'val_prec': val_prec / num_batches,
        'val_rec': val_rec / num_batches,
        'val_f1': val_f1 / num_batches
    }

    history.append(epoch_metrics)

    print(f"Epoch [{epoch_metrics['epoch']}] | "
          f"Train Loss: {epoch_metrics['train_loss']:.4f} | "
          f"Val Loss: {epoch_metrics['val_loss']:.4f} | "
          f"Dice: {epoch_metrics['val_dice']:.4f} | "
          f"Acc: {epoch_metrics['val_acc']:.4f} | "
          f"Prec: {epoch_metrics['val_prec']:.4f} | "
          f"Rec: {epoch_metrics['val_rec']:.4f} | " 
          f"F1: {epoch_metrics['val_f1']:.4f} | ")

def dice_coeff(pred, target, smooth=1e-6):
    pred = (torch.sigmoid(pred) > 0.5).float()
    intersection = (pred * target).sum(dim=(2, 3))
    union = pred.sum(dim=(2, 3)) + target.sum(dim=(2, 3))
    return ((2. * intersection + smooth) / (union + smooth)).mean().item()


def calculate_metrics(pred, target):
    pred = (torch.sigmoid(pred) > 0.5).int().flatten()
    target = target.int().flatten()

    TP = ((pred == 1) & (target == 1)).sum().item()
    TN = ((pred == 0) & (target == 0)).sum().item()
    FP = ((pred == 1) & (target == 0)).sum().item()
    FN = ((pred == 0) & (target == 1)).sum().item()

    eps = 1e-7
    accuracy = (TP + TN) / (TP + TN + FP + FN)
    precision = TP / (TP + FP + eps)
    recall = TP / (TP + FN + eps)
    f1_score = 2 * (precision * recall) / (precision + recall + eps)

    return accuracy, precision, recall, f1_score


def calculate_dataset_metrics_and_cm(model, dataloader, device='cuda', threshold=0.5):
    model.eval()
    total_TP, total_TN, total_FP, total_FN = 0, 0, 0, 0
    with torch.no_grad():
        for imgs, masks in dataloader:
            imgs = imgs.to(device)
            masks = masks.to(device)
            preds = (torch.sigmoid(model(imgs)) > threshold).int().flatten()
            targets = masks.int().flatten()

            total_TP += ((preds == 1) & (targets == 1)).sum().item()
            total_TN += ((preds == 0) & (targets == 0)).sum().item()
            total_FP += ((preds == 1) & (targets == 0)).sum().item()
            total_FN += ((preds == 0) & (targets == 1)).sum().item()

    eps = 1e-7
    accuracy = (total_TP + total_TN) / (total_TP + total_TN + total_FP + total_FN)
    precision = total_TP / (total_TP + total_FP + eps)
    recall = total_TP / (total_TP + total_FN + eps)
    f1_score = 2 * (precision * recall) / (precision + recall + eps)
    dice = 2 * total_TP / (2 * total_TP + total_FP + total_FN + eps)

    return {
        'TP': total_TP,
        'TN': total_TN,
        'FP': total_FP,
        'FN': total_FN,
        'accuracy': accuracy,
        'precision': precision,
        'recall': recall,
        'f1_score': f1_score,
        'dice': dice
    }


def plot_confusion_matrix(metrics_cm, title="Confusion Matrix (BCE, Epocha 10)", save_path=None):
    TP = metrics_cm['TP']
    TN = metrics_cm['TN']
    FP = metrics_cm['FP']
    FN = metrics_cm['FN']
    total = TP + TN + FP + FN

    cm = np.array([[TN, FP], [FN, TP]])
    cm_norm = cm / total * 100

    labels = [
        [f"TN (True Neg)\n{TN:,}\n({cm_norm[0, 0]:.2f} %)", f"FP (False Pos)\n{FP:,}\n({cm_norm[0, 1]:.2f} %)"],
        [f"FN (False Neg)\n{FN:,}\n({cm_norm[1, 0]:.2f} %)", f"TP (True Pos)\n{TP:,}\n({cm_norm[1, 1]:.2f} %)"]
    ]

    fig, ax = plt.subplots(figsize=(6.5, 5.5))
    cax = ax.matshow(cm, cmap='Blues')
    fig.colorbar(cax, fraction=0.046, pad=0.04)

    for i in range(2):
        for j in range(2):
            color = "white" if cm[i, j] > cm.max() / 2 else "black"
            ax.text(j, i, labels[i][j], ha="center", va="center", color=color, fontsize=11, fontweight="bold")

    ax.set_xticks([0, 1])
    ax.set_yticks([0, 1])
    ax.set_xticklabels(['BG (0)', 'Microtubules (1)'], fontsize=10, fontweight="bold")
    ax.set_yticklabels(['BG (0)', 'Microtubules (1)'], fontsize=10, fontweight="bold")
    ax.set_xlabel('Prediction', fontsize=11, fontweight="bold", labelpad=10)
    ax.set_ylabel('Reality (Ground Truth)', fontsize=11, fontweight="bold")
    ax.set_title(title, pad=18, fontsize=12, fontweight="bold")

    plt.tight_layout()
    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
    return fig


class DiceLoss(nn.Module):
    def __init__(self, smooth=1e-6):
        super().__init__()
        self.smooth = smooth

    def forward(self, pred, target):
        pred = torch.sigmoid(pred)
        intersection = (pred * target).sum(dim=(2, 3))
        union = pred.sum(dim=(2, 3)) + target.sum(dim=(2, 3))
        dice_score = (2. * intersection + self.smooth) / (union + self.smooth)
        return 1.0 - dice_score.mean()


class BCEDiceLoss(nn.Module):
    def __init__(self, alpha=0.5, smooth=1e-6):
        super().__init__()
        self.alpha = alpha 
        self.bce = nn.BCEWithLogitsLoss()
        self.dice = DiceLoss(smooth=smooth)

    def forward(self, pred, target):
        return self.alpha * self.bce(pred, target) + (1 - self.alpha) * self.dice(pred, target)


class FocalLoss(nn.Module):
    def __init__(self, alpha=0.25, gamma=2.0):
        super().__init__()
        self.alpha = alpha
        self.gamma = gamma

    def forward(self, pred, target):
        bce_loss = F.binary_cross_entropy_with_logits(pred, target, reduction='none')
        pt = torch.exp(-bce_loss) 
        focal_loss = self.alpha * ((1 - pt) ** self.gamma) * bce_loss
        return focal_loss.mean()


class TverskyLoss(nn.Module):
    def __init__(self, alpha=0.7, beta=0.3, smooth=1e-6):
        super().__init__()
        self.alpha = alpha  
        self.beta = beta   
        self.smooth = smooth

    def forward(self, pred, target):
        pred = torch.sigmoid(pred)
        tp = (pred * target).sum(dim=(2, 3))
        fp = (pred * (1 - target)).sum(dim=(2, 3))
        fn = ((1 - pred) * target).sum(dim=(2, 3))
        
        tversky = (tp + self.smooth) / (tp + self.alpha * fn + self.beta * fp + self.smooth)
        return 1.0 - tversky.mean()


def plot_train_loss_dice(history, epochs):
    

    def get_metric(key):
        if isinstance(history, dict):
            return history[key]
        return [h[key] for h in history]

    plt.figure(figsize=(12, 5))

    plt.subplot(1, 2, 1)
    plt.plot(epochs, get_metric('train_loss'), label='Train Loss', color='blue', marker='o')
    plt.plot(epochs, get_metric('val_loss'), label='Val Loss', color='red', marker='o')
    plt.title('Loss function during training and validation')
    plt.xlabel('Epoch')
    plt.ylabel('Loss')
    plt.xticks(epochs)
    plt.legend()
    plt.grid(True)

    plt.subplot(1, 2, 2)
    plt.plot(epochs, get_metric('val_dice'), label='Val Dice', color='green', marker='o')
    plt.title('Dice Score on Validation Set')
    plt.xlabel('Epoch')
    plt.ylabel('Dice Coefficient (0.0 - 1.0)')
    plt.xticks(epochs)
    plt.legend()
    plt.grid(True)

    plt.tight_layout()
    plt.show()


def plot_metrics(history, epochs):
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))

    def get_metric(key):
            if isinstance(history, dict):
                return history[key]
            return [h[key] for h in history]

    # Accuracy
    axes[0, 0].plot(epochs, get_metric('val_acc'), label='Val Accuracy', color='#6A0DAD', marker='o')
    axes[0, 0].set_title('Validation Accuracy')
    axes[0, 0].set_xlabel('Epoch')
    axes[0, 0].set_ylabel('Accuracy')
    axes[0, 0].set_xticks(epochs)
    axes[0, 0].legend()
    axes[0, 0].grid(True)

    # Precision
    axes[0, 1].plot(epochs, get_metric('val_prec'), label='Val Precision', color='#FF7F0E', marker='o')
    axes[0, 1].set_title('Validation Precision')
    axes[0, 1].set_xlabel('Epoch')
    axes[0, 1].set_ylabel('Precision')
    axes[0, 1].set_xticks(epochs)
    axes[0, 1].legend()
    axes[0, 1].grid(True)

    # Recall
    axes[1, 0].plot(epochs, get_metric('val_rec'), label='Val Recall', color='#D62728', marker='o')
    axes[1, 0].set_title('Validation Recall')
    axes[1, 0].set_xlabel('Epoch')
    axes[1, 0].set_ylabel('Recall')
    axes[1, 0].set_xticks(epochs)
    axes[1, 0].legend()
    axes[1, 0].grid(True)

    # F1-Score vs Dice Score
    axes[1, 1].plot(epochs, get_metric('val_f1'), label='Val F1-Score', color='#1F77B4', marker='o')
    axes[1, 1].plot(epochs, get_metric('val_dice'), label='Val Dice Score', color='#2CA02C', marker='s', linestyle='--')
    axes[1, 1].set_title('Validation F1-Score vs Dice Score')
    axes[1, 1].set_xlabel('Epoch')
    axes[1, 1].set_ylabel('Score (0.0 - 1.0)')
    axes[1, 1].set_xticks(epochs)
    axes[1, 1].legend()
    axes[1, 1].grid(True)

    plt.tight_layout()
    plt.show()

    plt.figure(figsize=(11, 5))
    plt.plot(epochs, get_metric('val_acc'), label='Accuracy', color='#6A0DAD', marker='^', linestyle=':', alpha=0.85)
    plt.plot(epochs, get_metric('val_prec'), label='Precision', color='#FF7F0E', marker='o')
    plt.plot(epochs, get_metric('val_rec'), label='Recall', color='#D62728', marker='s')
    plt.plot(epochs, get_metric('val_f1'), label='F1-Score', color='#1F77B4', marker='D', linewidth=2)
    plt.plot(epochs, get_metric('val_dice'), label='Dice Score', color='#2CA02C', marker='x', linestyle='--')
    plt.title('Comparison of All Validation Metrics Across Epochs')
    plt.xlabel('Epoch')
    plt.ylabel('Metric Value (0.0 - 1.0)')
    plt.xticks(epochs)
    plt.ylim(0, 1.05)
    plt.legend(loc='lower right')
    plt.grid(True)
    plt.tight_layout()
    plt.show()