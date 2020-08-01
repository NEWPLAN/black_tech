#include <iostream>
#include <sstream>
#include "blocking_queue.h"


void blocking_queue_test()
{
    using namespace std::chrono;
    newplan::BlockingQueue<int> *bq = new newplan::BlockingQueue<int>();
    std::thread *producer_1 = new std::thread([&]() {
        for (int index = 1; index < 100; index += 2)
        {
            bq->push(index);
            std::this_thread::sleep_for(1ms);
        }
    });
    std::thread *producer_2 = new std::thread([&]() {
        for (int index = 2; index < 100; index += 2)
        {
            bq->push(index);
            std::this_thread::sleep_for(1ms);
        }
    });
    std::thread *consumer = new std::thread([&]() { 
        while(true){
            std::cout << "from consumer: " << bq->pop() << std::endl;
            } });

    producer_1->join();
    producer_2->join();
    consumer->join();
}

int main(int argc, char **argv)
{
    blocking_queue_test();
    return 0;
}
