#ifndef ResponseBuilderTests_hpp
#define ResponseBuilderTests_hpp

#include "../ResponseBuilder.hpp"
#include "../stuntypes.h"
#include "testUtils.hpp"
#include <stdlib.h>
#include <cassert>


void test_non_stun_request_sets_error_true(){
    char * request = (char *)malloc(sizeof(char));
    ResponseBuilder testBulder = ResponseBuilder(true, (STUNIncommingHeader *)request, create_client());
    assert(testBulder.isError()== true);
}

void test_stun_request_sets_error_false(){
    STUNIncommingHeader * request = createStunRequest();
    ResponseBuilder testBulder = ResponseBuilder(true, request, create_client());
    assert(testBulder.isError() == false);
}

class ResponseBuilderTests {
public:
    int test();
};

int ResponseBuilderTests::test() {
    test_non_stun_request_sets_error_true();
    test_stun_request_sets_error_false();
    std::cout << "All ErrorResponseTests passed " << std::endl;
    return 0;
}

#endif