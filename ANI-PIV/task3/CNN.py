from typing import Literal
from typing import Any, cast

import torch
import torch.nn as nn
import torchvision.models as models


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
        in_channels: int = 3,
        num_classes: int = 2,
        base_channels: int = 32,
        num_blocks: int = 4,
        kernel_size: int = 3,
        pool_type: Literal["max", "avg", "adaptive"] = "max",
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

        self.extract_features = nn.Sequential(*blocks)

        self.classifier = nn.Sequential(  # MLP
            nn.Flatten(),  # flattens the tensor so the linear layers can work
            nn.Dropout(dropout),  # randomly turns off some neurons
            nn.Linear(curr_out, hidden_dim),  # 256 -> 128
            nn.ReLU(inplace=False),
            nn.Dropout(dropout),
            nn.Linear(hidden_dim, num_classes),  # 128 -> 2
        )

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        features = self.extract_features(x)
        return self.classifier(features)


def get_pretrained_resnet(
    num_classes: int = 2,
    fine_tune: bool = True
) -> models.ResNet:
    """
    Loads ResNet18 with ImageNet weights, adapting final fully connected layer.
    """
    model = models.resnet18(weights=models.ResNet18_Weights.IMAGENET1K_V1)
    if not fine_tune:
        for param in model.parameters():
            param.requires_grad = False

    # Extract input (512) feature dimension safely for static type checkers
    in_features: int = getattr(model.fc, "in_features", 512)

    cast(Any, model).fc = nn.Sequential(
        nn.Dropout(0.2),
        nn.Linear(in_features, num_classes)  # 512 -> 2
    )
    return model
