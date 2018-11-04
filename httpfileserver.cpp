#include "httpfileserver.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include <fstream>

using namespace std;

string HttpFileServer::extractFileName(const string request)
{
    auto last = request.find_first_of(" ", 4);
    auto first = request.rfind("/", last)+1;

    return request.substr(first,last-first);
}

HttpFileServer::HttpFileServer(int port)
{
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if(listener < 0)
    {
        perror("Error in socket creation");
        exit(1);
    }

    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listener, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Error in binding");
        exit(1);
    }

    listen(listener, 1);

    loadMimes(DEFAULT_MIMES_FILE);

}

HttpFileServer::~HttpFileServer()
{
    cout<<"I am destructor";
    close(listener);
}

void HttpFileServer::setDirectory(string dir)
{
    directory = dir;
}

bool HttpFileServer::isExist(string fileName)
{
    ifstream is (directory+"/"+fileName);

    if (is) {
        is.close();
        return true;
    }

    is.close();
    return false;
}

void HttpFileServer::loadMimes(string mimesFilePath)
{
    char* buf = new char[256];
    ifstream is(mimesFilePath);
    if (is) {
        while(!is.eof()){
            is.getline(buf,256);
            string input(buf);
            auto spacePos = input.find_first_of(' ');
            string key = input.substr(0, spacePos);
            string value = input.substr(spacePos+1, input.length()-spacePos);
            mimes[key]=value;
        }
    }else{
        cout<<"Error while reading mime file: "<<mimesFilePath<<endl;
    }

    is.close();
    delete[] buf;
}

string HttpFileServer::readFileFromDir(string fileName)
{
    char* buffer;
    ulong length;

    ifstream is (directory+"/"+fileName, std::ifstream::binary);

    if (is) {

        //получаем длину файла
        is.seekg (0, is.end);
        length = is.tellg();
        is.seekg (0, is.beg);

        if (length == 0){
            cout<<"Empty file"<<endl;
            is.close();
            return "";
        }

        buffer = new char [length];

        std::cout << "Reading " << length << " characters... "<<endl;
        is.read (buffer, length);

        if (is){
            std::cout << "all characters read successfully."<<endl;
        } else{
            std::cout << "error: only " << is.gcount() << " could be read"<<endl;
        }

        return string(buffer, length);

        is.close();

    }else{
        is.close();
        cout<<"Error during open file: "<<directory+"/"+fileName<<endl;
        return "";
    }

}

string HttpFileServer::getExtension(string fileName)
{
    auto dotPos = fileName.find_last_of('.');
    return fileName.substr(dotPos+1,fileName.length()-dotPos);
}

bool HttpFileServer::sendFileToClient(int socket, string fileName)
{
    string file = readFileFromDir(fileName);
    if (file.length() == 0) return false;

    string extension = getExtension(fileName);

    cout<<"Extension: "<<extension<<" MIME type: "<<mimes[extension]<<endl;

    stringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Version: HTTP/1.1\r\n"
             << "Content-Type: "<<mimes[extension]<<"; "
             << "Content-Length: " << file.length()
             << "\r\n\r\n"
             << file;

    cout<<"To client: "<<response.str().size()<<endl;

    auto sendedBytes = send(socket, response.str().c_str(), response.str().size(), 0);
    // cout<<"Bytes sended: "<<sendedBytes<<endl;

    if (sendedBytes != response.str().size()) return false;

    return true;

}

bool HttpFileServer::sendNotFound(int socket, string fileName)
{
    stringstream response_body;
    stringstream response;

    cout<<"File not found, sending header 404"<<endl;
    response_body << "<title>File Not Found</title>\n"
                  << "<h1>File \""<<fileName<<"\" not found</h1>\n"
                  << "<p>Please specify correct filename</p>\n";

    // Формируем весь ответ вместе с заголовками
    response << "HTTP/1.1 404 Not Found\r\n"
             << "Version: HTTP/1.1\r\n"
             << "Content-Type: text/html; charset=utf-8\r\n"
             << "Content-Length: " << response_body.str().length()
             << "\r\n\r\n"
             << response_body.str();

    auto sendedBytes = send(socket, response.str().c_str(), response.str().size(), 0);
    // cout<<"Bytes sended: "<<sendedBytes<<endl;

    if (sendedBytes != response.str().size()) return false;

    return true;
}

void HttpFileServer::startToListen()
{
    int clientSocket;
    char buffer[BUFFER_SIZE];

    while(1)
    {
        clientSocket = accept(listener, NULL, NULL);
        if(clientSocket < 0)
        {
            perror("Error during accept");
            exit(1);
        }

        auto bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0);

       // cout<<"From client: "<<endl<<buffer<<endl;

        string get = string(buffer).substr(0,3);
        if (get.compare("GET")!=0) {
            cout<<"It is not GET request"<<endl;
        }

        string fileName = extractFileName(string(buffer , bytesRead));
        cout<<"fileName: "<<fileName<<endl;

        if (isExist(fileName)){
            sendFileToClient(clientSocket, fileName);
        }else{
            sendNotFound(clientSocket, fileName);
        }

        close(clientSocket);
    }
    close(listener);
}
