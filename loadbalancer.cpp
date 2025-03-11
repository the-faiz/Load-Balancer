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
#include<thread>
#include<mutex> 
#include<chrono>
using namespace::std;

#define MAXLEN 50
#define PORT 8080
#define MAXCLIENTS 5
mutex server1mutex,server2mutex;

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


void update_load(uint16_t port, int &load,mutex &m){
    while(true){
        unique_lock<mutex> lock(m);
        int sockfd; //Setting up the load balancer as a client
        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            perror("Unable to create a socket at the client");
            exit(EXIT_FAILURE);
        }
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        inet_aton("127.0.0.1", &serv_addr.sin_addr);
        serv_addr.sin_port = htons(port);
    
        if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("Unable to connect the client to the server 1");
            exit(EXIT_FAILURE);
        }
        cout<<"Connection Extablished with teh Server 1"<<endl;
        string msg = "SEND LOAD";
        send(sockfd,msg.c_str(),msg.length()+1,0);
        load = stoi(recv_msg(sockfd));
        close(sockfd);
        lock.unlock();
        cout<<"Load at IP: "<<port<<" updated to  "<<load<<endl;
        sleep(5);
    }
}

string get_time(uint16_t port,mutex &m){
    unique_lock<mutex> lock(m);
    int sockfd; //Setting up the load balancer as a client
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Unable to create a socket at the client");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &serv_addr.sin_addr);
    serv_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Unable to connect the client to the server 1");
        exit(EXIT_FAILURE);
    }
    cout<<"Connection Extablished with teh Server 1"<<endl;
    string msg = "SEND TIME";
    send(sockfd,msg.c_str(),msg.length()+1,0);
    string time = recv_msg(sockfd);
    close(sockfd);
    lock.unlock();
    return time;
}


int main(int argc, char* argv[]){

    //Updating load every 5 seconds
    int load1=0,load2=0;
    uint16_t PORT1 = static_cast<uint16_t>(stoi(argv[1]));
    uint16_t PORT2 = static_cast<uint16_t>(stoi(argv[2]));
    thread t1(update_load,PORT1,ref(load1),ref(server1mutex));
    thread t2(update_load,PORT2,ref(load2),ref(server2mutex));

 
    int sockfd; //Setting up the load balancer as a server
    struct sockaddr_in lb_addr;
    socklen_t len = sizeof(lb_addr);
    sockfd = socket(AF_INET, SOCK_STREAM,0);
    if(sockfd<0){
        perror("Cannot create a socket at the load balancer");
        exit(EXIT_FAILURE);
    }
    lb_addr.sin_family = AF_INET;
    lb_addr.sin_addr.s_addr = INADDR_ANY;
    lb_addr.sin_port = htons(PORT);
    if(bind(sockfd,(struct sockaddr*)&lb_addr,sizeof(lb_addr))<0){
        perror("Unable to bind the load balancer");
        exit(EXIT_FAILURE);
    }
    listen(sockfd,5);
    
    fd_set readfds; //declaring the fd set
    int client_socket[MAXCLIENTS] = {0}; //storing all the clients file descriptors who are connected
    int max_fd,activity;
    while(true){
        FD_ZERO(&readfds); //clearing the fd set
        FD_SET(sockfd,&readfds); //adding the lb socket to the fd_set
        max_fd = sockfd; //Tracking the max fd

        activity = select(max_fd+1,&readfds,NULL,NULL,NULL); //wait indefinitely for an activity
        if(activity<0){
            perror("Selection error");
            exit(EXIT_FAILURE);
        }

        if(FD_ISSET(sockfd,&readfds)){
            //Checking if sockfd is ready for reading or a client is trying to connect
            int client_fd = accept(sockfd,(struct sockaddr*)&lb_addr,&len);
            if(client_fd<0){
                perror("Unable to accept the connection");
                exit(EXIT_FAILURE);
            }
            cout<<"New connection from: "<<inet_ntoa(lb_addr.sin_addr)<<endl;
            cout<<"Sending Time"<<endl;
            string msg;
            if(load1<load2) msg = get_time(PORT1,server1mutex);
            else msg = get_time(PORT2,server2mutex);
            send(client_fd,msg.c_str(),msg.length()+1,0);
            cout<<"Time Sent successfully"<<endl;
            close(client_fd);
        }
    }
    close(sockfd);
    t1.join();
    t2.join();
    return 0;
}