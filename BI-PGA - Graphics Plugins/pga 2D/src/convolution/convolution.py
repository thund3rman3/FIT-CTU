from PyQt5.QtGui import QImage
import numpy as np

def QImage_to_numpy(QImage):
    """
    Converts a QImage to a numpy array.

    Args:
        QImage (QImage): The input QImage object to be converted.

    Returns:
        numpy.ndarray: A numpy array representation of the QImage with shape 
                       (height, width, 4), where the last dimension represents 
                       the RGBA channels.
    """
    ptr = QImage.bits()
    ptr.setsize(QImage.byteCount())
    arr = np.array(ptr).reshape(QImage.height(), QImage.width(), 4)
    return arr

def numpy_to_QImage(np_image):
    """
    Converts a numpy array to a QImage.

    Args:
        np_image (numpy.ndarray): The image to convert.

    Returns:
        QImage: The converted image in RGBA8888 format.
    """
    return QImage(np_image.tobytes(), np_image.shape[1], np_image.shape[0], 
                  np_image.shape[1] * 4, QImage.Format_RGBA8888)

class Convolution:
    def __init__(self, grayscale_np_image, np_image, height, width):
        """
        Initializes the Convolution class with the given images and dimensions.

        Args:
            grayscale_np_image (numpy.ndarray): grayscale numpy image.
            np_image (numpy.ndarray): Numpy image.
            height (int): Height of the image.
            width (int): Width of the image.
        """
        self.kernels = {
            "Gaussian blur": np.array([[1, 2, 1], [2, 4, 2], [1, 2, 1]]) / 16,
            "Box blur": np.ones((3, 3)) / 9, 
            "Emboss": np.array([[-2, -1, 0], [-1, 1, 1], [0, 1, 2]]),
            "Sharpen": np.array([[0, -1, 0], [-1, 5, -1], [0, -1, 0]]),
            "Laplacian Edge": np.array([[0, 1, 0], [1, -4, 1], [0, 1, 0]]),
            "Edge 1": np.array([[1, 0, -1], [0, 0, 0], [-1, 0, 1]]),
            "Edge 2": np.array([[-1, -1, -1], [-1, 8, -1], [-1, -1, -1]]),
            "Roberts cross Edge": [np.array([[1, 0], [0, -1]]), np.array([[0, 1], [-1, 0]])],
            "Sobel Edge": [np.array([[1, 0, -1], [2, 0, -2], [1, 0, -1]]), np.array([[1, 2, 1], [0, 0, 0], [-1, -2, -1]])],
            "Prewitt Edge": [np.array([[1, 0, -1], [1, 0, -1], [1, 0, -1]]), np.array([[1, 1, 1], [0, 0, 0], [-1, -1, -1]])]
        }
        self.grayscale_np_image = grayscale_np_image
        self.np_image = np_image
        self.height = height
        self.width = width
        self.RGBA8888_channels = 4

    def decide_convolution(self, text):
        """
        Decides which convolution kernel to use based on the input text.

        Args:
            text (str): The name of the convolution kernel.

        Returns:
            QImage: The convolved image.
        """
        kernel = self.kernels[text]
        gray = "Edge" in text
        if gray:
            if text in ["Roberts cross Edge", "Sobel Edge", "Prewitt Edge"]:
                gradient_x = self.convolution(kernel[0], gray, self.grayscale_np_image, True)
                gradient_y = self.convolution(kernel[1], gray, self.grayscale_np_image, True)
                gradient_magnitude = np.sqrt(gradient_x.astype(np.float32)**2 + gradient_y.astype(np.float32)**2)
                gradient_magnitude = np.clip(gradient_magnitude, 0, 255).astype(np.uint8)
                gradient_magnitude[:, :, 3] = 255
                convolved_image = numpy_to_QImage(gradient_magnitude)
            else:
                convolved_image = self.convolution(kernel, gray, self.grayscale_np_image)
        else:
            convolved_image = self.convolution(kernel, gray, self.np_image)
        return convolved_image

    def enlarge_image(self, image, kernel_half_size):
        """
        Enlarges the image by padding the borders.

        Args:
            image (numpy.ndarray): The input image.
            kernel_half_size (int): Half the size of the kernel.

        Returns:
            numpy.ndarray: The enlarged image.
        """
        pixel_cnt = (kernel_half_size, kernel_half_size)
        res = np.pad(image, (pixel_cnt, pixel_cnt, (0, 0)), mode="edge")
        return res

    def convolution(self, kernel, gray, image, hv=False):
        """
        Applies the convolution operation on the image using the given kernel.

        Args:
            kernel (numpy.ndarray): The convolution kernel.
            gray (bool): Whether to convert the image to grayscale.
            image (numpy.ndarray): The input image.
            hv (bool): Whether to perform horizontal and vertical edge detection.

        Returns:
            numpy.ndarray or QImage: The convolved image.
        """
        enlarged_image = self.enlarge_image(image, kernel.shape[0] // 2)
        convolved_image = np.copy(image)
        for i in range(self.RGBA8888_channels):
            if gray and i == 3:
                break
            for y in range(self.height):
                for x in range(self.width):
                    sum = np.sum(kernel * enlarged_image[y:y + kernel.shape[0], x:x + kernel.shape[1], i])
                    convolved_image[y, x, i] = np.clip(sum, 0, 255)
        if not hv:
            convolved_image = numpy_to_QImage(convolved_image)

        return convolved_image
