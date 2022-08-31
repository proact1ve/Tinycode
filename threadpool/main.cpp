#include "threadpool.h"
#include <iostream>
#include <unistd.h>

using namespace my_threadpool;

class MyTask : public Task
{
public:
    MyTask() {}

    virtual int run()
    {
        while (1)
        {
            std::cout << "I am thread, My id is " << pthread_self() << "have a good day" << std::endl;
            usleep(1000);
        }
        return 0;
    }
};

int main()
{
    char *msg = "hello, world";
    MyTask taskObj;
    ThreadPool threadpool;
    taskObj.setArg((void *)msg);

    for (int i = 0; i < 30; i++)
    {
        threadpool.addTask(&taskObj);
    }

    while (1)
    {
        std::cout << "Now there are " << threadpool.size() << " are running" << std::endl;
        if (threadpool.size() == 0)
        {
            threadpool.stop();
            printf("Main thread will exit...\n");
            exit(0);
        }
        sleep(2);
    }
    return 0;
}