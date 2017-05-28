#include <iostream>
#include "server/Server.h"
#include "model/Handler.h"
extern "C" {
#include "json/json.h"
}

char *signIn() {
    std::string message = Server::getInstance()->getRequestData(pthread_self());
    JSON_Value *rootValue = json_parse_string(message.c_str());
    JSON_Object *rootObject = json_value_get_object(rootValue);
    char* userName = (char*) json_object_get_string(rootObject, "username");
    char* password = (char*) json_object_get_string(rootObject, "password");
    return (char*) SHDocument::Handler::getInstance()->signIn(userName, password).c_str();
}

char * signUp() {
    std::string message = Server::getInstance()->getRequestData(pthread_self());
    JSON_Value *rootValue = json_parse_string(message.c_str());
    JSON_Object *rootObject = json_value_get_object(rootValue);
    char* userName = (char*) json_object_get_string(rootObject, "username");
    char* password = (char*) json_object_get_string(rootObject, "password");
    return (char*) SHDocument::Handler::getInstance()->signUp(userName, password).c_str();
}

int main() {
    Server *server = Server::getInstance();
    server
            ->setPort(1234)
            ->mappingEvent((char *) "signIn", (void *) signIn)
            ->mappingEvent((char *) "signUp", (void *) signUp)
        ->start();
    std::cout << "Hello, World!" << std::endl;
    return 0;
}