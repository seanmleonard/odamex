#ifndef __PY_ACTOR_H__
#define __PY_ACTOR_H__

#include <Python.h>

PyObject* SC_MarshallActor(AActor *mo);
void SC_UnmarshallActor(PyObject* actor_dict, AActor *mo);

#endif // __PY_ACTOR_H__
