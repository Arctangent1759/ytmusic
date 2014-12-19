// A wrapper for the python Livestreamer API. Decodes live stream URLs to
// their raw forms.

#include <Python.h>
#include <string>

#ifndef URLDECODER_H
#define URLDECODER_H

namespace ytmusic {

// Object for decoding live stream URLs. Only ever instantiate one of these.
class URLDecoder {
  public:
    // Initializes a python interpreter and imports relevant python modules.
    URLDecoder();
    // Decodes a url and returns the url of the raw video file.
    std::string DecodeURL(std::string url);
    // Deallocates the python modules and shuts down the python interpreter.
    ~URLDecoder();
  private:
    PyObject* pFunc;  // The python function being run by the decoder.
};

} //  namespace ytmusic

#endif  // URLDECODER_H
