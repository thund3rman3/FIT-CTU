from typing import List, Optional, Any

import cv2
import numpy as np

import torch
import torch.nn as nn
import torch.nn.functional as F

from utils3 import normalize_map


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
