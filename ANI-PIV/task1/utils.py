from matplotlib.colors import Normalize, NoNorm
import cv2
from pathlib import Path
import numpy as np
import cv2 as cv
import improutils as iu
import matplotlib.pyplot as plt
import math
from collections import deque


def read_directory(image_dir):
    if not image_dir.exists():
        raise NotADirectoryError(f"Folder not found: {image_dir}")

    try:
        image_files = [p for p in image_dir.iterdir() if p.is_file()]

    except Exception as e:
        raise RuntimeError(
            f"Error accessing files in folder: {image_dir}") from e

    images_gray = []
    images_rgb = []
    for image_path in image_files:
        img = iu.load_image(image_path)
        if img is None:
            print(f"Skipping unreadable file: {image_path}")
            continue
        images_rgb.append(img)
        img = iu.to_gray(img)
        images_gray.append(img)
    return images_gray, images_rgb


def pwr_image(image, gamma):
    if gamma <= 0:
        raise ValueError("Gamma must be greater than 0")
    return (cv.pow(image.astype(np.float32) / 255.0, gamma) * 255).astype(np.uint8)


def morph_open(src: np.ndarray, ker_erode_SE: bool, ker_size_erode: int, iter_erode: int, ker_dilate_SE: bool, ker_size_dilate: int, iter_dilate: int) -> np.ndarray:
    if ker_erode_SE is False:
        ker_erode = np.ones((ker_size_erode, ker_size_erode), np.uint8)
    else:
        ker_erode = cv.getStructuringElement(
            cv.MORPH_ELLIPSE, (ker_size_erode, ker_size_erode))

    if ker_dilate_SE is False:
        ker_dilate = np.ones((ker_size_dilate, ker_size_dilate), np.uint8)
    else:
        ker_dilate = cv.getStructuringElement(
            cv.MORPH_ELLIPSE, (ker_size_dilate, ker_size_dilate))

    eroded = cv.erode(
        src, kernel=ker_erode, iterations=iter_erode)

    return cv.dilate(
        eroded, kernel=ker_dilate, iterations=iter_dilate)


def otsu_threshold(image: np.ndarray) -> np.ndarray:
    if image.ndim != 2:
        raise ValueError("Input image must be a grayscale image")
    _, thresholded = cv.threshold(
        image, 0, 255, cv.THRESH_BINARY + cv.THRESH_OTSU)
    return thresholded


def negate(image: np.ndarray) -> np.ndarray:
    if image.ndim != 2:
        raise ValueError("Input image must be a grayscale image")
    return cv.bitwise_not(image)


def circularity(c):
    area = cv.contourArea(c)
    perimeter = cv.arcLength(c, True)
    if perimeter == 0:
        return 0
    return 4 * np.pi * area / (perimeter * perimeter)


def height_width_ratio(c):
    x, y, w, h = cv.boundingRect(c)
    if w == 0:
        return 0
    return h / w


def find_contours(image: np.ndarray):
    contours, _ = cv.findContours(
        image, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    return contours


def draw_contours(rgb_image: np.ndarray, contours: list, fill: bool = False) -> np.ndarray:
    thick = cv.FILLED if fill else 5
    return cv.drawContours(
        np.zeros(rgb_image.shape, dtype=np.uint8),
        contours,
        -1,  # all contours
        color=(0, 0, 255),  # red color in BGR
        thickness=thick,
    )


class Pixel:
    def __init__(self, row, col):
        self.row = row
        self.col = col


def image_reconstruct(marker: np.ndarray, mask: np.ndarray) -> np.ndarray:
    """
    Perform morphological reconstruction by dilation.

    Parameters:
        marker (np.ndarray): The marker image (seed).
        mask (np.ndarray): The mask image (constraint).

    Returns:
        np.ndarray: Reconstructed image.
    """
    if marker.shape != mask.shape:
        raise ValueError("Marker and mask must have the same dimensions")

    # Ensure images are in the same type
    marker = marker.astype(mask.dtype)

    # Create a working copy
    reconstructed = marker.copy()

    # Use a queue for pixels to update
    q = deque()
    rows, cols = reconstructed.shape

    # Initialize queue with pixels where marker < mask
    for r in range(rows):
        for c in range(cols):
            if reconstructed[r, c] < mask[r, c]:
                q.append(Pixel(r, c))

    # Neighbor offsets (4-connectivity)
    neighbors = [(-1, 0), (1, 0), (0, -1), (0, 1)]

    while q:
        p = q.popleft()
        for dr, dc in neighbors:
            rr, cc = p.row + dr, p.col + dc
            if 0 <= rr < rows and 0 <= cc < cols:
                new_val = min(mask[rr, cc], max(
                    reconstructed[rr, cc], reconstructed[p.row, p.col]))
                if new_val > reconstructed[rr, cc]:
                    reconstructed[rr, cc] = new_val
                    q.append(Pixel(rr, cc))

    return reconstructed


def reconstruct(image, kernel_size, iterations):
    inverted_image = negate(image)
    kernel = np.ones((kernel_size, kernel_size), np.uint8)
    eroded_img = cv.erode(inverted_image, kernel=kernel, iterations=iterations)
    recons = image_reconstruct(eroded_img, inverted_image)
    mask = otsu_threshold(inverted_image - recons)
    return recons, mask


def plot_images(
    *imgs, titles=[], channels="bgr", normalize=False, ticks_off=True, title_size=32
):
    assert channels.lower() in ["bgr", "rgb", "mono"], (
        "Possible values for channels are: bgr, rgb or mono!"
    )

    width_def = 60
    height_def = 60

    width = math.ceil(math.sqrt(len(imgs)))
    height = math.ceil(len(imgs) / width)

    height_def = height_def / 5 * width
    if height_def > 65:
        height_def = 65

    f = plt.figure(figsize=(width_def, height_def))

    for i, img in enumerate(imgs, 1):
        ax = f.add_subplot(height, width, i)
        if ticks_off:
            ax.axis("off")

        if len(titles) != 0:
            if len(imgs) != len(titles):
                print("WARNING titles length is not the same as images length!")

            try:
                ax.set_title(
                    str(titles[i - 1]),
                    fontdict={"fontsize": title_size, "fontweight": 400},
                )
            except IndexError:
                pass

        if channels.lower() == "mono" or img.ndim == 2:
            if normalize:
                norm = Normalize()
            else:
                norm = NoNorm()
            ax.imshow(img, cmap=plt.get_cmap("gray"), norm=norm)
        elif channels.lower() == "rgb":
            ax.imshow(img)
        else:
            ax.imshow(cv2.cvtColor(img, cv2.COLOR_BGR2RGB))

    f.subplots_adjust(left=0, right=1, bottom=0, top=1, wspace=0, hspace=0)

    pad = 0.5 if len(titles) > 0 else 0.0
    f.tight_layout(pad=pad)
