#include "../SPSCQueue.h"
#include <thread>
#include <mutex>
#include <iostream>

typedef uint32_t ElemType;
#define LOG_PER_SECOND 100000000

std::mutex out_mtx;

int main(int argc, char *argv[])
{

    NEWPLAN::SPSCQueue<ElemType> *spsc_q = new NEWPLAN::SPSCQueue<ElemType>(102400);

    //spsc_q
    std::thread *spq_t1 = new std::thread([spsc_q]() {
        size_t record = 0;
        ElemType data = 0;
        while (true)
        {
            spsc_q->push(data);
            data++;
            record++;
            data %= LOG_PER_SECOND;
            if (record > LOG_PER_SECOND)
            {
                {
                    std::lock_guard<std::mutex> lg(out_mtx);
                    std::cout << "[SPSQ]: Write Performances: "
                              << record / 10000000.0 << " M wps " << spsc_q->size() << std::endl;
                }
                record = 0;
            }
        }
    });
    std::thread *spq_t2 = new std::thread([spsc_q]() {
        size_t record = 0;
        ElemType data = 0;
        ElemType *recv_ptr = nullptr;
        ElemType new_data = 0;
        while (true)
        {
            // do
            // {
            //     recv_ptr = spsc_q->front();
            // } while (recv_ptr == nullptr);
            // spsc_q->pop();

            new_data = spsc_q->blocking_pop();

            record++;

            if (record > LOG_PER_SECOND)
            {
                {
                    std::lock_guard<std::mutex> lg(out_mtx);

                    std::cout << "[SPSQ]: Read Performances: " << record / 10000000.0
                              << " M rps " << spsc_q->size() << std::endl;
                }
                record = 0;
            }
        }
    });

    spq_t1->join();
    spq_t2->join();

    return 0;
}