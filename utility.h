#ifndef UTILITY_H_INCLUDE
#define UTILITY_H_INCLUDE

#include <iostream>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//sockets list
std::list<int> clients_list;


//server ip
#define SERVER_IP "127.0.0.1"

//server port
#define SERVER_PORT 8888

//epoll size
#define EPOLL_SIZE 1000

//msg buf size
#define BUF_SIZE 0xFFFF

#define SERVER_WELCOME "Welcome you join to the chat room！Your chat ID is : Client #%d"

#define SERVER_MESSAGE "ClientID %d say >> %s"

//exit
#define EXIT "EXIT"

#define CAUTION "There is only one int the char room"


/**
 * set no block
 * @param sockfd
 * @return
 */
int setnonblock(int sockfd)
{
    fcntl(sockfd,F_SETFL,fcntl(sockfd,F_GETFD,0) | O_NONBLOCK);
    return 0;
}


/**
 *
 * @param epollfd
 * @param fd
 * @param enable_et
 */
void addfd(int epollfd,int fd, bool enable_et){
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if(enable_et){
        ev.events = EPOLLIN | EPOLLET;
    }
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
    setnonblock(fd);
}




int sendBoradcastMessage(int clientfd)
{
    char buf[BUF_SIZE],message[BUF_SIZE];
    bzero(buf,BUF_SIZE);
    bzero(message,BUF_SIZE);
    printf("read from client(clientID=%d)\n",clientfd);
    int len = recv(clientfd,buf,BUF_SIZE,0);
    if(len == 0){
        close(clientfd);
        clients_list.remove(clientfd);
        printf("Client ID = %d closed.\n now there are %d client in the char room\n",clientfd,(int)clients_list.size());
    }else{
        if(clients_list.size() == 1){
            send(clientfd,CAUTION,strlen(CAUTION),0);
            return len;
        }
        sprintf(message,SERVER_MESSAGE,clientfd,buf);

        std::list<int>::iterator it;
        for (it = clients_list.begin(); it != clients_list.end() ; ++it) {
            if(*it != clientfd){
                if(send(*it,message,BUF_SIZE,0) < 0) { perror("error");exit(-1); }
            }
        }
    }
    return len;
}

#endif