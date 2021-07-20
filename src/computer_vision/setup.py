#!/usr/bin/env python3

from distutils.core import setup, Extension

my_module = Extension("module", sources = ["module.c", "functions.c"])
setup(name = "my_package", version = "1", description = "package containing \"module\"", ext_modules = [my_module])
