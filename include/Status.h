#include <string>

#ifndef STATUS_H
#define STATUS_H

namespace ytmusic {
namespace util {

class Status {
  public:
    Status(std::string message, int code);
    Status(std::string message);
    Status(int code);
    Status();
    std::string GetMessage() const;
    int GetCode() const;
    operator bool() const;

  private:
    std::string message;
    int code;
};

}  // namespace util
}  // namespace ytmusic


#endif  // STATUS_H
