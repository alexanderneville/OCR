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

PyObject* test(PyObject* self) {
    printf("working as expected\n");
    Py_INCREF(Py_None);
    return Py_None;
}

PyObject* process(PyObject* self, PyObject* args) {

    char * infile;
    char * outfile;
    if (!PyArg_ParseTuple(args, "ss",  &infile, &outfile)) {
        printf("arguements not parsed correctly\n");
        return NULL;
    }

    int height, width, channels, color_type, bit_depth;
    unsigned char ** pixels = read_image(infile, &height, &width, &channels, &bit_depth, &color_type);
    image_data * image = initialise_data(pixels, height, width, 3);
    image->colour_to_greyscale(image);
    image->expand_greyscale(image);
    pixels = image->export_pixels(image);
    write_image(outfile, pixels, height, width, image->channels, bit_depth, color_type);

    Py_INCREF(Py_None);
    return Py_None;
} 

static PyMethodDef methods[] = {
    {"test", (PyCFunction)test, METH_NOARGS, "test function number 1"},
    {"process", process, METH_VARARGS, "so some image processing"},
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
