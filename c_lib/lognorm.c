//----------------------------------------------------------------------------

#include <Python.h>
#include <string.h>
#include <liblognorm.h>

//----------------------------------------------------------------------------

#define MODULE_NAME "liblognorm"
#define TYPE_NAME   "Lognorm"

#define MODULE_DOCSTRING "Log normalization library."
#define TYPE_DOCSTRING   "liblognorm context"

//----------------------------------------------------------------------------

// struct for object instance
typedef struct {
  PyObject_HEAD
  ln_ctx lognorm_context;
} ObjectInstance;

// struct for object instance's class
static
PyTypeObject TypeObject = {
  PyObject_HEAD_INIT(NULL)
  0,                         /*ob_size*/
  MODULE_NAME "." TYPE_NAME, /*tp_name*/
  sizeof(ObjectInstance),    /*tp_basicsize*/
  0,                         /*tp_itemsize*/
  0,                         /*tp_dealloc*/
  0,                         /*tp_print*/
  0,                         /*tp_getattr*/
  0,                         /*tp_setattr*/
  0,                         /*tp_compare*/
  0,                         /*tp_repr*/
  0,                         /*tp_as_number*/
  0,                         /*tp_as_sequence*/
  0,                         /*tp_as_mapping*/
  0,                         /*tp_hash */
  0,                         /*tp_call*/
  0,                         /*tp_str*/
  0,                         /*tp_getattro*/
  0,                         /*tp_setattro*/
  0,                         /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,        /*tp_flags*/
  TYPE_DOCSTRING,            /* tp_doc */
};

// methods for Lognorm instance
static
PyMethodDef object_methods[] = {
  {NULL}  /* sentinel */
};

//----------------------------------------------------------------------------

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif

// XXX: module entry point needs to have "initliblognorm" name
PyMODINIT_FUNC initliblognorm(void) 
{
  PyObject* module;

  TypeObject.tp_new = PyType_GenericNew;
  if (PyType_Ready(&TypeObject) < 0)
    return;

  module = Py_InitModule3(MODULE_NAME, object_methods, MODULE_DOCSTRING);

  Py_INCREF(&TypeObject);
  PyModule_AddObject(module, TYPE_NAME, (PyObject *)&TypeObject);
}

//----------------------------------------------------------------------------
// vim:ft=c:foldmethod=marker
