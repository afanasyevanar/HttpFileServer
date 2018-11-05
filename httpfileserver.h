#ifndef HTTPFILESERVER_H
#define HTTPFILESERVER_H

#include <string>
#include <map>

#define DEFAULT_PORT 8080
#define BUFFER_SIZE 1024
#define DEFAULT_MIMES_FILE "mimes.txt"

using namespace std;

class HttpFileServer
{

private:
    int listener;
    uint16_t port;
    string directory;
    map<string, string> mimes;

private:
    string extractFileName(const string request) const;
    void loadMimes(string mimesFilePath=DEFAULT_MIMES_FILE);
    bool isExist(string fileName) const;
    string readFileFromDir(string fileName) const;
    string getExtension(string fileName) const;
    bool sendFileToClient(int socket, string fileName) const;
    bool sendNotFound(int socket, string fileName) const;
    void processConnection(int clientSocket) const;
    static void* processConnectionInThread(void* threadArgs);

public:
    HttpFileServer(uint16_t port = DEFAULT_PORT);
    ~HttpFileServer();

    void setDirectory(string dir);
    void startToListen();

};

#endif // HTTPFILESERVER_H
