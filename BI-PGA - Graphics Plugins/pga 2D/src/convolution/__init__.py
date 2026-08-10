from .app import App

# And add the extension to Krita's list of extensions:
app = Krita.instance()
# Instantiate class:
extension = App(parent = app)
app.addExtension(extension)
