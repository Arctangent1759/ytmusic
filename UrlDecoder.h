#include <Python.h>
#include <string>

namespace ytmusic {

class URLDecoder {
  public:
    URLDecoder();
    string DecodeURL(string url);
    ~URLDecoder();
  private:
    PyObject* pFunc;
}

} //  namespace ytmusic
