from typing import Optional

import cv2
import numpy as np

import torch
import torch.nn as nn
from captum.attr import Saliency, IntegratedGradients, LayerGradCam, GuidedGradCam, LayerAttribution

from utils3 import normalize_map


def _set_relu_inplace(model: nn.Module, inplace: bool = False) -> list:
    """Temporarily sets inplace=False on ReLU layers for Captum GuidedBackprop/GuidedGradCam."""
    changed = []
    for m in model.modules():
        if isinstance(m, nn.ReLU) and m.inplace != inplace:
            changed.append((m, m.inplace))
            m.inplace = inplace
    return changed


def _restore_relu_inplace(changed: list) -> None:
    for m, orig_inplace in changed:
        m.inplace = orig_inplace


class GradCAM:
    """
    Captum LayerGradCam wrapper for PyTorch CNN architectures.
    """

    def __init__(self, model: nn.Module, target_layer: nn.Module):
        self.model = model
        self.target_layer = target_layer
        self.layer_gc = LayerGradCam(model, target_layer)

    def generate(self, input_tensor: torch.Tensor, target_class: Optional[int] = None) -> np.ndarray:
        """
        Generates 2D Grad-CAM heatmap normalized to [0, 1] using Captum LayerGradCam.
        input_tensor: shape (1, C, H, W)
        """
        self.model.eval()
        inp = input_tensor.clone().detach().requires_grad_(True)
        if target_class is None:
            with torch.no_grad():
                target_class = int(self.model(inp).argmax(dim=1).item())

        attr = self.layer_gc.attribute(inp, target=target_class, relu_attributions=True)
        upsampled = LayerAttribution.interpolate(
            attr,
            (input_tensor.shape[2], input_tensor.shape[3]),
            interpolate_mode="bilinear",
        )
        cam_np = upsampled[0, 0].detach().cpu().numpy()
        return normalize_map(cam_np)


def compute_guided_gradcam(
    model: nn.Module,
    target_layer: nn.Module,
    input_tensor: torch.Tensor,
    target_class: Optional[int] = None
) -> np.ndarray:
    """
    Computes Guided Grad-CAM attribution map using Captum GuidedGradCam.
    """
    model.eval()
    inp = input_tensor.clone().detach().requires_grad_(True)
    if target_class is None:
        with torch.no_grad():
            target_class = int(model(inp).argmax(dim=1).item())

    changed = _set_relu_inplace(model, inplace=False)
    try:
        guided_gc = GuidedGradCam(model, target_layer)
        attr = guided_gc.attribute(inp, target=target_class)
    finally:
        _restore_relu_inplace(changed)

    ggc_map = torch.max(torch.abs(attr[0]), dim=0)[0].detach().cpu().numpy()
    return normalize_map(ggc_map)


def compute_saliency_map(
    model: nn.Module,
    input_tensor: torch.Tensor,
    target_class: Optional[int] = None
) -> np.ndarray:
    """
    Computes Saliency map using Captum Saliency.
    """
    model.eval()
    inp = input_tensor.clone().detach().requires_grad_(True)
    if target_class is None:
        with torch.no_grad():
            target_class = int(model(inp).argmax(dim=1).item())

    saliency = Saliency(model)
    attr = saliency.attribute(inp, target=target_class, abs=True)
    saliency_map = torch.max(attr[0], dim=0)[0].detach().cpu().numpy()
    return normalize_map(saliency_map)


def compute_integrated_gradients(
    model: nn.Module,
    input_tensor: torch.Tensor,
    target_class: Optional[int] = None,
    steps: int = 30,
    baseline: Optional[torch.Tensor] = None
) -> np.ndarray:
    """
    Computes Integrated Gradients attribution map using Captum IntegratedGradients.
    """
    model.eval()
    if baseline is None:
        baseline = torch.zeros_like(input_tensor)

    inp = input_tensor.clone().detach().requires_grad_(True)
    if target_class is None:
        with torch.no_grad():
            target_class = int(model(inp).argmax(dim=1).item())

    ig = IntegratedGradients(model)
    attr = ig.attribute(inp, baselines=baseline, target=target_class, n_steps=steps)
    ig_map = torch.max(torch.abs(attr[0]), dim=0)[0].detach().cpu().numpy()
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
