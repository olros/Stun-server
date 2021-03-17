#ifndef ErrorResponseBuilder_hpp
#define ErrorResponseBuilder_hpp

#include "stuntypes.h"
#include <arpa/inet.h>

#define ErrorCode 0x0111
#define ErrorCodeType 0x0009
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
    res->length= htons(12);
    return *this;
}

ErrorResponseBuilder&  ErrorResponseBuilder::setAttLength(){
    res->attlength = htons(32);
    return *this;
}

ErrorResponseBuilder&  ErrorResponseBuilder::setAttType(){
    res->atttype = htons(ErrorCodeType);
    return *this;
}







#endif