from pathlib import Path
import cv2 as cv
import improutils as iu
import matplotlib.pyplot as plt

# Data is one level above this script folder: ANI-PIV/Data
image_dir = Path(__file__).resolve().parent.parent / "Data"

if not image_dir.exists():
    raise NotADirectoryError(f"Folder not found: {image_dir}")

try:
    image_files = [p for p in image_dir.iterdir() if p.is_file()]
    image_files = [image_files[0]]

except Exception as e:
    raise RuntimeError(f"Error accessing files in folder: {image_dir}") from e


images = {}
for image_path in image_files:
    img = iu.load_image(image_path)
    if img is None:
        print(f"Skipping unreadable file: {image_path}")
        continue
    images[str(image_path)] = img


# iu.plot_images(*images.values())
# plt.show()
iu.show_images(*images.values())
