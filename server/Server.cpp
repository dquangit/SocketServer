//
// Created by dquang on 5/26/17.
//

#include "Server.h"
#include <iostream>

extern "C" {
#include "../json/json.h"
}

#define BACK_LOG 20
#define MAX_DATA_SIZE 100

static Server *instance;

void sigchld_handler(int s) {
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

/**
 * Constructor
 * @param port
 */
Server::Server() {
}

/**
 * Destructor
 */
Server::~Server() {

}

/**
 * get Instance
 * @return
 */
Server* Server::getInstance() {
    if (instance == NULL) {
        instance = new Server();
    }
    return instance;
}

void Server::initializeSocket() {
    int port = this->port;
    if (port == 0) {
        port = 9999;
    }
    struct addrinfo hints;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    int rv;
    if ((rv = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &serverInfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }

}

/**
 * bind server address
 * loop through all the results and bind to the first we can
 * @param serverInfo
 */
void Server::bindAddress(struct addrinfo *serverInfo) {
    addrinfo *p;
    int option = 1;
    for(p = serverInfo; p != NULL; p = p->ai_next) {
        if ((socketId = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(socketId, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(socketId, p->ai_addr, p->ai_addrlen) == -1) {
            close(socketId);
            perror("server: bind");
            continue;
        }
        break;
    }
    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
}

/**
 * Start server to listen event
 */
void Server::start() {
    initializeSocket();
    bindAddress(serverInfo);
    freeaddrinfo(serverInfo);
    startListening();

}

/**
 * get client address ipv4 or ipv6
 * @param socketAddress
 * @return
 */
void* Server::getAddress(struct sockaddr *socketAddress) {
    if (socketAddress->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)socketAddress)->sin_addr);
    }
    return &(((struct sockaddr_in6*)socketAddress)->sin6_addr);
}

/**
 * mapping event name with callback
 * @param eventName
 * @param response
 * @return
 */
Server* Server::mappingEvent(std::string eventName, void *response) {
    this->mapFunction[eventName] = response;
    return this;
}

/**
 * set port
 * @param port
 * @return
 */
Server* Server::setPort(int port) {
    this->port = port;
    return this;
}

/**
 * get port
 * @return port
 */
int Server::getPort() {
    return this->port;
}

/**
 * Execute callback when receive message from client
 * @param message
 */
void Server::onReceiveMessage(Message *message) {
    long currentThread = (long) pthread_self();
    mappingData(currentThread, message->getData());
    std::string eventName = getEventName(message);
    void* callback = getCallback(eventName);
    if (callback == NULL) {
        printf("Event %s not found\n", eventName.c_str());
        return;
    }
    char* response = (char*) ((void* (*) ()) callback)();
    if (send(message->getId(), response, strlen(response), 0) == -1) {
        perror("could not send response\n");
    }
    close(message->getId());
    free(message);
}

/**
 * listening request from client, fork thread to process request
 */
void Server::startListening() {
    if (listen(socketId, BACK_LOG) == -1) {
        perror("listen");
        exit(1);
    }
    struct sigaction sa;
    socklen_t sin_size;
    struct sockaddr_storage clientAddress;
    int socketNewFd;
    sa.sa_handler = sigchld_handler; // reap all dead processes
    char s[INET6_ADDRSTRLEN];
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");
    while(1) {  // main accept() loop
        sin_size = sizeof clientAddress;
        socketNewFd = accept(socketId, (struct sockaddr *)&clientAddress, &sin_size);
        if (socketNewFd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(clientAddress.ss_family,
                  getAddress((struct sockaddr *)&clientAddress),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        if (!fork()) { // this is the child process
            char buf[MAX_DATA_SIZE];
            int dataByte = (int) recv(socketNewFd, buf, MAX_DATA_SIZE-1, 0);
            buf[dataByte] = '\0';
            onReceiveMessage(new Message(socketNewFd, std::string(buf)));
            exit(0);
        }
    }
}

/**
 * get callback from event name
 * @param eventName
 * @return callback
 */
void* Server::getCallback(std::string eventName) {
    return mapFunction[eventName];
}

/**
 * get event name from message
 * parse json and get event name
 * @param message
 * @return
 */
std::string Server::getEventName(Message *message) {
    JSON_Value *rootValue = json_parse_string(message->getData().c_str());
    JSON_Object *rootObject = json_value_get_object(rootValue);
    std::string eventName = json_object_get_string(rootObject, "event");
    return eventName;
}

/**
 * get request data
 * @param threadId
 * @return request data
 */
std::string Server::getRequestData(long threadId) {
    std::string data = mapData[threadId];
    return data == ""? "{}" : data;
}

/**
 * mapping data with thread ID
 * @param threadId
 * @param data
 */
void Server::mappingData(long threadId, std::string data) {
    mapData[threadId] = data;
}