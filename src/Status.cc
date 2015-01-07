#include "Status.h"
#include <iostream>

namespace ytmusic {
namespace util {

Status::Status(std::string message, int code) : message(message), code(code) {}
Status::Status(std::string message) : Status(message,1) {}
Status::Status(int code) : Status("",code) {}
Status::Status() : Status("",0) {}
Status::operator bool() const{
  return (this->code == 0);
}
Status::operator std::string() const{
  return this->GetMessage();
}
std::string Status::GetMessage() const {
  return this->message;
}
int Status::GetCode() const {
  return this->code;
}

}  // namespace util
}  // namespace ytmusic
