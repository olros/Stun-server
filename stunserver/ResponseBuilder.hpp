#ifndef ResponseBuilder_hpp
#define ResponseBuilder_hpp
#include "SuccessResponseBuilder.hpp"


class ResponseBuilder{
    private:
        bool isIPV4;
        STUNIncommingHeader* inc;
        sockaddr_in client;
    public:
        ResponseBuilder();
        ResponseBuilder(bool isIPV4, STUNIncommingHeader* inc, sockaddr_in client);
        SuccessResponseBuilder buildSuccessResponse();

};

ResponseBuilder::ResponseBuilder() {}

ResponseBuilder::ResponseBuilder(bool isIPV4, STUNIncommingHeader* inc, sockaddr_in client){
    this->client = client;
    this->inc = inc;
    this->isIPV4 = isIPV4;
}


SuccessResponseBuilder ResponseBuilder::buildSuccessResponse(){
    SuccessResponseBuilder builder = SuccessResponseBuilder();
    return builder.setStunSuccessHeaders(inc)
    .setlength(isIPV4)
    .setAttLength(isIPV4)
    .setAttType(0)
    .setProtocol(isIPV4)
    .XORAttributes(client.sin_addr.s_addr, client.sin_port, isIPV4)
    .setPadding(0);
    
}


#endif