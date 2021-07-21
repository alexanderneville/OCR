#include <stdio.h>
#include <python3.9/Python.h>
#include "functions.h"

PyObject* func_1(PyObject* self) {
    printf("hello world");
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* func_2(PyObject* self, PyObject* args) {
    int input;
    if (!PyArg_ParseTuple(args, "i",  &input)) {
        return NULL;
    }
    int output = input * 2;
    return Py_BuildValue("i", output);
} 
