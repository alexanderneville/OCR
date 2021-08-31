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

#include "../includes/image_io.h"
#include "../includes/image_processing.h"

PyObject* _test(PyObject* self) {
    printf("working as expected\n");
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* _process(PyObject* self, PyObject* args) {

    char * infile;
    char * outfile;

    if (!PyArg_ParseTuple(args, "ss",  &infile, &outfile)) {
        printf("arguements not parsed correctly\n");
        return NULL;
    }

    int height, width, channels, bit_depth, color_type;
    unsigned char ** pixels = read_image(infile, &height, &width, &channels, &bit_depth, &color_type);
    /* image_data * image_p = initialise_data(pixels, height, width, 3); */
    /* (* image_p).colour_to_greyscale(image_p); */
    /* (* image_p).soften(image_p); */
    /* (* image_p).reduce_resolution(image_p); */
    /* (* image_p).sharpen(image_p); */
    /* (* image_p).expand_greyscale(image_p); */
    /* pixels = (* image_p).export_pixels(image_p); */
    write_image(outfile, pixels, height, width, channels, bit_depth, color_type);

    Py_INCREF(Py_None);
    return Py_None;
} 

static PyMethodDef methods[] = {
    {"test", (PyCFunction)_test, METH_NOARGS, "test function number 1"},
    {"process", _process, METH_VARARGS, "so some image processing"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef open_close = {
    PyModuleDef_HEAD_INIT,
    "open_close", // module name
    "first attempt at c extension", // docs
    -1,
    methods // module def
};

PyMODINIT_FUNC PyInit_open_close(void) {
    return PyModule_Create(&open_close);
}
