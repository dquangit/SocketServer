//
// Created by dquang on 5/26/17.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <map>
#include "../entity/Message.h"

#ifndef UNTITLED_SERVER_H
#define UNTITLED_SERVER_H

class Server {
private:
    int port;
    int socketId;
    struct addrinfo *serverInfo;
    std::map<std::string, void* > mapFunction;
    std::map<long, std::string> mapData;
    Server();
    ~Server();
    std::string getEventName(Message *message);
    void *getAddress(struct sockaddr *socketAddress);
    void bindAddress(struct addrinfo *serverInfo);
    void initializeSocket();
    void onReceiveMessage(Message *message);
    void mappingData(long threadId, std::string data);
    void *getCallback(std::string eventName);
    void startListening();

public:
    Server* setPort(int port);
    int getPort();
    static Server *getInstance();
    Server* mappingEvent(std::string eventName, void *response);
    std::string getRequestData(long threadId);
    void start();
};


#endif //UNTITLED_SERVER_H
