#include "SocketServer.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <signal.h>
#include <csignal>
#include <re2/re2.h>

namespace ytmusic {
namespace util {

SocketServer::SocketServer(int port, int backlog, RequestHandler *handler) {
  this->handler = std::unique_ptr<RequestHandler>(handler);
  this->backlog = backlog;
  std::signal(SIGINT, [](int signum) { exit(0); });
  this->sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (this->sockfd < 0) {
    fprintf(stderr, "Failed to open socket.\n");
    exit(-1);
  }
  int socketOption = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &socketOption,
                 sizeof(socketOption))) {
    fprintf(stderr, "Failed to set reuseadd option.\n");
    exit(-1);
  }
  struct sockaddr_in serv_addr;
  memset((char *)&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    close(sockfd);
    fprintf(stderr,
            "Failed to bind socket to port %d. (perhaps it is occupied?)\n",
            port);
    exit(-1);
  }
}

void SocketServer::Start() {
  int conn_sock;
  unsigned clilen = sizeof(sockaddr_in);
  while (true) {
    listen(this->sockfd, this->backlog);
    struct sockaddr_in cli_addr;
    if ((conn_sock = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen)) <
        0) {
      fprintf(stderr, "Failed to accept connection.\n");
    }
    std::thread(&SocketServer::ServeRequest, this, conn_sock).detach();
  }
  printf("Server exiting\n");
  close(sockfd);
}

std::string urlDecode(std::string &SRC) {
  // Thanks tominko:
  // http://stackoverflow.com/questions/154536/encode-decode-urls-in-c
  std::string ret;
  char ch;
  int i, ii;
  for (i = 0; i < SRC.length(); i++) {
    if (int(SRC[i]) == 37) {
      sscanf(SRC.substr(i + 1, 2).c_str(), "%x", &ii);
      ch = static_cast<char>(ii);
      ret += ch;
      i = i + 2;
    } else {
      ret += SRC[i];
    }
  }
  return (ret);
}

void SocketServer::ServeRequest(int sockfd) {
  std::string raw_request, request;
  bool is_http = false;
  char buf[2] = {'\n', '\0'};
  while (true) {
    read(sockfd, buf, 1);
    if (buf[0] == '\n') {
      break;
    }
    raw_request += buf;
  }
  if (RE2::FullMatch(raw_request, "GET /(.+) HTTP/1\\.1.*", &request)) {
    is_http = true;
    request = urlDecode(request);
  } else {
    request = raw_request;
  }
  std::cout << request << std::endl;
  if (is_http) {
    dprintf(sockfd, "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\n%s\n\r\n",
            this->handler->HandleRequest(request).c_str());
  } else {
    dprintf(sockfd, "%s\n", this->handler->HandleRequest(request).c_str());
  }
  close(sockfd);
}

}  // namespace util
}  // namespace ytmusic
