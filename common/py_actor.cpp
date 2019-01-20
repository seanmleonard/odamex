#include <Python.h>
#include "py_main.h"
#include "py_actor.h"
#include "actor.h"

PyObject* SC_MarshallActor(AActor *mo)
{
	if (!mo)
		return NULL;

	PyObject* actor_dict = PyDict_New();

	PyObject* value;

	// TODO: add sprite
	// TODO: add snext, sprev
	// TODO: add subsector
	// TODO: add floorsector

	value = PyLong_FromLong(mo->x);
	PyDict_SetItemString(actor_dict, "x", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->y);
	PyDict_SetItemString(actor_dict, "y", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->z);
	PyDict_SetItemString(actor_dict, "z", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->prevx);
	PyDict_SetItemString(actor_dict, "prevx", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->prevy);
	PyDict_SetItemString(actor_dict, "prevy", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->prevz);
	PyDict_SetItemString(actor_dict, "prevz", value);
	Py_DECREF(value);

	value = PyLong_FromUnsignedLong(mo->angle);
	PyDict_SetItemString(actor_dict, "angle", value);
	Py_DECREF(value);

	value = PyLong_FromUnsignedLong(mo->prevangle);
	PyDict_SetItemString(actor_dict, "prevangle", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->frame);
	PyDict_SetItemString(actor_dict, "frame", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->pitch);
	PyDict_SetItemString(actor_dict, "pitch", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->prevpitch);
	PyDict_SetItemString(actor_dict, "prevpitch", value);
	Py_DECREF(value);

	value = PyLong_FromUnsignedLong(mo->effects);
	PyDict_SetItemString(actor_dict, "effects", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->floorz);
	PyDict_SetItemString(actor_dict, "floorz", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->ceilingz);
	PyDict_SetItemString(actor_dict, "ceilingz", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->dropoffz);
	PyDict_SetItemString(actor_dict, "dropoffz", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->radius);
	PyDict_SetItemString(actor_dict, "radius", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->height);
	PyDict_SetItemString(actor_dict, "height", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->momx);
	PyDict_SetItemString(actor_dict, "momx", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->momy);
	PyDict_SetItemString(actor_dict, "momy", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->momz);
	PyDict_SetItemString(actor_dict, "momz", value);
	Py_DECREF(value);

	value = PyLong_FromLong(mo->health);
	PyDict_SetItemString(actor_dict, "health", value);
	Py_DECREF(value);

	return actor_dict;
}


void SC_UnmarshallActor(PyObject* actor_dict, AActor* mo)
{
    PyObject* value;
	
	value = PyDict_GetItemString(actor_dict, "x");
	mo->x = PyLong_AsLong(value);

	value = PyDict_GetItemString(actor_dict, "y");
	mo->y = PyLong_AsLong(value);

	value = PyDict_GetItemString(actor_dict, "z");
	mo->z = PyLong_AsLong(value);

	value = PyDict_GetItemString(actor_dict, "prevx");
	mo->prevx = PyLong_AsLong(value);

	value = PyDict_GetItemString(actor_dict, "prevy");
	mo->prevy = PyLong_AsLong(value);

	value = PyDict_GetItemString(actor_dict, "prevz");
	mo->prevz = PyLong_AsLong(value);

	value = PyDict_GetItemString(actor_dict, "angle");
	mo->angle = PyLong_AsUnsignedLong(value);

	value = PyDict_GetItemString(actor_dict, "prevangle");
	mo->prevangle = PyLong_AsUnsignedLong(value);

	value = PyDict_GetItemString(actor_dict, "health");
	mo->health = PyLong_AsLong(value);
}
