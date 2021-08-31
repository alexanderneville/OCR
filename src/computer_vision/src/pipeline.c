#define PY_SSIZE_T_CLEAN
#include <python3.9/Python.h>
#include "structmember.h"

#include "../includes/image_processing.h"

typedef struct {
    PyObject_HEAD
    PyObject *infile;
    PyObject *outfile;
    PyObject *dumpfile;
    image_data * image;
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
    }
    return (PyObject *) self;
}

static int
Custom_init(PipelineObject *self, PyObject *args, PyObject *kwds)
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
    return 0;
}

static PyMemberDef Custom_members[] = {
    {"infile", T_OBJECT_EX, offsetof(PipelineObject, infile), 0,
     "the input file"},
    {"outfile", T_OBJECT_EX, offsetof(PipelineObject, outfile), 0,
     "the output file"},
    {"dumpfile", T_OBJECT_EX, offsetof(PipelineObject, dumpfile), 0,
     "where to dump the json data"},
    {NULL}  /* Sentinel */
};

static PyObject *
Custom_files(PipelineObject *self, PyObject *Py_UNUSED(ignored))
{
    if (self->infile == NULL) {
        PyErr_SetString(PyExc_AttributeError, "input file missing");
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

static PyMethodDef Custom_methods[] = {
    {"files", (PyCFunction) Custom_files, METH_NOARGS,
     "return the names of the files that the object will use / is using."
    },
    {NULL}  /* Sentinel */
};

static PyTypeObject CustomType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "custom2.Custom",
    .tp_doc = "Custom objects",
    .tp_basicsize = sizeof(PipelineObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_new = Custom_new,
    .tp_init = (initproc) Custom_init,
    .tp_dealloc = (destructor) Custom_dealloc,
    .tp_members = Custom_members,
    .tp_methods = Custom_methods,
};

static PyModuleDef custommodule = {
    PyModuleDef_HEAD_INIT,
    .m_name = "custom2",
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

    return m;
}
