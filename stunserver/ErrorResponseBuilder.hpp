#ifndef ErrorResponseBuilder_hpp
#define ErrorResponseBuilder_hpp

#include "stuntypes.h"
#include <arpa/inet.h>
#include <string>

#define ErrorCode 0x0111
#define ErrorCodeType 0x0009
#define ERROR_LENGTH 136
#define ERROR_ATT_LENGTH 136

class ErrorResponseBuilder{
    private:
        struct StunErrorResponse* res;

    public:
        StunErrorResponse* getResponse();
        ErrorResponseBuilder();
        ErrorResponseBuilder&  setStunErrorHeaders(struct STUNIncommingHeader* inc);
        ErrorResponseBuilder&  setlength();
        ErrorResponseBuilder&  setAttLength();
        ErrorResponseBuilder&  setAttType();
        ErrorResponseBuilder&  setAttribute(int code);
        ErrorResponseBuilder&  setMSG(std::string msg);

};
ErrorResponseBuilder::ErrorResponseBuilder(){
    res = (StunErrorResponse*) malloc(sizeof(struct StunErrorResponse));
}

struct StunErrorResponse* ErrorResponseBuilder::getResponse(){
    return res;
}

ErrorResponseBuilder& ErrorResponseBuilder::setStunErrorHeaders(struct STUNIncommingHeader* inc){
    res->type = htons(ErrorCode);
    for(int i = 0; i < identifier_size; i++){
        res->identifier[i] = inc->identifier[i];
    }
    return *this;
}

ErrorResponseBuilder& ErrorResponseBuilder::setlength(){
    res->length = htons(ERROR_LENGTH);
    return *this;
}

ErrorResponseBuilder&  ErrorResponseBuilder::setAttLength(){
    res->attlength = htons(ERROR_ATT_LENGTH);
    return *this;
}

ErrorResponseBuilder&  ErrorResponseBuilder::setAttType(){
    res->atttype = htons(ErrorCodeType);
    return *this;
}

ErrorResponseBuilder&  ErrorResponseBuilder::setAttribute(int code){
    res->clss = code/100;
    res->nr = code%100;

    return *this;
}

ErrorResponseBuilder&  ErrorResponseBuilder::setMSG(std::string msg){
    for(int i = 0; i<msg.length(); i++){
        res->reason[i]  = msg[i];
    }
    return *this;
}   








#endif