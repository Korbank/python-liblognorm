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

static PyObject* convert_object(json_object *obj);

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

  PyObject *result = NULL;

  struct json_object *log = NULL;
  int norm_result = ln_normalize(self->lognorm_context, log_entry,
                                 log_entry_length, &log);
  if (norm_result == 0) {
    // log != NULL here
    result = convert_object(log);
  } else {
    fprintf(stderr, "some kind of error (%d)\n", norm_result);
  }

  if (log != NULL)
    // yes, this is how is called the function freeing memory for the object
    json_object_put(log);

  return result;
}

//----------------------------------------------------------------------------

static PyObject* convert_scalar(json_object *obj);
static PyObject* convert_list(json_object *obj);
static PyObject* convert_hash(json_object *obj);

static
PyObject* convert_object(json_object *obj)
{
  switch (json_object_get_type(obj)) {
    case json_type_null:
    case json_type_boolean:
    case json_type_double:
    case json_type_int:
    case json_type_string:
      return convert_scalar(obj);
    break;
    case json_type_object:
      return convert_hash(obj);
    break;
    case json_type_array:
      return convert_list(obj);
    break;
  }
  // XXX: never reached
  return NULL;
}

static
PyObject* convert_scalar(json_object *obj)
{
  switch (json_object_get_type(obj)) {
    case json_type_null:
      Py_INCREF(Py_None);
      return Py_None;
    break;
    case json_type_boolean:
      if (json_object_get_boolean(obj)) {
        Py_INCREF(Py_True);
        return Py_True;
      } else {
        Py_INCREF(Py_False);
        return Py_False;
      }
    break;
    case json_type_double:
      return Py_BuildValue("d", json_object_get_double(obj));
    break;
    case json_type_int:
      return Py_BuildValue("l", json_object_get_int64(obj));
    break;
    case json_type_string:
      return Py_BuildValue("s", json_object_get_string(obj));
    break;
    default:
      // XXX: impossible, only for silencing warnings
      return NULL;
    break;
  }
}

static
PyObject* convert_list(json_object *obj)
{
  PyObject *result = Py_BuildValue("[]");

  int array_length = json_object_array_length(obj);
  int i;
  for (i = 0; i < array_length; ++i) {
    PyObject *item = convert_object(json_object_array_get_idx(obj, i));
    PyList_Append(result, item);
    Py_DECREF(item);
  }

  return result;
}

static
PyObject* convert_hash(json_object *obj)
{
  PyObject *result = Py_BuildValue("{}");
  json_object_iter iter;

  json_object_object_foreachC(obj, iter) {
    PyObject *value = convert_object(iter.val);
    PyDict_SetItemString(result, iter.key, value);
    Py_DECREF(value);
  }

  return result;
}

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
