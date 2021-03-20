#include "../SuccessResponseBuilder.hpp"
#include "../stuntypes.h"
#include "testUtils.hpp"
#include <cassert>


#define SuccessCode 0x0101

void test_setStunHeaders_sets_the_correct_headers(){
    SuccessResponseBuilder testBuilder = SuccessResponseBuilder();
    STUNIncommingHeader* request = createStunRequest();
    testBuilder.setStunSuccessHeaders(request);
    STUNResponse* res = testBuilder.getResponse();
    for (int i = 0; i<identifier_size; i++){
        assert(res->identifier[i] == request->identifier[i]);
    }
    assert(res->type == SuccessCode);

}
void test_setLength_sets_correct_length(){
    SuccessResponseBuilder testBuilder = SuccessResponseBuilder();
    testBuilder.setlength(true);
    assert(testBuilder.getResponse()->length == htons(IPV4_LENGTH));
    testBuilder.setlength(false);
    assert(testBuilder.getResponse()->length == htons(IPV6_LENGTH));
}

void test_setAttLength_sets_correct_length(){
    SuccessResponseBuilder testBuilder = SuccessResponseBuilder();
    testBuilder.setAttLength(true);
    assert(testBuilder.getResponse()->attlength == htons(IPV4_ATTLENGTH));
    testBuilder.setAttLength(false);
    assert(testBuilder.getResponse()->attlength == htons(IPV6_ATTLENGTH));
}

void test_setProtocol_sets_correct_protocol(){
    SuccessResponseBuilder testBuilder = SuccessResponseBuilder();
    testBuilder.setProtocol(true);
    assert(testBuilder.getResponse()->protocol == IPv4_PROTOCOL_VALUE);
    testBuilder.setProtocol(false);
    assert(testBuilder.getResponse()->protocol == IPv6_PROTOCOL_VALUE);
}

void test_setAttType_sets_correct_Type(){
    SuccessResponseBuilder testBuilder = SuccessResponseBuilder();
    testBuilder.setAttType(XOR_MAPPED_ADDRESSS);
    assert(testBuilder.getResponse()->atttype == htons(XOR_MAPPED_ADDRESSS));
}

void test_XORAttributes_XORs_correctly(){
    SuccessResponseBuilder testBuilder = SuccessResponseBuilder();
    STUNIncommingHeader* request = createStunRequest();
    testBuilder.setStunSuccessHeaders((request));
    in_addr_t ip = createIp();
    in_port_t port = createPort();
    uint8_t* iparr = (uint8_t*)&(ip);
    uint8_t* portarr = (uint8_t*)&(port);
    testBuilder.XORAttributes(ip, port, true);
    STUNResponse* res = testBuilder.getResponse();
    for(int i = 0; i<4;i++){
        assert((res->ip[i]) == iparr[i]);
    }
    for(int i = 0; i<2;i++){
        assert((res->port[i]) == portarr[i]);
    }
    
}


class SuccessResponseTests {
public:
    int test();
};

int SuccessResponseTests::test() {
    test_setStunHeaders_sets_the_correct_headers();
    test_setLength_sets_correct_length();
    test_setAttLength_sets_correct_length();
    test_setProtocol_sets_correct_protocol();
    test_setAttType_sets_correct_Type();
    test_XORAttributes_XORs_correctly();
    std::cout << "All SuccessResponseTests passed " << std::endl;
    return 0;
}