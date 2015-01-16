#include <ncurses.h>
#include <fstream>
#include <map>
#include <vector>
#include <cmath>
#include <iostream>

#include "ClientApp.h"
#include "YTMClient.h"

int main() {
  YTMClient::ClientApp app(new YTMClient::YTMClient("localhost",1759));
  app.Run();
}
