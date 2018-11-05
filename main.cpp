#include "httpfileserver.h"

#include <iostream>


int main()
{
    HttpFileServer server;
    server.setDirectory("/home/anton/test");
    server.startToListen();

    return 0;
}
