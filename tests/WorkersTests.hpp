#include "../stunserver/Workers.hpp"
#include <string>
#include <unistd.h>
#include <cassert>

//Lot easier to call global functions than to call class functions
void test_postAfter_waits_before_running_function() {
    Workers event_loop(1);
    std::vector <std::string> orderOfCompletion;

    event_loop.start();
    event_loop.post_after([&orderOfCompletion] {
        orderOfCompletion.emplace_back("Should finish be last");
    }, [] {
        sleep(1);
    });

    event_loop.post([&orderOfCompletion] {
        orderOfCompletion.emplace_back("Should finish first");
    });

    event_loop.stop();
    event_loop.join();
    assert(orderOfCompletion[0] == "Should finish first" && orderOfCompletion[1] == "Should finish be last" &&
           orderOfCompletion.size() == 2);
}

void test_Post_tasks_are_completed_in_right_order() {
    Workers event_loop(1);
    std::vector<int> orderOfCompletion;
    //testing a thousand times to increase probability of getting an error if there exists one
    for (int i = 0; i < 10; ++i) {
        //Have to pass copy of i
        event_loop.post([&orderOfCompletion, i] {
            orderOfCompletion.emplace_back(i);
        });
    }

    event_loop.start();
    event_loop.stop();
    event_loop.join();

    for (int i = 0; i < 10; ++i) {
        assert(orderOfCompletion[i] == i);
    }
}

class WorkersTests {
public:
    int test();
};

int WorkersTests::test() {
    test_postAfter_waits_before_running_function();
    test_Post_tasks_are_completed_in_right_order();
    std::cout << "All test passed" << std::endl;
    return 0;
}

