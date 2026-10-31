import torch
from torch import nn
from torch.utils.data import DataLoader
from torchvision import datasets
from torchvision.transforms import v2
from torchvision.io import read_image, ImageReadMode
from torchvision import tv_tensors

import cv2
import numpy as np
import random


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
    def __init__(self, in_channels, out_channels):
        super().__init__()
        self.conv = nn.Sequential(
            nn.Conv2d(in_channels, out_channels, 3, padding=1),
            nn.BatchNorm2d(out_channels),
            nn.ReLU(inplace=True),
            nn.Conv2d(out_channels, out_channels, 3, padding=1),
            nn.BatchNorm2d(out_channels),
            nn.ReLU(inplace=True)
        )

    def forward(self, x):
        return self.conv(x)


class UNet(nn.Module):
    def __init__(self):
        super().__init__()
        self.enc1 = DoubleConv(1, 32)
        self.enc2 = DoubleConv(32, 64)
        self.enc3 = DoubleConv(64, 128)
        self.pool = nn.MaxPool2d(2)
        self.up = nn.Upsample(
            scale_factor=2, mode='bilinear', align_corners=True)
        self.dec2 = DoubleConv(192, 64)  # 128 + 64
        self.dec1 = DoubleConv(96, 32)  # 64 + 32
        self.out = nn.Conv2d(32, 1, 1)

    def forward(self, x):
        # Encoder
        e1 = self.enc1(x)
        e2 = self.enc2(self.pool(e1))
        e3 = self.enc3(self.pool(e2))
        # Decoder, skip connections
        d2 = self.dec2(torch.cat([self.up(e3), e2], dim=1))
        d1 = self.dec1(torch.cat([self.up(d2), e1], dim=1))
        return self.out(d1)


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
