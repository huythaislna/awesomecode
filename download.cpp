#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <sstream>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <openssl/ssl.h>

//g++ client.cpp -o client -lssl -lcrypto
using namespace std;

string getHostname(string url);
string getPath(string url);
int getPort(string url);
string createGETRequest(string hostname, string path);
int openConnection(string hostname, int port);

int main(int argc, char const *argv[]) {
    string url = argv[1];
    string filename = argv[2];
    string hostname = getHostname(url);
    string path = getPath(url);
    int port = getPort(url);

    int sock = openConnection(hostname, port);

    int valread;
    FILE *file = NULL;
    remove(filename.c_str());
    file = fopen(filename.c_str(), "ab");

    char buffer[1024] = {0};
    char header[10000] = {0};
    string request = createGETRequest(hostname, path);
    string body = "";

    if (port == 443) {
        SSL_load_error_strings ();
        SSL_library_init ();
        SSL_CTX *ssl_ctx = SSL_CTX_new (SSLv23_client_method ());
        SSL *conn = SSL_new(ssl_ctx);
        SSL_set_fd(conn, sock);
        int err = SSL_connect(conn);
        if (err != 1)
        {
            cout << "Error SSL\n";
            abort(); // handle error
        }
        SSL_write(conn, request.c_str(), request.length());
        //read header
        //valread = SSL_read(conn, header, sizeof header);
        do {
                valread = SSL_read(conn, buffer, sizeof(buffer));
                fwrite(buffer , valread , 1, file);
                //printf("Received: \"%s\"\n", buffer);
        } while (valread > 0);
        SSL_CTX_free(ssl_ctx);
        SSL_free(conn); 
    } else {
        send(sock , request.c_str() , request.length() , 0);
        //read header
        //valread = read( sock , header, sizeof header);
        do {
            
            valread = read( sock , buffer, 1024);
            fwrite(buffer , valread , 1, file);
        } while (valread > 0);        
    }

    close(sock);
    fclose(file);
    return 0;
}


string getHostname(string url) {
    string hostname;
    string delimiter = "//";
    string temp = url.substr(url.find(delimiter) + delimiter.length(), url.length() - 1);
    delimiter = "/";
    try {
        hostname = temp.substr(0, temp.find(delimiter));
    } catch(...) {
        hostname = temp;
    }
    cout << "Hostname: " << hostname << endl;
    return hostname;
}

string getPath(string url) {
    string delimiter = "//";
    string temp = url.substr(url.find(delimiter) + delimiter.length(), url.length() - 1);
    delimiter = "/";
    string path = temp.substr(temp.find(delimiter), temp.length() - 1);
    if (path == "") return "/";
    cout << "Path: " << path << endl;
    return path;
}

int getPort(string url) {
    string delimiter = "//";
    string protocal = url.substr(0, url.find(delimiter));
    if (protocal == "https:") return 443;
    else return 80;
}

int openConnection(string hostname, int port) {
    int sock;
    struct sockaddr_in serv_addr;
    struct hostent * host = gethostbyname(hostname.c_str());
    memcpy(&serv_addr.sin_addr, host->h_addr, host->h_length);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    return sock;
}

string createGETRequest(string hostname, string path) {
    stringstream ss;
    ss << "GET "<< path << " HTTP/1.1\r\n"
    << "Host: "<< hostname << "\r\n"
    << "Accept: text/html\r\n"
    << "Connection: close\r\n"
    << "\r\n\r\n";
    
    string request = ss.str();
    return request;
}
