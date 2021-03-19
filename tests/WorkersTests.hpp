#include "../stunserver/Workers.hpp"
#include <string>
#include <unistd.h>
#include <cassert>

//Lot easier to call global functions than to call class functions
void test_postAfter_waits_before_running_function() {
    Workers event_loop(1);
    std::vector <std::string> orderOfCompletion;

    const string FINISH_FIRST = "Should finish first";
    const string FINISH_LAST = "Should finish be last"

    event_loop.start();
    event_loop.post_after([&orderOfCompletion] {
        orderOfCompletion.emplace_back(FINISH_LAST);
    }, [] {
        sleep(1);
    });

    event_loop.post([&orderOfCompletion] {
        orderOfCompletion.emplace_back(FINISH_FIRST);
    });

    event_loop.stop();
    event_loop.join();
    assert(orderOfCompletion[0] == FINISH_FIRST && orderOfCompletion[1] == FINISH_LAST &&
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

