#include "utility.h"

int main(int argc, char * argv[]) {

    struct sockaddr_in serverAddr;
    serverAddr.sin_family= PF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    //create socket
    int listener = socket(PF_INET,SOCK_STREAM,0);
    if(listener < 0){
        perror("listener create failed");
        exit(-1);
    }

    //bind
    if (bind(listener,(struct sockaddr *)&serverAddr, sizeof(serverAddr))< 0){
        perror("bind failed");
        exit(-1);
    }

    //listen
    int ret = listen(listener,5);
    if (ret < 0 ){
        perror("listen failed");
    }
    printf("THE SERVER STAERT LISTEN : %s",SERVER_IP);

    //create epoll
    int epfd = epoll_create(EPOLL_SIZE);
    if(epfd < 0){
        perror("epoll create failed");
        exit(-1);
    }
    printf("epoll created, epfd=%d\n",epfd);
    static struct epoll_event events[EPOLL_SIZE];
    addfd(epfd,listener,true);
    while (1) {
        int epoll_event_count = epoll_wait(epfd,events,EPOLL_SIZE,-1);
        if(epoll_event_count < 0){
            perror("epoll failed");
            break;
        }
        for (int i = 0; i < epoll_event_count; ++i) {
            int sockfd = events[i].data.fd;
            if(sockfd == listener){
                struct sockaddr_in client_address;
                socklen_t client_addrLength = sizeof(struct sockaddr_in);
                int clientfd = accept(listener,(struct sockaddr*)&client_address,&client_addrLength);

                printf("client connectiong from: %s : %d (IP:PORT),client = %d\n",inet_ntoa(client_address.sin_addr),ntohs(client_address.sin_port),clientfd);
                addfd(epfd,clientfd,true);

                clients_list.push_back(clientfd);
                printf("Add new clientfd = %d to epoll\n",clientfd);
                printf("Now there are %d clients in the chat room\n",(int)clients_list.size());

                printf("welcome message\n");
                char message[BUF_SIZE];
                bzero(message,BUF_SIZE);
                sprintf(message,SERVER_WELCOME,clientfd);
                int ret = send(clientfd,message,BUF_SIZE,0);
                if(ret < 0){
                    perror("send error");
                    exit(-1);
                }
            } else {
                int ret = sendBoradcastMessage(sockfd);
                if(ret < 0){
                    perror("borad send error");
                    exit(-1);
                }
            }
        }

    }
    close(listener);
    close(epfd);
    return 0;

}