#!/usr/bin/env python3

from distutils.core import setup, Extension

test_extension = Extension("test_module", sources = ["test_module.c"], include_dirs=[], libraries = [], library_dirs=[])

setup(name = "test_package", version = "1", description = "package containing \"test_module\"", ext_modules = [test_extension])
