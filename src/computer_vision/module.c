#include <stdio.h>
#include <python3.9/Python.h>
#include "functions.h"

static PyMethodDef methods[] = {
    {"hello", (PyCFunction)func_1, METH_NOARGS, "test function number 1"},
    {"calc", func_2, METH_NOARGS, "test function number 2"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef my_module = {
    PyModuleDef_HEAD_INIT,
    "my_module", // module name
    "first attempt at c extension", // docs
    -1,
    methods // module def
};

PyMODINIT_FUNC PyInit_module(void) {
    return PyModule_Create(&my_module);
}
