from krita import Extension
from PyQt5.QtWidgets import QDialog, QVBoxLayout, QRadioButton, QLabel, QComboBox, QLineEdit, QPushButton, QMessageBox, QHBoxLayout, QWidget, QApplication
from PyQt5.QtCore import Qt, QByteArray
from PyQt5.QtGui import QPixmap
import numpy as np
from .convolution import Convolution, numpy_to_QImage, QImage_to_numpy

class Preview(QDialog):
    def __init__(self, doc):
        super().__init__()

        self.setWindowTitle("Convolution")
        self.setWindowFlags(Qt.Window)
        self.layout = QVBoxLayout()
        self.doc = doc

        self.layer = self.doc.activeNode()
        self.width = self.doc.width()
        self.height = self.doc.height()
        self.image = self.layer.pixelData(0, 0, self.width, self.height)
        self.np_image = np.frombuffer(self.image, dtype=np.uint8).reshape((self.width, self.height, 4))
        self.np_image = self.np_image[..., [2, 1, 0, 3]]  # Swap R and B channels
        self.np_image[:, :, 3] = 255
        self.image = numpy_to_QImage(self.np_image)

        grayscale_img = np.average(self.np_image[:, :, :3].astype(float), weights=[0.299, 0.587, 0.114], axis=2).astype(np.uint8)
        #bin_img = np.where(grayscale_img < 128, 0, 255)
        self.grayscale_np_image = np.zeros_like(self.np_image)
        self.grayscale_np_image[:, :, :3] = np.stack([grayscale_img] * 3, axis=-1)
        self.grayscale_np_image[:, :, 3] = 255

        self.conv = Convolution(self.grayscale_np_image, self.np_image, self.height, self.width)
        self.preview = self.image

        radio_layout = QVBoxLayout()

        self.radio_predefined = QRadioButton("User pre-defined kernel")
        self.radio_predefined.setChecked(True)

        self.predefined_layout = QVBoxLayout()

        self.preview_label = QLabel(self)
        self.predefined_layout.addWidget(self.preview_label)

        # Kernel dropdown
        self.kernel_layout = QHBoxLayout()
        self.kernel_layout.addWidget(QLabel("Kernel:"))
        self.kernel_select = QComboBox()
        self.kernel_select.addItems(["Gaussian blur", "Box blur", "Laplacian Edge", "Edge 1", "Edge 2",
                                     "Roberts cross Edge", "Sobel Edge", "Prewitt Edge", "Emboss", "Sharpen"])
        self.kernel_select.currentIndexChanged.connect(self.update_preview)
        self.kernel_layout.addWidget(self.kernel_select)
        kernel_widget = QWidget()
        kernel_widget.setLayout(self.kernel_layout)
        self.predefined_layout.addWidget(kernel_widget)

        # Status label
        self.status_layout = QHBoxLayout()
        self.status_layout.addWidget(QLabel("Status:"))
        self.status_label = QLabel("nothing")
        self.status_layout.addWidget(self.status_label)
        status_widget = QWidget()
        status_widget.setLayout(self.status_layout)
        self.predefined_layout.addWidget(status_widget)

        self.radio_custom = QRadioButton("Custom kernel")
        self.radio_custom.setChecked(False)

        self.user_defined_layout = QVBoxLayout()

        # TextBox for custom kernel input
        self.user_defined_layout.addWidget(QLabel("Enter custom kernel in format of numpy array'[[x,y,z,...],[a,b,c,...],...]':"))
        self.custom_kernel_input = QLineEdit()
        self.user_defined_layout.addWidget(self.custom_kernel_input)

        radio_layout.addWidget(self.radio_predefined)
        self.predefined_widget = QWidget()
        self.predefined_widget.setLayout(self.predefined_layout)
        radio_layout.addWidget(self.predefined_widget)
        radio_layout.addWidget(self.radio_custom)
        self.user_defined_widget = QWidget()
        self.user_defined_widget.setLayout(self.user_defined_layout)
        self.user_defined_widget.setVisible(False)
        radio_layout.addWidget(self.user_defined_widget)
        self.layout.addLayout(radio_layout)

        # Connect radio buttons to toggle UI elements
        self.radio_predefined.toggled.connect(self.toggle_layout_visibility)
        self.radio_custom.toggled.connect(self.toggle_layout_visibility)

        # Buttons yes/no
        button_layout = QHBoxLayout()
        self.apply_button = QPushButton("Apply")
        self.cancel_button = QPushButton("Cancel")
        button_layout.addWidget(self.apply_button)
        button_layout.addWidget(self.cancel_button)
        self.layout.addLayout(button_layout)

        # Buttons yes/no connections
        self.apply_button.clicked.connect(self.apply_convolution)
        self.cancel_button.clicked.connect(self.reject)

        self.setLayout(self.layout)
        self.update_preview()

    def toggle_layout_visibility(self):
        """Toggle visibility of layouts based on the selected radio button."""
        self.predefined_widget.setVisible(self.radio_predefined.isChecked())
        self.user_defined_widget.setVisible(self.radio_custom.isChecked())

    def update_preview(self):
        self.status_label.setText("working")
        QApplication.processEvents()
        self.preview = self.conv.decide_convolution(self.kernel_select.currentText())
        self.preview_label.setPixmap(QPixmap.fromImage(self.preview.scaled(self.width, self.height, Qt.KeepAspectRatio)))
        self.status_label.setText("done")

    def get_input(self):
        """Get the user input for the custom kernel."""
        
        kernel = eval(self.custom_kernel_input.text())

        if not isinstance(kernel, list) or not all(isinstance(row, list) for row in kernel) or not kernel or all(not sublist for sublist in kernel):
            raise ValueError("Kernel must be a 2D list.")

        if len(set(len(row) for row in kernel)) > 1:
            raise ValueError("All rows in the kernel must have the same length.")
        kernel = np.array(kernel)
        return kernel


    def apply_convolution(self):
        gray = False

        if self.radio_custom.isChecked():
            try:
                kernel = self.get_input()
            except (ValueError, SyntaxError)  as e:
                QMessageBox.warning(None, "Convolution", f"Kernel invalid: {str(e)}")
                self.reject()
                return

            convolved_image = self.conv.convolution(kernel, gray, self.np_image)
        elif self.radio_predefined.isChecked():
            convolved_image = self.preview

        buffer = convolved_image.bits()
        buffer.setsize(convolved_image.byteCount())
        convolved_np_image = QImage_to_numpy(convolved_image)

        # Swap R and B channels back
        convolved_np_image = convolved_np_image[..., [2, 1, 0, 3]]

        image_data = QByteArray(convolved_np_image.tobytes())
        self.layer.setPixelData(image_data, 0, 0, self.width, self.height)
        self.doc.refreshProjection()
        self.accept()
