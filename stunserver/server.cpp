#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h> 
#include <iostream> 
#include <bits/stdc++.h> 



struct STUNXORMappedIPv4Address{
    unsigned char reserved;
    
    unsigned char family;
    
    unsigned short port;
    
    unsigned int address;
};

struct STUNAttributeHeader{
    unsigned short type;
    
    unsigned short length;

    STUNXORMappedIPv4Address ip;

};

struct STUNIncommingHeader{

    unsigned short type;
    
    unsigned short length;
    
    uint8_t identifier[16];
};

struct STUNMessageHeader{

    unsigned short type;
    

    unsigned short length;
    


    
    // Unique Transaction ID
    uint8_t identifier[16];


    uint16_t atttype;
    

    uint16_t attlength;

    uint8_t reserved;

    uint8_t protocol;


    
    uint16_t attport;

    uint8_t  attaddress[16];
};

#define IS_REQUEST(msg_type)       (((msg_type) & 0x0110) == 0x0000)
#define IS_INDICATION(msg_type)    (((msg_type) & 0x0110) == 0x0010)
#define IS_SUCCESS_RESP(msg_type)  (((msg_type) & 0x0110) == 0x0100)
#define IS_ERR_RESP(msg_type)      (((msg_type) & 0x0110) == 0x0110)

int main(){
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));

    unsigned char message[256];
    struct sockaddr_in from;
    socklen_t fromLen = sizeof(from);
    recvfrom(sock, message, sizeof(message), 0, reinterpret_cast<struct sockaddr*>(&from), &fromLen);

    char ip[16];
    inet_ntop(AF_INET, &from.sin_addr.s_addr, ip, sizeof(ip));
    std::cout << ip << ":" << ntohs(from.sin_port) << " - " << message[5] << std::endl;
    struct STUNMessageHeader* request = (STUNMessageHeader*) malloc(sizeof(struct STUNMessageHeader));


    struct STUNIncommingHeader* inc = (STUNIncommingHeader*) message;

    // request->type = htons(0x0101);

    // request->length = htons(0x000c);
    in_addr_t n = inet_addr("2001:700:300:4000:9d2f:26ec:eaba:c700");

    uint8_t* pPort;
    uint8_t* pIP;

    pPort = (uint8_t*)&(from.sin_port);
    pIP = (uint8_t*)&(n);

    // pPort[0] = pPort[0] ^ request->identifier[0];
    // pPort[1] = pPort[1] ^ request->identifier[1];
    
    for (size_t i = 0; i < 16; i++)
    {
        pIP[i] = pIP[i] ^ request->identifier[i];
    }

    for (size_t i = 0; i < 16; i++)
    {
        request->identifier[i] = inc->identifier[i];
    }

    
    // request->atttype = htons(0x0020);
    // request->attlength = htons(0x0008);
    // request->reserved = 0x0;
    // request->protocol = 0x01;
    // unsigned long ID = inc->identifier[0] + inc->identifier[1] + inc->identifier[2];
    // request->attport = htons(from.sin_port);
    // request->attaddress = htonl(pIP[0] | (pIP[1] << 8) | (pIP[2] << 16) | (pIP[3] << 24));
    // std::printf("%d\n", sizeof(request->attaddress)/sizeof(uint8_t));
    // std::printf("%d\n", sizeof(request->attport)/sizeof(uint8_t));


    request->type = htons(0x0101);
    request->length = htons(0x00018);
    // request->cookie = htonl(0x2112A442);
    // request->identifier[0] = htonl(0x4f6f4735);
    // request->identifier[1] = htonl(0x45677062);
    // request->identifier[2] = htonl(0x77737a47);
    request->type = htons(0x0101);
    request->atttype = htons(0x0020);
    request->attlength = htons(0x0014);
    request->reserved = 0x00;
    request->protocol = 0x02;
    request->attport = htons(0xcdaa);
    // for (size_t i = 0; i < 16; i++)
    // {
    //     request->attaddress[i] = pIP[i];
    // }
    
    
    // request->attaddress[0] = htonl(0x0113a342);
    // request->attaddress[1] = htonl(0x4c6f0735);
    // request->attaddress[2] = htonl(0xa848568e);
    // request->attaddress[3] = htonl(0x9dc9bd47);
    sendto(sock, request, sizeof(struct STUNMessageHeader), 0, (const struct sockaddr *) &from, sizeof(struct sockaddr_in));
  
}