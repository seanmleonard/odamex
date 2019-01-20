#ifndef __PY_MAIN_H__
#define __PY_MAIN_H__

#include <string>
#include <vector>
#include <Python.h>


class PythonScriptRunner
{
public:
	PythonScriptRunner();
	~PythonScriptRunner();

	int run(const std::string& module_name, const std::string& func_name, std::vector<PyObject*> args);

private:
	int setup();
	int teardown();
	int importModule(const std::string& path, const std::string& name, const char* data);
	int importModuleFromFile(const std::string& path, const std::string& base_path);
	int importModules();
	void handleException();
};


void Py_Init();
void Py_Shutdown();

class AActor;
void Py_RunActorThinker(AActor* mo);

#endif // __PY_MAIN_H__
