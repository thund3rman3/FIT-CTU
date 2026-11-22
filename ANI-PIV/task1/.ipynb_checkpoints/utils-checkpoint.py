from pathlib import Path
import numpy as np
import cv2 as cv
import improutils as iu
import matplotlib.pyplot as plt

def read_directory_gray(image_dir):
    if not image_dir.exists():
        raise NotADirectoryError(f"Folder not found: {image_dir}")
    
    try:
        image_files = [p for p in image_dir.iterdir() if p.is_file()]
    
    except Exception as e:
        raise RuntimeError(f"Error accessing files in folder: {image_dir}") from e
    
    images = []
    for image_path in image_files:
        img = iu.load_image(image_path)
        if img is None:
            print(f"Skipping unreadable file: {image_path}")
            continue
        img = iu.to_gray(img)
        images.append(img)
    return images