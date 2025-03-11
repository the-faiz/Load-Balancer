#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace::std;

#define PORT 8080
#define MAXLEN 50

int keep_recv(int sockfd,string &recv_str){
    char buf[MAXLEN];
    int byt_recv=recv(sockfd,&buf[0],MAXLEN-1,0);
    buf[byt_recv] = '\0';
    if(byt_recv<0) return 0;
    recv_str+=buf;
    if(buf[byt_recv-1]=='\0'){
        return 0;
    }
    return 1;
}

string recv_msg(int sockfd){
    int status = 1;
    string recv_str="";
    while(status!=0){
        status = keep_recv(sockfd,recv_str);
    }
    return recv_str;
}

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;

    // SOCKET CREATION AT THE CLIENT
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Unable to create a socket at the client");
        exit(EXIT_FAILURE);
    }

    // ADDRESS SPECIFICATION AT THE CLIENT
    serv_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &serv_addr.sin_addr); // Server is on the same machine
    serv_addr.sin_port = htons(PORT);

    // CONNECT TO SERVER
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Unable to connect the client to the server");
        exit(EXIT_FAILURE);
    }
    cout<<"Connection Extablished with the Server"<<endl;
    string time = recv_msg(sockfd);
    cout<<"Received time: "<<time<<endl;
    cout<<"Closing the connection on the client side"<<endl;   
    close(sockfd);
    return 0;
}