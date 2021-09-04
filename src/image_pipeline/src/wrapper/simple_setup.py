#!/usr/bin/env python3

from distutils.core import setup, Extension

# module1 = Extension("module", sources = ["wrapper.c", "image_io.c", "image_processing.c"], extra_compile_args=["-lpng"])

module1 = Extension("open_close", sources = ["wrapper.c", "image_io.c"], include_dirs=['/usr/include'], libraries = ['png'], library_dirs=['/usr/lib'])

setup(name = "open_close", version = "1", description = "package containing \"module\"", ext_modules = [module1])
