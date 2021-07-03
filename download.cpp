#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#define PORT 80
using namespace std;

string getHostname(string url);
string getPath(string url);
int main(int argc, char const *argv[])
{
    string url = argv[1];
    string hostname = getHostname(url);
    string path = getPath(url);
    struct sockaddr_in serv_addr;
    struct hostent * host = gethostbyname(hostname.c_str());
    memcpy(&serv_addr.sin_addr, host->h_addr, host->h_length);

    FILE *file = NULL;
    remove("index.html");
    file = fopen("index.html", "ab");

    int sock = 0, valread;
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "\nSocket creation error\n";
        return -1;
    }
   
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cout << "\nConnection Failed \n";
        return -1;
    }
    
    stringstream ss;
    
    ss << "GET "<< path << " HTTP/1.1 \r\n"
    << "Host: "<< hostname << "\r\n"
    << "Connection: close\r\n"
    << "\r\n\r\n";
    
    string request = ss.str();

    if (send(sock , request.c_str() , request.length() , 0) > 0) {
        cout << "Send get request !!!\n";
    }

    do {
        valread = read( sock , buffer, 1024);
        fwrite(buffer , valread , 1, file);
    } while (valread > 0);

    fclose(file);
    return 0;
}


string getHostname(string url) {
    string delimiter = "//";
    string temp = url.substr(url.find(delimiter) + delimiter.length(), url.length() - 1);
    delimiter = "/";
    string hostname = temp.substr(0, temp.find(delimiter));
    return hostname;
}

string getPath(string url) {
    string delimiter = "//";
    string temp = url.substr(url.find(delimiter) + delimiter.length(), url.length() - 1);
    delimiter = "/";
    string path = temp.substr(temp.find(delimiter), temp.length() - 1);
    if (path == "") return "/";
    return path;
}

