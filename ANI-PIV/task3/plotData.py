from pathlib import Path
from typing import Tuple, List, Dict, Optional, Union, Any

import numpy as np
import matplotlib.pyplot as plt

import torch
import torch.nn as nn
import torch.nn.functional as F
from torchvision.transforms import v2

from utils3 import *
from SiameseCNN import SiameseNetwork, _expand_samples_for_siamese
from xai import *


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
        hist_resnet_frozen: Dict[str, List[float]],
        hist_resnet_finetuned: Dict[str, List[float]],
        save_path: Optional[Union[Path, str]] = None):
    """
    Plots training and validation loss and accuracy comparison.
    """
    epochs_custom = range(1, len(hist_custom["train_loss"]) + 1)
    epochs_resnet_frozen = range(1, len(hist_resnet_frozen["train_loss"]) + 1)
    epochs_resnet_finetuned = range(
        1, len(hist_resnet_finetuned["train_loss"]) + 1)

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

    # Loss
    ax1.plot(epochs_custom,
             hist_custom["train_loss"], "b--", label="Custom CNN (Train)")
    ax1.plot(epochs_custom, hist_custom["val_loss"],
             "b-", lw=2, label="Custom CNN (Val)")
    ax1.plot(epochs_resnet_frozen,
             hist_resnet_frozen["train_loss"], "r--", label="ResNet18 frozen (Train)")
    ax1.plot(epochs_resnet_frozen, hist_resnet_frozen["val_loss"],
             "r-", lw=2, label="ResNet18 frozen (Val)")

    ax1.plot(epochs_resnet_finetuned,
             hist_resnet_finetuned["train_loss"], "g--", label="ResNet18 finetuned (Train)")
    ax1.plot(epochs_resnet_finetuned, hist_resnet_finetuned["val_loss"],
             "g-", lw=2, label="ResNet18 finetuned (Val)")

    ax1.set_title("Loss Curves Comparison", fontsize=13, fontweight="bold")
    ax1.set_xlabel("Epoch", fontsize=11)
    ax1.set_ylabel("BCE", fontsize=11)
    ax1.grid(True, linestyle="--", alpha=0.5)
    ax1.legend(fontsize=10)

    # Accuracy
    ax2.plot(epochs_custom,
             hist_custom["train_acc"], "b--", label="Custom CNN (Train)")
    ax2.plot(epochs_custom, hist_custom["val_acc"],
             "b-", lw=2, label="Custom CNN (Val)")
    ax2.plot(epochs_resnet_frozen,
             hist_resnet_frozen["train_acc"], "r--", label="ResNet18 frozen (Train)")
    ax2.plot(epochs_resnet_frozen, hist_resnet_frozen["val_acc"],
             "r-", lw=2, label="ResNet18 frozen (Val)")

    ax2.plot(epochs_resnet_finetuned,
             hist_resnet_finetuned["train_acc"], "g--", label="ResNet18 finetuned (Train)")
    ax2.plot(epochs_resnet_finetuned, hist_resnet_finetuned["val_acc"],
             "g-", lw=2, label="ResNet18 finedtuned (Val)")

    ax2.set_title("Accuracy Curves Comparison", fontsize=13, fontweight="bold")
    ax2.set_xlabel("Epoch", fontsize=11)
    ax2.set_ylabel("Accuracy", fontsize=11)
    ax2.grid(True, linestyle="--", alpha=0.5)
    ax2.legend(fontsize=10)

    plt.tight_layout()
    save_figure(fig, save_path)


def plot_confusion_matrices(
        metrics_custom: Dict[str, Any],
        metrics_resnet_finetuned: Dict[str, Any],
        metrics_resnet_frozen: Dict[str, Any],
        class_names: Optional[List[str]] = None,
        save_path: Optional[Union[Path, str]] = None):
    """
    Plots side-by-side confusion matrices with metric annotations.
    """
    if class_names is None:
        class_names = ["Original", "Inpainted"]

    fig, axes = plt.subplots(1, 3, figsize=(13, 5))

    for ax, metrics, title in zip(
        axes,
        [metrics_custom, metrics_resnet_finetuned, metrics_resnet_frozen],
        ["Custom CNN", "finetuned ResNet18", "frozen ResNet18"]
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
            img_raw, transform).unsqueeze(0).to(dev)

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
            t = process_img(s["image"], transform).unsqueeze(0).to(dev)
            e = model.forward_one(t).cpu().numpy()[0]
            embs.append(e)
            labels.append(int(s["label"]))

    return np.array(embs, dtype=np.float32), np.array(labels, dtype=np.int32)


def plot_siamese_pair_examples(
        model: SiameseNetwork,
        pairs: List[Tuple[Dict[str, Any], Dict[str, Any], float]],
        num_examples: int = 4,
        img_size: int = 256,
        threshold: float = 0.5,
        device: Union[str, torch.device] = "cpu",
        save_path: Optional[Union[Path, str]] = None):
    dev = torch.device(device)
    model.to(dev)
    model.eval()

    transform = v2.Compose([
        v2.Resize((img_size, img_size),
                  interpolation=v2.InterpolationMode.BILINEAR),
        v2.ToDtype(torch.float32, scale=True),
        v2.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225])
    ])

    n = min(num_examples, len(pairs))
    fig, axes = plt.subplots(n, 2, figsize=(9.5, 3.4 * n))
    if n == 1:
        axes = np.expand_dims(axes, 0)

    # Větší mezera (wspace) mezi oběma obrázky pro text uprostřed
    plt.subplots_adjust(wspace=0.45, hspace=0.35)

    for i in range(n):
        s1, s2, true_label = pairs[i]
        im1, im2 = s1["image"], s2["image"]

        t1 = process_img(im1, transform).unsqueeze(0).to(dev)
        t2 = process_img(im2, transform).unsqueeze(0).to(dev)

        with torch.no_grad():
            res = model(t1, t2)
            e1, e2 = res if isinstance(res, tuple) else (res, res)
            dist = float(F.pairwise_distance(e1, e2).item())

        pred_similar = dist < threshold
        is_correct = (pred_similar == (true_label == 1.0))
        relation_str = "Similar\n(Positive)" if true_label == 1.0 else "Dissimilar\n(Negative)"
        status_str = f"[{'CORRECT' if is_correct else 'WRONG'}]"
        status_color = "green" if is_correct else "red"

        axes[i, 0].imshow(im1, cmap="gray")
        axes[i, 0].set_title(
            f"Sample 1: {s1['label_name']}", fontsize=10, color="black")
        axes[i, 0].axis("off")

        axes[i, 1].imshow(im2, cmap="gray")
        axes[i, 1].set_title(
            f"Sample 2: {s2['label_name']}", fontsize=10, color="black")
        axes[i, 1].axis("off")

        axes[i, 0].text(
            1.225, 0.58,
            f"True:\n{relation_str}\n\nDist: {dist:.3f}",
            transform=axes[i, 0].transAxes,
            ha="center", va="center", fontsize=9.5, color="black"
        )
        axes[i, 0].text(
            1.225, 0.28,
            status_str,
            transform=axes[i, 0].transAxes,
            ha="center", va="center", fontsize=10.5, fontweight="bold", color=status_color
        )

    plt.tight_layout()
    save_figure(fig, save_path)
