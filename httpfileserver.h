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
    int listener;
    int port;
    string directory;
    map<string, string> mimes;

private:
    string extractFileName(const string request);
    void loadMimes(string mimesFilePath);
    bool isExist(string fileName);
    string readFileFromDir(string fileName);
    string getExtension(string fileName);
    bool sendFileToClient(int socket, string fileName);
    bool sendNotFound(int socket, string fileName);

public:
    HttpFileServer(int port = DEFAULT_PORT);
    ~HttpFileServer();

    void setDirectory(string dir);
    void startToListen();


};

#endif // HTTPFILESERVER_H
