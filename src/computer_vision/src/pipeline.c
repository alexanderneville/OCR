#define PY_SSIZE_T_CLEAN
#include <python3.9/Python.h>
#include "structmember.h"
#include "../includes/image_io.h"
#include "../includes/image_processing.h"

#include "../includes/image_processing.h"

static PyObject *PipeLineError;

typedef struct {
    PyObject_HEAD
    PyObject *infile;
    PyObject *outfile;
    PyObject *dumpfile;
    image_data * image;
    int height, width, channels, color_type, bit_depth;
} PipelineObject;

static void
Custom_dealloc(PipelineObject *self)
{
    Py_XDECREF(self->infile);
    Py_XDECREF(self->outfile);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *
Custom_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
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
/* static int */
/* Pipeline_check_header(PipelineObject *self, PyObject *args, PyObject *kwds) */
/* { */
/*     return 0; */
/* } */

static PyMemberDef Pipeline_members[] = {
    {"infile", T_OBJECT_EX, offsetof(PipelineObject, infile), 0,
     "the input file"},
    {"outfile", T_OBJECT_EX, offsetof(PipelineObject, outfile), 0,
     "the output file"},
    {"dumpfile", T_OBJECT_EX, offsetof(PipelineObject, dumpfile), 0,
     "where to dump the json data"},
    /* {"height", T_INT, offsetof(PipelineObject, height), */
    /*  "height of the image in pixels"}, */
    /* {"width", T_INT, offsetof(PipelineObject, width), */
    /*  "width of the image in pixels"}, */
    /* {"channels", T_INT, offsetof(PipelineObject, channels), */
    /*  "number of channels"}, */
    /* {"color_type", T_INT, offsetof(PipelineObject, color_type), */
    /*  "refer to libpng documentation"}, */
    /* {"bit_depth", T_INT, offsetof(PipelineObject, bit_depth), */
    /*  "bit depth per channel"}, */
    {NULL}  /* Sentinel */
};

// methods
static PyObject *
Pipeline_files(PipelineObject *self, PyObject *Py_UNUSED(ignored))
{
    if (self->infile == NULL) {
        PyErr_SetString(PipeLineError, "input file missing");
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


static PyMethodDef Pipeline_methods[] = {
    {"files", (PyCFunction) Pipeline_files, METH_NOARGS,
     "return the names of the files that the object will use / is using."},
    /* {"test", (PyCFunction) Pipeline_test_exception, METH_NOARGS, */
    /*  "test exception"}, */
    {NULL}  /* Sentinel */
};

static PyTypeObject CustomType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "pipeline.Pipeline",
    .tp_doc = "object for interfacing with the image pipeline",
    .tp_basicsize = sizeof(PipelineObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = Custom_new,
    .tp_init = (initproc) Pipeline_init,
    .tp_dealloc = (destructor) Custom_dealloc,
    .tp_members = Pipeline_methods,
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
    if (PyType_Ready(&CustomType) < 0)
        return NULL;

    m = PyModule_Create(&custommodule);
    if (m == NULL)
        return NULL;

    Py_INCREF(&CustomType);
    if (PyModule_AddObject(m, "Pipeline", (PyObject *) &CustomType) < 0) {
        Py_DECREF(&CustomType);
        Py_DECREF(m);
        return NULL;
    }

    PipeLineError = PyErr_NewException("pipeline.error", NULL, NULL);
    Py_XINCREF(PipeLineError);
    if (PyModule_AddObject(m, "error", PipeLineError) < 0) {
        Py_XDECREF(PipeLineError);
        Py_CLEAR(PipeLineError);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
