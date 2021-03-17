
#ifndef stuntypes_h
#define stuntypes_h

#include <iostream>

#define identifier_size 16
#define IPv4_PROTOCOL_VALUE 0x01
#define IPv6_PROTOCOL_VALUE 0x02
#define XOR_MAPPED_ADdRESSS 0x0020
#define IS_REQUEST(msg_type)       (((msg_type) & 0x0110) == 0x0000)
#define IS_INDICATION(msg_type)    (((msg_type) & 0x0110) == 0x0010)
#define IS_SUCCESS_RESP(msg_type)  (((msg_type) & 0x0110) == 0x0100)
#define IS_ERR_RESP(msg_type)      (((msg_type) & 0x0110) == 0x0110)

struct STUNXORMappedIPv4Address{
    unsigned char reserved;
    
    unsigned char family;
    
    unsigned short port;
    
    unsigned int address;
};


struct STUNIncommingHeader{

    uint16_t type;
    
    uint16_t length;
    
    uint8_t identifier[identifier_size];
};

struct STUNResponse{

    uint16_t type;

    uint16_t length;
    
    
    uint8_t identifier[identifier_size];


    uint16_t atttype;
    

    uint16_t attlength;

    uint8_t padding;

    uint8_t protocol;


    
    uint8_t port[2];

    uint8_t  ip[16];
};

struct STUNResponseIPV4{

    uint16_t type;

    uint16_t length;
    
    
    uint8_t identifier[identifier_size];


    uint16_t atttype;
    

    uint16_t attlength;

    uint8_t padding;

    uint8_t protocol;


    
    uint8_t port[2];

    uint8_t  ip[4];
};

#endif
