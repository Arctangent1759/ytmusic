#include "URLDecoder.h"

#include <Python.h>
#include <string>

namespace ytmusic {

URLDecoder::URLDecoder(){
  Py_Initialize();
  PyObject* pName = PyString_FromString("livestreamer") ;
  PyObject* pModule = PyImport_Import(pName);
  this->pFunc = PyObject_GetAttrString(pModule, "streams");
  Py_DECREF(pName);
  Py_DECREF(pModule);
}

std::string URLDecoder::DecodeURL(std::string url){
  PyObject* pArgs = PyTuple_New(1);
  PyObject* pValue = PyString_FromString(url.c_str());
  PyTuple_SetItem(pArgs,0,pValue);
  PyObject* pStreams = PyObject_CallObject(this->pFunc, pArgs);
  PyObject* best_pStream = PyDict_GetItemString(pStreams,"best");
  PyObject* best_pStream_url = PyObject_GetAttrString(best_pStream,"url");
  std::string out = PyString_AsString(best_pStream_url);
  Py_DECREF(pArgs);
  Py_DECREF(pValue);
  Py_DECREF(pStreams);
  Py_DECREF(best_pStream);
  Py_DECREF(best_pStream_url);
  return out;
}

URLDecoder::~URLDecoder(){
  Py_DECREF(this->pFunc);
}

} //  namespace ytmusic
