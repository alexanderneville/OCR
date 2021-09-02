#define PY_SSIZE_T_CLEAN
#include <python3.9/Python.h>
#include "structmember.h"
#include <stddef.h>

#include "../includes/image_io.h"
#include "../includes/image_processing.h"


static PyObject *Pipeline_Error;

typedef struct {
    PyObject_HEAD
    PyObject *infile;
    PyObject *outfile;
    PyObject *dumpfile;
    image_data * image;
    int height, width, channels, color_type, bit_depth;
} PipelineObject;

static PyMemberDef Pipeline_members[] = {

    {"infile",      T_OBJECT_EX,    offsetof(PipelineObject, infile),       0, "the input file"                 },
    {"outfile",     T_OBJECT_EX,    offsetof(PipelineObject, outfile),      0, "the output file"                },
    {"dumpfile",    T_OBJECT_EX,    offsetof(PipelineObject, dumpfile),     0, "where to dump the json data"    },
    {"height",      T_INT,          offsetof(PipelineObject, height),       0, "height of the image in pixels"  },
    {"width",       T_INT,          offsetof(PipelineObject, width),        0, "width of the image in pixels"   },
    {"channels",    T_INT,          offsetof(PipelineObject, channels),     0, "number of channels"             },
    {"color_type",  T_INT,          offsetof(PipelineObject, color_type),   0, "refer to libpng documentation"  },
    {"bit_depth",   T_INT,          offsetof(PipelineObject, bit_depth),    0, "bit depth per channel"          },

    {NULL}  // null terminator
};

static void
Pipeline_dealloc(PipelineObject *self)
{
    Py_XDECREF(self->infile);
    Py_XDECREF(self->outfile);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *
Pipeline_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PipelineObject *self;
    self = (PipelineObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->infile = PyUnicode_FromString("");
        if (self->infile == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->outfile = PyUnicode_FromString("");
        if (self->outfile == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->dumpfile = PyUnicode_FromString("");
        if (self->dumpfile == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->image = NULL;
        self->height = 0;
        self->width = 0;
        self->channels = 0;
        self->color_type = 0;
        self->bit_depth = 0;
    }
    return (PyObject *) self;
}

static int
Pipeline_init(PipelineObject *self, PyObject *args, PyObject *kwds)
{
    static char *kwlist[] = {"infile", "outfile", "dumpfile", NULL};
    PyObject *infile = NULL, *outfile = NULL, *dumpfile = NULL, *tmp;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OOO", kwlist, &infile, &outfile, &dumpfile))
        return -1;

    if (infile) {
        tmp = self->infile;
        Py_INCREF(infile);
        self->infile = infile;
        Py_XDECREF(tmp);
    }
    if (outfile) {
        tmp = self->outfile;
        Py_INCREF(outfile);
        self->outfile = outfile;
        Py_XDECREF(tmp);
    }
    if (dumpfile) {
        tmp = self->dumpfile;
        Py_INCREF(dumpfile);
        self->dumpfile = dumpfile;
        Py_XDECREF(tmp);
    }
    return 0;
}


/*
||=================||
|| object methods  ||
||=================||
*/

static PyObject *
Pipeline_files(PipelineObject *self, PyObject *Py_UNUSED(ignored))
{
    if (self->infile == NULL) {
        PyErr_SetString(Pipeline_Error, "input file missing");
        return NULL;
    }
    if (self->outfile == NULL) {
        PyErr_SetString(PyExc_AttributeError, "output file missing");
        return NULL;
    }
    if (self->dumpfile == NULL) {
        PyErr_SetString(PyExc_AttributeError, "json file missing");
        return NULL;
    }
    return PyUnicode_FromFormat("%S %S %S", self->infile, self->outfile, self->dumpfile);
}

static PyObject *
Pipeline_check_header(PipelineObject *self, PyObject * args)
{
    char * file;
    if(!PyArg_ParseTuple(args, "s", &file)){
        PyErr_SetString(Pipeline_Error, "path arguement required.");
        return NULL;
    }
    int rc = check_header(file);
    if (rc == 0) {
        return PyLong_FromLong(1);
    } else {
        return PyLong_FromLong(0);
    }

}

static PyObject *
Pipeline_load_file(PipelineObject *self, PyObject * args) {

    char * infile;
    if(!PyArg_ParseTuple(args, "s", &infile)){
        PyErr_SetString(Pipeline_Error, "path arguement required.");
        return NULL;
    }
    self->infile = PyUnicode_FromString(infile);

    if(check_header(infile) != 0) {
        PyErr_SetString(Pipeline_Error, "invalid file.");
        return NULL;
    }

    unsigned char ** pixels = read_image(infile, &(self->height), &(self->width), &(self->channels), &(self->bit_depth), &(self->color_type));
    self->image = initialise_data(pixels, self->height, self->width, self->channels);

    return PyLong_FromLong(1);

}

static PyObject *
Pipeline_save_to_file(PipelineObject *self, PyObject * args) {

    char * outfile;
    if(!PyArg_ParseTuple(args, "s", &outfile)){
        PyErr_SetString(Pipeline_Error, "path arguement required.");
        return NULL;
    }
    self->outfile = PyUnicode_FromString(outfile);

    if (self->image == NULL) {
        PyErr_SetString(Pipeline_Error, "Call to 'save_to_file' must follow 'load_file'.");
        return NULL;
    }
    unsigned char ** pixels = self->image->export_pixels(self->image);
    write_image(outfile, pixels, self->image->height, self->image->width, self->channels, self->bit_depth, self->color_type);

    return PyLong_FromLong(1);

}

static PyObject *
Pipeline_scan_image(PipelineObject *self, PyObject *Py_UNUSED(ignored)) {

    if (self->image == NULL) {
        PyErr_SetString(Pipeline_Error, "file must be loaded first.");
        return NULL;
    }

    self->image->create_document_outline(self->image);

    return PyLong_FromLong(1);

}

static PyObject *
Pipeline_generate_dataset(PipelineObject *self, PyObject * args) {

    char * outfile;
    if(!PyArg_ParseTuple(args, "s", &outfile)){
        PyErr_SetString(Pipeline_Error, "path arguement required.");
        return NULL;
    }

    self->dumpfile = PyUnicode_FromString(outfile);

    if (self->image == NULL) {
        PyErr_SetString(Pipeline_Error, "file must be loaded first.");
        return NULL;
    }

    self->image->generate_dataset_from_image(self->image, outfile);

    return PyLong_FromLong(1);

}

static PyObject *
Pipeline_convolution(PipelineObject *self, PyObject * args) {

    int type, dimensions;
    if(!PyArg_ParseTuple(args, "i i", &type, &dimensions)){
        PyErr_SetString(Pipeline_Error, "path arguement required.");
        return NULL;
    }

    if (self->image == NULL) {
        PyErr_SetString(Pipeline_Error, "file must be loaded first.");
        return NULL;
    }

    switch(type){
        case 1:
            self->image->process(self->image, Mean, dimensions);
            break;
        case 2:
            self->image->process(self->image, Gaussian, dimensions);
            break;
        case3:
            self->image->process(self->image, Gaussian, dimensions);
            break;
    }

    return PyLong_FromLong(1);

}

static PyObject *
Pipeline_resize(PipelineObject *self, PyObject * args) {

    float scale_factor;
    if(!PyArg_ParseTuple(args, "f", &scale_factor)){
        PyErr_SetString(Pipeline_Error, "path arguement required.");
        return NULL;
    }

    if (self->image == NULL) {
        PyErr_SetString(Pipeline_Error, "file must be loaded first.");
        return NULL;
    }

    self->image->resize(self->image, scale_factor);

    self->height = self->image->height;
    self->width = self->image->width;
    return PyLong_FromLong(1);

}

static PyObject *
Pipeline_translate(PipelineObject *self, PyObject * args) {

    return PyLong_FromLong(1);

}

static PyObject *
Pipeline_invert_colours(PipelineObject *self, PyObject *Py_UNUSED(ignored)) {

    if (self->image == NULL) {
        PyErr_SetString(Pipeline_Error, "file must be loaded first.");
        return NULL;
    }

    self->image->invert(self->image);

    return PyLong_FromLong(1);

}

static PyObject *
Pipeline_switch_channel_num(PipelineObject *self, PyObject *Py_UNUSED(ignored)) {

    if (self->image == NULL) {
        PyErr_SetString(Pipeline_Error, "file must be loaded first.");
        return NULL;
    }

    if (self->image->channels == 3) {
        self->image->rgb_to_greyscale(self->image);
    } else {
        self->image->greyscale_to_rgb(self->image);
    }

}

static PyMethodDef Pipeline_methods[] = {

    {"print_files", (PyCFunction) Pipeline_files, METH_NOARGS, 
     "return the names of the files that the object will use / is using."},
    {"check_header", (PyCFunction) Pipeline_check_header, METH_VARARGS, 
     "return the names of the files that the object will use / is using."},

    {"load_file", (PyCFunction) Pipeline_load_file, METH_VARARGS, 
     "load the contents of the specified file into the object."},
    {"save_to_file", (PyCFunction) Pipeline_save_to_file, METH_VARARGS, 
     "save the contents of the current instance to the specified file."},

    {"scan_image", (PyCFunction) Pipeline_scan_image, METH_NOARGS, 
     "create an internal document structure, identifying where character reside in the input image."},
    {"generate_dataset", (PyCFunction) Pipeline_generate_dataset, METH_VARARGS, 
     "generate a json dataset from the input image. Takes the file to save to."},

    {"convolution", (PyCFunction) Pipeline_convolution, METH_VARARGS,
     "perform a convolution on the loaded file."},
    {"resize", (PyCFunction) Pipeline_resize, METH_VARARGS, 
     "resize the input image."},
    {"translate", (PyCFunction) Pipeline_translate, METH_VARARGS, 
     "'move' the input image contents, wrapping on the corners."},

    {"switch_channel_num", (PyCFunction) Pipeline_switch_channel_num, METH_NOARGS, 
     "greyscale/rgb inversion"},
    {"invert_colours", (PyCFunction) Pipeline_invert_colours, METH_NOARGS, 
     "greyscale/rgb inversion"},

    {0, 0},        /* Null terminator */
};

static PyTypeObject PipelineType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "pipeline.Pipeline",
    .tp_doc = "object for interfacing with the image pipeline",
    .tp_basicsize = sizeof(PipelineObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = Pipeline_new,
    .tp_init = (initproc) Pipeline_init,
    .tp_dealloc = (destructor) Pipeline_dealloc,
    .tp_members = Pipeline_members,
    .tp_methods = Pipeline_methods,
};

static PyModuleDef custommodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "pipeline",
    .m_doc = "extension type for interfacing with the image processing pipeline.",
    .m_size = -1,
};

PyMODINIT_FUNC
PyInit_pipeline(void)
{
    PyObject *m;
    if (PyType_Ready(&PipelineType) < 0)
        return NULL;

    m = PyModule_Create(&custommodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&PipelineType);
    if (PyModule_AddObject(m, "Pipeline", (PyObject *) &PipelineType) < 0) {
        Py_DECREF(&PipelineType);
        Py_DECREF(m);
        return NULL;
    }

    Pipeline_Error = PyErr_NewException("pipeline.error", NULL, NULL);
    Py_XINCREF(Pipeline_Error);
    if (PyModule_AddObject(m, "PipelineErrror", Pipeline_Error) < 0) {
        Py_XDECREF(Pipeline_Error);
        Py_CLEAR(Pipeline_Error);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
