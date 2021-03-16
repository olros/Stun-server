#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h> 
#include <iostream>



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
    
    unsigned int cookie;
    
    unsigned int identifier[3];
};

struct STUNMessageHeader{

    unsigned short type;
    

    unsigned short length;
    

    unsigned int cookie;
    

    unsigned int identifier[3];

    uint16_t atttype;
    

    uint16_t attlength;

    uint8_t reserved;

    uint8_t protocol;


    
    uint16_t attport;

    uint32_t attaddress;
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
    inet_ntop(AF_INET, &from.sin_addr, ip, sizeof(ip));
    std::cout << ip << ":" << ntohs(from.sin_port) << " - " << message[5] << std::endl;
    struct STUNMessageHeader* request = (STUNMessageHeader*) malloc(sizeof(struct STUNMessageHeader));


    struct STUNIncommingHeader* inc = (STUNIncommingHeader*) message;

    request->type = inc->type;

    request->length = inc->length;

    request->cookie = inc->cookie;
    request->identifier[0] = inc->identifier[0];
    request->identifier[1] = inc->identifier[1];
    request->identifier[2] = inc->identifier[2];
    request->atttype = htons(0x0020);
    request->attlength = htons(0x0008);
    request->reserved = 0x00;
    request->protocol = 0x01;
    request->attport = htons(0x6bae);
    request->attaddress = htonl(0xa0e340e2);


    // request->type = htons(0x0101);
    // request->length = htons(0x000c);
    // request->cookie = htonl(0x2112A442);
    // request->identifier[0] = htonl(0x372f6e55);
    // request->identifier[1] = htonl(0x354f644a);
    // request->identifier[2] = htonl(0x66664c78);
    // request->atttype = htons(0x0020);
    // request->attlength = htons(0x0008);
    // request->reserved = 0x00;
    // request->protocol = 0x01;
    // request->attport = htons(0x6bae);
    // request->attaddress = htonl(0xa0e340e2);
    sendto(sock, request, sizeof(struct STUNMessageHeader), 0, (const struct sockaddr *) &from, sizeof(struct sockaddr_in));
  
}