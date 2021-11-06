#include <python3.9/Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>
#include <png.h>

PyObject* _test_1(PyObject* self) {
    printf("Hello from extension model!\n");
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* _test_2(PyObject* self, PyObject* args) {
    int a;
    int b;
    if (!PyArg_ParseTuple(args, "ii",  &a, &b)) {
        printf("arguements not parsed correctly\n");
        return NULL;
    }
    // printf("sum: %d\n", a+b);
    return PyLong_FromLong(a+b);
} 

static PyMethodDef methods[] = {
    {"test_1", (PyCFunction)_test_1, METH_NOARGS, "Print a message"},
    {"test_2", _test_2, METH_VARARGS, "Add 2 integers"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef test_module = {
    PyModuleDef_HEAD_INIT,
    "test_module",
    "c extension demonstration",
    -1,
    methods
};

PyMODINIT_FUNC PyInit_test_module(void) {
    return PyModule_Create(&test_module);
}
