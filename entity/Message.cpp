//
// Created by dquang on 5/27/17.
//

#include "Message.h"

Message::Message() {

}

Message::Message(int id, std::string data) {
    this->data = data;
    this->id = id;
}

Message::~Message() {

}

Message* Message::setData(std::string data) {
    this->data = data;
    return this;
}

Message* Message::setId(int id) {
    this->id = id;
    return this;
}

int Message::getId() {
    return this->id;
}

std::string Message::getData() {
    return this->data;
}