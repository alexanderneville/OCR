from distutils.core import setup, Extension

module1 = Extension(
        "pipeline", 
        sources = ["pipeline.c", "image_processing.c", 
                    "image_io.c", "matrix.c", "dataset.c",
                    "convolution.c", "character_detection.c"],
        include_dirs=['/usr/include'], 
        libraries = ['png'], 
        library_dirs=['/usr/lib'])

setup(name = "pipeline", 
        version = "1", 
        description = "package containing the image pipeline interface for python", 
        ext_modules = [module1])
