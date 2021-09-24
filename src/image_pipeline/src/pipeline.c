#define PY_SSIZE_T_CLEAN
#include <python3.9/Python.h>
#include "structmember.h"
#include <stddef.h>

#include "../includes/image_io.h"
#include "../includes/image_processing.h"

static PyObject *Pipeline_Error;

typedef struct {
    PyObject_HEAD
    PyObject *infile_path;
    PyObject *outfile_path;
    PyObject *dataset_path;
    PyObject *sample_path;
    PyObject *info_path;
    image_data * image;
    int height, width, channels, color_type, bit_depth;
} PipelineObject;

static PyMemberDef Pipeline_members[] = {

    {"infile_path",     T_OBJECT_EX,    offsetof(PipelineObject, infile_path),  0, "the input file"                 },
    {"outfile_path",    T_OBJECT_EX,    offsetof(PipelineObject, outfile_path), 0, "the output file"                },
    {"dataset_path",    T_OBJECT_EX,    offsetof(PipelineObject, dataset_path), 0, "path for the full dataset"      },
    {"sample_path",     T_OBJECT_EX,    offsetof(PipelineObject, sample_path),  0, "path for the samples"           },
    {"info_path",       T_OBJECT_EX,    offsetof(PipelineObject, info_path),    0, "path for general info"          },
    {"height",          T_INT,          offsetof(PipelineObject, height),       0, "height of the image in pixels"  },
    {"width",           T_INT,          offsetof(PipelineObject, width),        0, "width of the image in pixels"   },
    {"channels",        T_INT,          offsetof(PipelineObject, channels),     0, "number of channels"             },
    {"color_type",      T_INT,          offsetof(PipelineObject, color_type),   0, "refer to libpng documentation"  },
    {"bit_depth",       T_INT,          offsetof(PipelineObject, bit_depth),    0, "bit depth per channel"          },

    {NULL}  // null terminator
};

static void
Pipeline_dealloc(PipelineObject *self)
{
    Py_XDECREF(self->infile_path);
    Py_XDECREF(self->outfile_path);
    Py_XDECREF(self->sample_path);
    Py_XDECREF(self->info_path);
    destroy_image_data(self->image);
    Py_TYPE(self)->tp_free((PyObject *) self);
}

static PyObject *
Pipeline_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PipelineObject *self;
    self = (PipelineObject *) type->tp_alloc(type, 0);
    if (self != NULL) {
        self->infile_path = PyUnicode_FromString("");
        if (self->infile_path == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->outfile_path = PyUnicode_FromString("");
        if (self->outfile_path == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->dataset_path = PyUnicode_FromString("");
        if (self->dataset_path == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->sample_path = PyUnicode_FromString("");
        if (self->sample_path == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->info_path = PyUnicode_FromString("");
        if (self->info_path == NULL) {
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
    static char *kwlist[] = {"infile_path", "outfile_path", "dataset_path", "sample_path", "info_path", NULL};
    PyObject *infile_path = NULL, *outfile_path = NULL, *dataset_path = NULL, *sample_path = NULL, *info_path = NULL, *tmp;

    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OOOOO", kwlist, &infile_path, &outfile_path, &dataset_path, &sample_path, &info_path))
        return -1;

    if (infile_path) {
        tmp = self->infile_path;
        Py_INCREF(infile_path);
        self->infile_path = infile_path;
        Py_XDECREF(tmp);
    }
    if (outfile_path) {
        tmp = self->outfile_path;
        Py_INCREF(outfile_path);
        self->outfile_path = outfile_path;
        Py_XDECREF(tmp);
    }
    if (dataset_path) {
        tmp = self->dataset_path;
        Py_INCREF(dataset_path);
        self->dataset_path = dataset_path;
        Py_XDECREF(tmp);
    }
    if (sample_path) {
        tmp = self->dataset_path;
        Py_INCREF(sample_path);
        self->sample_path = sample_path;
        Py_XDECREF(tmp);
    }
    if (info_path) {
        tmp = self->dataset_path;
        Py_INCREF(info_path);
        self->info_path = info_path;
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
    if (self->infile_path == NULL) {
        PyErr_SetString(Pipeline_Error, "input file missing");
        return NULL;
    }
    if (self->outfile_path == NULL) {
        PyErr_SetString(PyExc_AttributeError, "output file missing");
        return NULL;
    }
    if (self->dataset_path == NULL) {
        PyErr_SetString(PyExc_AttributeError, "No dataset path");
        return NULL;
    }
    if (self->sample_path == NULL) {
        PyErr_SetString(PyExc_AttributeError, "No sample path");
        return NULL;
    }
    if (self->info_path == NULL) {
        PyErr_SetString(PyExc_AttributeError, "No info path");
        return NULL;
    }

    return PyUnicode_FromFormat("%S %S %S %S %S", self->infile_path, self->outfile_path, self->dataset_path, self->sample_path, self->info_path);
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

    char * infile_path;
    if(!PyArg_ParseTuple(args, "s", &infile_path)){
        PyErr_SetString(Pipeline_Error, "path arguement required.");
        return NULL;
    }
    self->infile_path = PyUnicode_FromString(infile_path);

    if(check_header(infile_path) != 0) {
        PyErr_SetString(Pipeline_Error, "invalid file.");
        return NULL;
    }

    unsigned char ** pixels = read_image(infile_path, &(self->height), &(self->width), &(self->channels), &(self->bit_depth), &(self->color_type));
    self->image = initialise_data(pixels, self->height, self->width, self->channels);

    return PyLong_FromLong(1);

}

static PyObject *
Pipeline_save_to_file(PipelineObject *self, PyObject * args) {

    char * outfile_path;
    if(!PyArg_ParseTuple(args, "s", &outfile_path)){
        PyErr_SetString(Pipeline_Error, "path arguement required.");
        return NULL;
    }
    self->outfile_path = PyUnicode_FromString(outfile_path);

    if (self->image == NULL) {
        PyErr_SetString(Pipeline_Error, "Call to 'save_to_file' must follow 'load_file'.");
        return NULL;
    }
    unsigned char ** pixels = self->image->export_pixels(self->image);
    write_image(outfile_path, pixels, self->image->height, self->image->width, self->image->channels, self->bit_depth, self->color_type);

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

    char * dataset_path;
    char * sample_path;
    char * info_path;
    if(!PyArg_ParseTuple(args, "sss", &dataset_path, &sample_path, &info_path)){
        PyErr_SetString(Pipeline_Error, "path arguement required.");
        return NULL;
    }

    self->dataset_path = PyUnicode_FromString(dataset_path);
    self->sample_path = PyUnicode_FromString(sample_path);
    self->info_path = PyUnicode_FromString(info_path);

    if (self->image == NULL) {
        PyErr_SetString(Pipeline_Error, "file must be loaded first.");
        return NULL;
    }

    self->image->generate_dataset_from_image(self->image, dataset_path, sample_path, info_path);

    return PyLong_FromLong(1);

}

static PyObject *
Pipeline_convolution(PipelineObject *self, PyObject * args) {

    int type, dimensions;
    if(!PyArg_ParseTuple(args, "ii", &type, &dimensions)){
        PyErr_SetString(Pipeline_Error, "takes 2 arguements: type, dimensions");
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
        case 3:
            self->image->process(self->image, Sharpen, dimensions);
            break;
        default:
            return PyLong_FromLong(0);
    }

    return PyLong_FromLong(1);

}

static PyObject *
Pipeline_resize(PipelineObject *self, PyObject * args) {

    float scale_factor;
    if(!PyArg_ParseTuple(args, "f", &scale_factor)){
        PyErr_SetString(Pipeline_Error, "scale factor must be specified.");
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

    int x, y;
    if(!PyArg_ParseTuple(args, "ii", &x, &y)){
        PyErr_SetString(Pipeline_Error, "takes two arguements: x and y");
        return NULL;
    }

    if (self->image == NULL) {
        PyErr_SetString(Pipeline_Error, "file must be loaded first.");
        return NULL;
    }

    self->image->image_translation(self->image, x, y);

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

    return PyLong_FromLong(1);

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
