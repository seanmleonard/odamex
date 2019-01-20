#include <string>
#include <vector>

#include <Python.h>
#include "py_main.h"
#include "py_actor.h"

#include "m_fileio.h"

//
// Converts a file path to a Python module name.
//
static std::string ConvertFilePathToModuleName(const std::string& path)
{
	std::string module_name(path);

	// Trim off the ".py" filename extension.
    size_t last_dot = module_name.find_last_of('.');
    if (last_dot != std::string::npos && last_dot > 0)
		module_name = module_name.substr(0, last_dot);

	// Replace path separator with a dot
	for (std::string::iterator it = module_name.begin(); it != module_name.end(); ++it)
		if (*it == '/' || *it == '\\')
			*it = '.';

	// Handle "__init__.py"
	// "my_module/__init__.py" should be converted to "my_module"
	const std::string init_str("__init__");
	if (module_name.find(init_str) == module_name.length() - init_str.length())
		module_name = module_name.substr(0, module_name.length() - init_str.length());

	// Trim off any trailing "."
	while (module_name[module_name.length() - 1] == '.')
		module_name = module_name.substr(0, module_name.length() - 1);

	return module_name;
}


//
// PythonScriptRunner::PythonScriptRunner
//
PythonScriptRunner::PythonScriptRunner()
{
	setup();
}


//
// PythonScriptRunner::~PythonScriptRunner
//
PythonScriptRunner::~PythonScriptRunner()
{
	teardown();
}


//
// PythonScriptRunner::setup
//
// Initializes the Python interpreter and imports the modules for
// the game code.
//
int PythonScriptRunner::setup()
{
	Py_Initialize();
	importModules();
	return 0;
}


//
// PythonScriptRunner::teardown
//
// Tears down the Python interpreter.
//
int PythonScriptRunner::teardown()
{
	if (Py_FinalizeEx() < 0)
		return -1;
	return 0;
}


//
// PythonScriptRunner::run
//
// Executes a callable Python object, typically a function.
// The module must already be imported.
//
int PythonScriptRunner::run(const std::string& module_name, const std::string& func_name, std::vector<PyObject*> args)
{
	PyObject* py_args = PyTuple_New(args.size());
	for (size_t i = 0; i < args.size(); i++)
		PyTuple_SetItem(py_args, i, args[i]);

	int ret = -1;
	PyObject *module = PyImport_AddModule(module_name.c_str());

	if (module)
	{
		PyObject* func = PyObject_GetAttrString(module, func_name.c_str());
		if (func)
		{
			if (PyCallable_Check(func))
			{
				PyObject* py_ret = PyObject_CallObject(func, py_args);
				if (!py_ret)
				{
					Printf(PRINT_HIGH, "Error executing Python callable %s.%s\n",
							module_name.c_str(),
							func_name.c_str());
				}
				ret = PyLong_AsLong(py_ret);
				Py_XDECREF(py_ret);
			}
			Py_DECREF(func);
		}
		else
		{
			Printf(PRINT_HIGH, "Unknown Python callable %s.%s\n",
					module_name.c_str(),
					func_name.c_str());
		}
	}
	else
	{
		Printf(PRINT_HIGH, "Unknown Python module %s\n",
				module_name.c_str());
	}

	Py_DECREF(py_args);
	return ret;
}


//
// PythonScriptRunner::importModules
//
int PythonScriptRunner::importModules()
{
	const std::string base_path("/home/sean/odamex/python_scripting");
	return importModuleFromFile("test.py", base_path);
}


//
// PythonScriptRunner::importModule
//
// Imports a python module using the given name. The data parameter contains
// the module's source code as a null-terminated string.
//
int PythonScriptRunner::importModule(const std::string& path, const std::string& name, const char* data)
{
	PyObject* code_obj = Py_CompileString(data, path.c_str(), Py_file_input);
	if (code_obj == NULL)
		return -1;

	PyObject* module_obj = PyImport_ExecCodeModule(name.c_str(), code_obj);
	Py_DECREF(code_obj);

	if (module_obj == NULL)
		return -1;
	
	Py_DECREF(module_obj);

	return 0;
}


//
// PythonScriptRunner::importModuleFromFile
//
int PythonScriptRunner::importModuleFromFile(const std::string& path, const std::string& base_path)
{
	const std::string full_path = base_path + "/" + path;
	FILE* fp = fopen(full_path.c_str(), "rb");
	size_t size = M_FileLength(fp) + 1;

	char* data = new char[size];
	fread(data, 1, size -  1, fp);
	data[size - 1] = '\0';
	fclose(fp);

	const std::string module_name = ConvertFilePathToModuleName(path);
	int ret = importModule(path, module_name, data);

	delete [] data;

	return ret;
}



static PythonScriptRunner* py_runner = NULL;


void Py_Init()
{
	if (py_runner)
		Py_Shutdown();
	py_runner = new PythonScriptRunner();
}


void Py_Shutdown()
{
	if (py_runner)
		delete py_runner;
	py_runner = NULL;
}


void Py_RunActorThinker(AActor* mo)
{
	if (!mo)
		return;

	PyObject* actor_dict = SC_MarshallActor(mo);
	std::vector<PyObject*> args;
	args.push_back(actor_dict);

	py_runner->run("test", "run_actor_thinker", args);

	SC_UnmarshallActor(actor_dict, mo);
	Py_DECREF(actor_dict);
}
