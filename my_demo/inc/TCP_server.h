#ifndef TCP_SERVER_H // 防止重复包含
#define TCP_SERVER_H

extern "C" {
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include <stdlib.h>
}

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <iostream>
#include <vector>
#include <lvgl/lvgl.h>
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <type_traits>
#include <sstream>
#include <string>
#include <map>
#include <chrono>
#include <iomanip>
#include <ctime>
#include "OssDownloader.h"

using namespace std;
class TCPServer {
  private:
    int server_socket;
    int port;
    string ip;
    int new_socket;
  public:
    TCPServer(int port,string ip);

    int TCP_server_connect();
    void distinguish_data(char *buf);
   void send_data(int socket, const string& data);
   int get_new_sorcket();
   
};
#endif 