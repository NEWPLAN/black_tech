#include "timeline.h"
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include <glog/logging.h>

void test_function()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

My_TensorTableEntry build(int index)
{
    My_TensorTableEntry entry;
    entry.tensor_name = "tensor_" + std::to_string(index);
    return entry;
}

int main(int argc, char *argv[])
{

    // Timeline writer.
    Timeline timeline;
    timeline.Initialize("abc.json", 1);
    std::vector<My_TensorTableEntry> records;

    for (int index = 0; index < 10; index++)
    {
        My_TensorTableEntry e = build(index);
        records.push_back(e);
        //const std::string name = e.tensor_name;
        timeline.NegotiateStart(e.tensor_name, READ_REMOTE);
        test_function();
        timeline.NegotiateEnd(e.tensor_name);
        timeline.Start(e.tensor_name, WRITE_REMOTE_DONE);
    }
    std::string start = "test start";
    std::string stop = "test stop";

    std::cout << "Negonatived done" << std::endl;

    std::vector<std::thread *> thread_group;
    for (int index = 2; index < 9; index++)
    {

        thread_group.push_back(new std::thread([&, index]() {
            std::string tensor_name = "tensor_" + std::to_string(index);
            LOG(INFO) << "In : " << tensor_name;
            timeline.End(tensor_name, &stop);
            test_function();
            test_function();
            timeline.ActivityStart(tensor_name, "WRITE_REMOTE_DONE");
            test_function();
            test_function();
            timeline.End(tensor_name, &stop);
            test_function();
            test_function();
            LOG(INFO) << "out : " << tensor_name;
        }));
    }

    for (auto thread : thread_group)
    {
        thread->join();
    }
    LOG(INFO) << "Master thread has jointed all threads";

    timeline.ActivityStartAll(records, start);
    std::cout << "having activated all" << std::endl;
    test_function();
    timeline.ActivityEndAll(records);
    std::cout << "having end all" << std::endl;
    timeline.Shutdown();
    return 0;
}