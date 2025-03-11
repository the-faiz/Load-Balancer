#include<iostream>
#include<cstdlib>
#include<cstring>
#include<unistd.h>
#include<limits.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<ctime>
using namespace::std;

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

int main(int argc, char* argv[]){
    uint16_t PORT = static_cast<uint16_t>(stoi(argv[1]));
    srand(time(0));
    int sockfd,newsockfd;
    socklen_t lblen;
    struct sockaddr_in lb_addr, serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM,0);
    if(sockfd<0){
        perror("Cannot create a socket at teh server");
        exit(EXIT_FAILURE);
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0){
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    listen(sockfd,5);


    //TCP Iterative Server
    while(true){
        lblen  = sizeof(lb_addr);
        newsockfd = accept(sockfd,(struct sockaddr*)&lb_addr,&lblen);
    
        if(newsockfd<0){
            perror("Accept error with the Load Balancer");
            exit(EXIT_FAILURE);
        }
    
        cout<<"Connection Established with the Load Balancer"<<endl;
        string input = recv_msg(newsockfd);
        if(input=="SEND LOAD"){
            int load = rand()%100+1;
            string output = to_string(load);
            send(newsockfd,output.c_str(),output.length(),0);
            cout<<"Load Send: "<<load<<endl;
        }else{
            time_t now = time(0);
            string dt = ctime(&now);
            send(newsockfd,dt.c_str(),dt.length()+1,0);
            cout<<"Time Sent: "<<dt<<endl;
        }
        cout<<"Closing the connection on the Load Balancer"<<endl;
        close(newsockfd);
    }
    close(sockfd);

    return 0;
}