from .preview import Preview

from krita import Extension
from PyQt5.QtWidgets import *
from PyQt5.QtCore import Qt, QByteArray
from PyQt5.QtGui import *

EXTENSION_ID = "pykrita_convolution"
MENU_ENTRY = "Convolution"

class App(Extension):

    def __init__(self, parent):
        super().__init__(parent)

    def setup(self):
        pass

    def createActions(self, window):
        action = window.createAction(EXTENSION_ID, MENU_ENTRY, "tools")
        action.triggered.connect(self.convolution)

    def convolution(self):
        kr = Krita.instance()
        doc = kr.activeDocument()

        if not doc:
            QMessageBox.warning(None, "Convolution", "No active document found!")
            return
        
        preview = Preview(doc)
        preview.exec() 


