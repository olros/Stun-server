#include "WorkersTests.hpp"
#include "SuccessResponseTests.hpp"
#include "ErrorResponseTests.hpp"


int main(){
    WorkersTests().test() ;
    SuccessResponseTests().test();
    ErrorResponseTests().test();
}

