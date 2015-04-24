//----------------------------------------------------------------------------

#include <Python.h>
#include <string.h>
#include <liblognorm.h>

#include <stdio.h>

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

//----------------------------------------------------------------------------
// __init__()/__del__() {{{
// well, sort of

static
int obj_init(ObjectInstance *self, PyObject *args, PyObject *kwargs)
{
  char *rulebase;

  static char *kwlist[] = {"rules", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s", kwlist, 
                                   &rulebase))
    return -1;

  self->lognorm_context = ln_initCtx();
  ln_loadSamples(self->lognorm_context, rulebase);
  // TODO: check for errors

  return 0;
}

static
void obj_dealloc(ObjectInstance *self)
{
  ln_exitCtx(self->lognorm_context);
  self->ob_type->tp_free((PyObject *)self);
}

// }}}
//----------------------------------------------------------------------------

// result = lognorm.normalize(log = "...", strip = True)
static
PyObject* normalize(ObjectInstance *self, PyObject *args, PyObject *kwargs)
{
  char *log_entry;
  int log_entry_length;
  PyObject *strip = NULL;

  //-------------------------------------------------------
  // parsing arguments {{{

  static char *kwlist[] = {"log", "strip", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s#|O", kwlist,
                                   &log_entry, &log_entry_length, &strip))
    return NULL;

  if (log_entry_length == 0) {
    Py_INCREF(Py_None);
    return Py_None;
  }

  // }}}
  //-------------------------------------------------------
  // strip = True {{{

  if (strip != NULL && PyObject_IsTrue(strip)) {
    while (log_entry_length > 0 &&
           (log_entry[log_entry_length - 1] == '\n' ||
            log_entry[log_entry_length - 1] == '\r' ||
            log_entry[log_entry_length - 1] == '\t' ||
            log_entry[log_entry_length - 1] == ' '))
      log_entry_length--;
  }

  // }}}
  //-------------------------------------------------------

  struct json_object *log = NULL;
  int result = ln_normalize(self->lognorm_context, log_entry,
                            log_entry_length, &log);
  if (result == 0) {
    // log != NULL here
    fprintf(stdout, "> %s\n", json_object_to_json_string(log));
  } else {
    fprintf(stderr, "some kind of error (%d)\n", result);
  }

  if (log != NULL)
    // yes, this is how is called the function freeing memory for the object
    json_object_put(log);

  Py_INCREF(Py_None);
  return Py_None;
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Python module administrative stuff
//----------------------------------------------------------------------------
// static variables {{{

// methods for Lognorm instance
static
PyMethodDef object_methods[] = {
  {"normalize", (PyCFunction)normalize, METH_VARARGS | METH_KEYWORDS,
    "parse log line to dict object"},
  {NULL}  /* sentinel */
};

// struct for object instance's class
static
PyTypeObject TypeObject = {
  PyObject_HEAD_INIT(NULL)
  0,                         /* ob_size           */
  MODULE_NAME "." TYPE_NAME, /* tp_name           */
  sizeof(ObjectInstance),    /* tp_basicsize      */
  0,                         /* tp_itemsize       */
  (destructor)obj_dealloc,   /* tp_dealloc        */
  0,                         /* tp_print          */
  0,                         /* tp_getattr        */
  0,                         /* tp_setattr        */
  0,                         /* tp_compare        */
  0,                         /* tp_repr           */
  0,                         /* tp_as_number      */
  0,                         /* tp_as_sequence    */
  0,                         /* tp_as_mapping     */
  0,                         /* tp_hash           */
  0,                         /* tp_call           */
  0,                         /* tp_str            */
  0,                         /* tp_getattro       */
  0,                         /* tp_setattro       */
  0,                         /* tp_as_buffer      */
  Py_TPFLAGS_DEFAULT,        /* tp_flags          */
  TYPE_DOCSTRING,            /* tp_doc            */
  0,		                     /* tp_traverse       */
  0,		                     /* tp_clear          */
  0,		                     /* tp_richcompare    */
  0,		                     /* tp_weaklistoffset */
  0,		                     /* tp_iter           */
  0,		                     /* tp_iternext       */
  object_methods,            /* tp_methods        */
  0,                         /* tp_members        */
  0,                         /* tp_getset         */
  0,                         /* tp_base           */
  0,                         /* tp_dict           */
  0,                         /* tp_descr_get      */
  0,                         /* tp_descr_set      */
  0,                         /* tp_dictoffset     */
  (initproc)obj_init,        /* tp_init           */
};

// }}}
//----------------------------------------------------------------------------
// module initializer {{{

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

// }}}
//----------------------------------------------------------------------------
// vim:ft=c:foldmethod=marker
