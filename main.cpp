#include "httpfileserver.h"

#include <iostream>
#include <signal.h>

void signal_handler(int sig)
{
    cout<<"Signal number: "<<sig<<endl;
    switch(sig)
    {
        case SIGHUP:
            cout<<"SIGHUP"<<endl;
            break;

        case SIGTERM:
            cout<<"SIGTERM"<<endl;
            exit(0);
            break;

        case SIGKILL:
        cout<<"SIGKILL"<<endl;
        exit(0);
        break;
    }
}

int main()
{
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    HttpFileServer server;
    server.setDirectory("/home/anton/test");
    server.startToListen();

    return 0;
}
