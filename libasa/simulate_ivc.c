#define PY_SSIZE_T_CLEAN
#include <Python.h>

char* SimulateIvc(char* name){
	Py_Initialize();
	//name = PyUnicode_FromString((const char*) "./src/python");
	PyRun_SimpleString("import simulate");
	//char* pObjct; char* pDict; char* pModule; char* pName; char* pVal; 
	char* ret;
	PyObject* pName = PyUnicode_FromString("simulate");
	PyObject* pModule = PyImport_Import(pName);
	PyObject* pDict = PyModule_GetDict(pModule);
	PyObject* pObjct = PyDict_GetItemString(pDict, (const char*) "example");
	PyObject* pVal = PyObject_CallFunction(pObjct, (char*)"(s)", "3_win.cir");
	PyObject* pResultRepr = PyObject_Repr(pVal);
	ret = strdup(PyBytes_AS_STRING(PyUnicode_AsEncodedString(pResultRepr, "utf-8", "ERROR")));
	Py_Finalize();
	printf("DONE!! %s", ret);
	return ret;
}
