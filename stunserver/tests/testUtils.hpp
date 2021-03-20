#ifndef testUtils_hpp
#define testUtils_hpp

#include "../stuntypes.h"
#include <stdlib.h> 
#include <arpa/inet.h>

STUNIncommingHeader* createStunRequest(){
    STUNIncommingHeader* request = (STUNIncommingHeader *) malloc(sizeof(STUNIncommingHeader));
    for (int i = 0; i<COOKIE_LENGTH; i++){
        request->identifier[i] = cookie[i];
    }
    for (int i = COOKIE_LENGTH; i<identifier_size-COOKIE_LENGTH; i++){
        request->identifier[i] = rand() % 10;
    }
    request->length = 0;
    request->type =htons(STUN_REQUEST_TYPE);
    return request;
}

in_addr_t createIp(){
    return htonl(rand());
} 

in_port_t createPort(){
    return htons(rand());
}





#endif