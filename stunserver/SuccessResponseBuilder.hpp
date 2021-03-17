#ifndef SuccessResponseBuilder_hpp
#define SuccessResponseBuilder_hpp

#include <iostream>
#include "stuntypes.h"
#include <arpa/inet.h>

// TODO Fix the XOR function to correctly XOR the port and IP.
// TODO Maybe add a isIPv4 object attribute so that we dont need to check for it,
// but this depends on how we want to use SuccessResponseBuilder in the server class


#define SuccessCode 0x0101
class SuccessResponseBuilder{
    private:
        struct STUNResponse* res;
        void setPort(uint8_t *port);
        void setIP(uint8_t *ip, bool isIPv4);
    public:
        long getlength();
        SuccessResponseBuilder();
        struct STUNResponse* getResponse();
        SuccessResponseBuilder&  setStunSuccessHeaders(struct STUNIncommingHeader* inc);
        SuccessResponseBuilder&  setlength(bool isIPv4);
        SuccessResponseBuilder&  setAttType(int type);
        SuccessResponseBuilder&  setAttLength(bool isIPv4);
        SuccessResponseBuilder&  setProtocol( bool isIPv4);
        SuccessResponseBuilder&  setPadding( int size);
        SuccessResponseBuilder& XORAttributes(in_addr_t &ip,in_port_t &port, bool isIPv4);
};

SuccessResponseBuilder::SuccessResponseBuilder(){
    res = (STUNResponse*) malloc(sizeof(struct STUNResponse));
}

struct STUNResponse* SuccessResponseBuilder::getResponse(){
    return res;
}

SuccessResponseBuilder&  SuccessResponseBuilder::setStunSuccessHeaders(struct STUNIncommingHeader* inc){
    res->type = htons(SuccessCode);
    for(int i = 0; i < identifier_size; i++){
        res->identifier[i] = inc->identifier[i];
    }
    return *this;
}

SuccessResponseBuilder&  SuccessResponseBuilder::setlength(bool isIPv4){
    res->length = htons(isIPv4? 12: 24);
    return *this;

}

SuccessResponseBuilder&  SuccessResponseBuilder::setAttType(int type){
    res->atttype= htons(XOR_MAPPED_ADdRESSS);
    return *this;
}
SuccessResponseBuilder&  SuccessResponseBuilder::setAttLength(bool isIPv4){
    res->attlength = htons(isIPv4 ? 8 : 20);
    return *this;

}


SuccessResponseBuilder&  SuccessResponseBuilder::setProtocol(bool isIPv4){
    res->protocol = isIPv4 ? IPv4_PROTOCOL_VALUE : IPv6_PROTOCOL_VALUE; 
    return *this;

}

SuccessResponseBuilder&  SuccessResponseBuilder::setPadding(int size){
    res->padding = ((size_t )res & 1) ? 0x01 : 0x00;
    return *this;
}

SuccessResponseBuilder& SuccessResponseBuilder::XORAttributes(in_addr_t &ip ,in_port_t &port, bool isIPv4){
    uint8_t* pPort = (uint8_t*)&(port);
    uint8_t* pIP = (uint8_t*)&(ip);

    pPort[0] = pPort[0] ^ res->identifier[0];
    pPort[1] = pPort[1] ^ res->identifier[1];

    
    for (size_t i = 0; i < (isIPv4? 4: 16); i++){
        pIP[i] = pIP[i] ^ res->identifier[i];
    }
    setPort(pPort);
    setIP(pIP, isIPv4);

    return *this;
}

void SuccessResponseBuilder::setPort(uint8_t *port){
    for (size_t i = 0; i < 2; i++){
        res->port[i] = port[i];
    }
    
}

void SuccessResponseBuilder::setIP(uint8_t *ip, bool isIPv4){
    for(int i = 0; i < (isIPv4? 4: 16); i++){
        res->ip[i] = ip[i];
    }
}

long SuccessResponseBuilder::getlength(){
    auto length = sizeof(res);
    return length;
}
#endif


