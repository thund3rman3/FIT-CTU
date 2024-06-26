import numpy as np


def apply_filter(image: np.array, kernel: np.array) -> np.array:
    # A given image has to have either 2 (grayscale) or 3 (RGB) dimensions
    assert image.ndim in [2, 3]
    # A given filter has to be 2 dimensional and square
    assert kernel.ndim == 2
    assert kernel.shape[0] == kernel.shape[1]

    # TODO
    kernelRows, kernelColls = kernel.shape

    # kernel flip
    pad = kernelRows // 2
    new = np.zeros(image.shape).astype(np.uint8)

    # 2dim, grayscale
    if image.ndim == 2:
        PaddedImage = np.pad(image, pad, 'constant', constant_values=0)
        finalRows, finalColls = PaddedImage.shape
        flippedKernel = np.flip(kernel)
        # sude kernely, moc nefunguje
        if kernelRows % 2 == 0:
            for row in range(pad, finalRows - pad):
                for coll in range(pad, finalColls - pad):
                    imageCut = PaddedImage[row - pad:row + 1, coll - pad:coll + 1]
                    newPixel = np.sum(kernel * imageCut, axis=(0, 1)).clip(0, 255)
                    new[row - pad, coll - pad] = newPixel
        # liche kernely
        else:
            for row in range(pad, finalRows - pad):
                for coll in range(pad, finalColls - pad):
                    imageCut = PaddedImage[row - pad:row - pad + kernelRows, coll - pad:coll - pad + kernelColls]
                    newPixel = flippedKernel * imageCut
                    new[row - pad, coll - pad] = np.sum(newPixel, axis=(0, 1)).clip(0, 255)
    # 3dim obrazky, RGB
    else:
        PaddedImage = np.stack(
            [np.pad(image[:, :, c], pad, mode='constant', constant_values=0) for c in range(3)], axis=2)
        finalRows, finalColls, finalDeep = PaddedImage.shape
        flippedKernel = np.flip(kernel)

        # sude kernely, nevim jestli funguje
        if kernelRows % 2 == 0:
            for deep in range(finalDeep):
                for row in range(pad, finalRows - pad):
                    for coll in range(pad, finalColls - pad):
                        imageCut = PaddedImage[row - pad:row + 1, coll - pad:coll + 1, deep]
                        newPixel = np.sum(kernel * imageCut, axis=(0, 1)).clip(0, 255)
                        new[row - pad, coll - pad, deep] = newPixel
        # liche kernely
        else:
            for deep in range(finalDeep):
                for row in range(pad, finalRows - pad):
                    for coll in range(pad, finalColls - pad):
                        imageCut = PaddedImage[row - pad:row - pad + kernelRows, coll - pad:coll - pad + kernelColls,
                                   deep]
                        newPixel = flippedKernel * imageCut
                        new[row - pad, coll - pad, deep] = np.sum(newPixel, axis=(0, 1)).clip(0, 255)

    return new.astype(np.uint8)
