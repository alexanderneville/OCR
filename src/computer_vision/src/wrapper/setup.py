#!/usr/bin/env python3

from distutils.core import setup, Extension
my_module = Extension("module", sources = ["wrapper.c", "image_io.c", "image_processing.c"], extra_compile_args=["-lpng"])
setup(name = "my_package", version = "1", description = "package containing \"module\"", ext_modules = [my_module])
