#ifndef ResponseBuilder_hpp
#define ResponseBuilder_hpp

#include <iostream>
#include "stuntypes.h"
#include <arpa/inet.h>

// TODO Fix the XOR function to correctly XOR the port and IP.
// TODO Maybe add a isIPv4 object attribute so that we dont need to check for it,
// but this depends on how we want to use SuccessResponseBuilder in the server class


#define SuccessCode 0x0001
class SuccessResponseBuilder{
    private:
        struct STUNResponse* res;
        void setPort(uint8_t *port);
        void setIP(uint8_t *ip, bool isIPv4);
    public:
        SuccessResponseBuilder();
        struct STUNResponse* getResponse();
        void setStunSuccessHeaders(struct STUNIncommingHeader* inc);
        void setlength(int length);
        void setAttType(int type);
        void setAttLength(int length);
        void setProtocol( bool isIPv4);
        void setPadding( int size);
        void setIP(uint8_t *ip);
        void XORAttributes(in_addr_t &ip,in_port_t &port, bool isIPv4);
};

SuccessResponseBuilder::SuccessResponseBuilder(){
    res = (STUNResponse*) malloc(sizeof(struct STUNResponse));
}

struct STUNResponse* SuccessResponseBuilder::getResponse(){
    return res;
}

void SuccessResponseBuilder::setStunSuccessHeaders(struct STUNIncommingHeader* inc){
    res->type = htons(SuccessCode);
    for(int i = 0; i < identifier_size; i++){
        res->identifier[i] = inc->identifier[i];
    }
}

void SuccessResponseBuilder::setlength(int length){
    res->length = htons(0x00c);
}

void SuccessResponseBuilder::setAttType(int type){
    res->atttype= htons(0x0020);//htons(type)
}
void SuccessResponseBuilder::setAttLength(int length){
    res->attlength = htons(0x0008);
}


void SuccessResponseBuilder::setProtocol(bool isIPv4){
    res->protocol = 0x01;//isIPv4 ? IPv4_PROTOCOL_VALUE : IPv6_PROTOCOL_VALUE; 
}

void SuccessResponseBuilder::setPadding(int size){
    res->padding = size == 0 ? 0x00 : 32-size;
}

void SuccessResponseBuilder::XORAttributes(in_addr_t &ip ,in_port_t &port, bool isIPv4){
    uint8_t* pPort = (uint8_t*)&(port);
    uint8_t* pIP = (uint8_t*)&(ip);

    pPort[0] = pPort[0] ^ res->identifier[0];
    pPort[1] = pPort[1] ^ res->identifier[1];

    
    for (size_t i = 0; i < (isIPv4? 4: 16); i++){
        pIP[i] = pIP[i] ^ res->identifier[i];
    }
    setPort(pPort);
    setIP(pIP, isIPv4);

}

void SuccessResponseBuilder::setPort(uint8_t *port){
    for (size_t i = 0; i < 2; i++){
        res->port[i] = port[i];
    }
        // res->port = ntohs(0x6bae);
    
}

void SuccessResponseBuilder::setIP(uint8_t *ip, bool isIPv4){
    for(int i = 0; i < (isIPv4? 4: 16); i++){
        res->ip[i] = ip[i];
    }
}

#endif


