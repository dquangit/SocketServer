//
// Created by dquang on 5/27/17.
//


#ifndef SOCKETSERVER_MESSAGE_H
#define SOCKETSERVER_MESSAGE_H

#include <string>

class Message {
public:
    Message();
    Message(int id, std::string data);
    ~Message();
    Message* setData(std::string data);
    std::string getData();
    Message *setId(int id);
    int getId();
private:
    std::string data;
    int id;
};


#endif //SOCKETSERVER_MESSAGE_H
