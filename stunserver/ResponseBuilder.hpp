#ifndef ResponseBuilder_hpp
#define ResponseBuilder_hpp
#include <stdlib.h> 
#include "SuccessResponseBuilder.hpp"
#include "ErrorResponseBuilder.hpp"
#include "stuntypes.h"

const  int COOKIE_LENGTH = 4;
const uint8_t STUN_COOKIE_B1 = 0x21;
const uint8_t STUN_COOKIE_B2 = 0x12;
const uint8_t STUN_COOKIE_B3 = 0xA4;
const uint8_t STUN_COOKIE_B4 = 0x42;
class ResponseBuilder{
    private:
        uint8_t cookie[COOKIE_LENGTH] = {STUN_COOKIE_B1,STUN_COOKIE_B2, STUN_COOKIE_B3, STUN_COOKIE_B4};
        bool isIPV4;
        bool isErrorRequest;
        STUNIncommingHeader* inc;
        sockaddr_in client;
        void checkHeader();
        void  checkIdentifier();
        void createIdentifier();

    public:
        bool isError();
        ResponseBuilder();
        ResponseBuilder(bool isIPV4, STUNIncommingHeader* inc, sockaddr_in client);
        SuccessResponseBuilder buildSuccessResponse();
        ErrorResponseBuilder buildErrorResponse(int errorCode, std::string errorMsg);

};

ResponseBuilder::ResponseBuilder() {}

ResponseBuilder::ResponseBuilder(bool isIPV4, STUNIncommingHeader* inc, sockaddr_in client){
    this->client = client;
    this->inc = inc;
    this->isIPV4 = isIPV4;
    this->isErrorRequest = false;
    checkIdentifier();
    checkHeader();

}


bool ResponseBuilder::isError(){
    return isErrorRequest;
}

void ResponseBuilder::checkHeader(){
    if(!IS_BINDING_REQUEST(inc->type))this->isErrorRequest = true;
}

void ResponseBuilder::checkIdentifier(){
    bool isStun = true;
    for (int i = 0; i<COOKIE_LENGTH; i++){
        if(this->inc->identifier[i] != cookie[i]){
            isStun = false;
        }
    }
    if(!isStun)createIdentifier();
}
void ResponseBuilder::createIdentifier(){
    isErrorRequest = true;
    for (int i = 0; i<COOKIE_LENGTH; i++){
        this->inc->identifier[i] = cookie[i];
    }
    for (int i = COOKIE_LENGTH; i<identifier_size-COOKIE_LENGTH; i++){
        this->inc->identifier[i] = rand() % 10;
    }
}

SuccessResponseBuilder ResponseBuilder::buildSuccessResponse(){
    return SuccessResponseBuilder()
    .setStunSuccessHeaders(inc)
    .setlength(isIPV4)
    .setAttLength(isIPV4)
    .setAttType(0)
    .setProtocol(isIPV4)
    .XORAttributes(client.sin_addr.s_addr, client.sin_port, isIPV4)
    .setPadding(0);
    
}

ErrorResponseBuilder ResponseBuilder::buildErrorResponse(int errorCode, std::string errorMsg){
    return ErrorResponseBuilder()
    .setStunErrorHeaders(inc)
    .setlength()
    .setAttLength()
    .setAttType()
    .setAttribute(errorCode)
    .setMSG(errorMsg);
    
}


#endif