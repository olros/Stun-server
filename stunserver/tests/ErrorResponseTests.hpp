#ifndef ErrorResponseTests_hpp
#define ErrorResponseTests_hpp

#include "../ErrorResponseBuilder.hpp"
#include "../stuntypes.h"
#include "testUtils.hpp"
#include <string.h>

#include <cassert>



void test_setErrorStunHeaders_sets_the_correct_headers(){
    ErrorResponseBuilder testBuilder = ErrorResponseBuilder();
    STUNIncommingHeader* request = createStunRequest();
    testBuilder.setStunErrorHeaders(request);
    StunErrorResponse* res = testBuilder.getResponse();
    for (int i = 0; i<identifier_size; i++){
        assert(res->identifier[i] == request->identifier[i]);
    }
    assert(res->type ==  htons(ErrorCode));
}

void test_setLength_Error_sets_correct_length(){
    ErrorResponseBuilder testBuilder = ErrorResponseBuilder();
    testBuilder.setlength();
    assert(testBuilder.getResponse()->length == htons(ERROR_LENGTH));
    testBuilder.setlength();
    assert(testBuilder.getResponse()->length == htons(ERROR_LENGTH));
}

void test_setAttLength_Error_sets_correct_length(){
    ErrorResponseBuilder testBuilder = ErrorResponseBuilder();
    testBuilder.setAttLength();
    assert(testBuilder.getResponse()->attlength == htons(ERROR_ATT_LENGTH));
    testBuilder.setAttLength();
    assert(testBuilder.getResponse()->attlength == htons(ERROR_ATT_LENGTH));
}

void test_setAttType_Error_sets_correct_Type(){
    ErrorResponseBuilder testBuilder = ErrorResponseBuilder();
    testBuilder.setAttType();
    assert(testBuilder.getResponse()->atttype == htons(ErrorCodeType));
}

void test_setAttribute_Error_sets_correct_class_and_nr(){
    int errorCode = 400;
    ErrorResponseBuilder testBuilder = ErrorResponseBuilder();
    testBuilder.setAttribute(errorCode);
    assert(testBuilder.getResponse()->clss == errorCode/100);
    assert(testBuilder.getResponse()->nr == errorCode%100);
}

void test_setMSG_sets_correct_MSG(){
    std::string errorCode = "Test message";
    ErrorResponseBuilder testBuilder = ErrorResponseBuilder();
    testBuilder.setMSG(errorCode);
    for(int i = 0; i<errorCode.length();i++){
        assert(testBuilder.getResponse()->reason[i]==errorCode.c_str()[i]);
    }
}


class ErrorResponseTests {
public:
    int test();
};

int ErrorResponseTests::test() {
    test_setErrorStunHeaders_sets_the_correct_headers();
    test_setLength_Error_sets_correct_length();
    test_setAttLength_Error_sets_correct_length();
    test_setAttType_Error_sets_correct_Type();
    test_setAttribute_Error_sets_correct_class_and_nr();
    std::cout << "All ErrorResponseTests passed " << std::endl;
    return 0;
}
#endif