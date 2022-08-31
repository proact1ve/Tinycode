#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <stdlib.h>
#include <deque>
#include <string>

namespace my_threadpool
{
    class Task
    {
    public:
        Task(void *arg = NULL, const std::string taskName = "")
            : arg_(arg), taskName_(taskName) {}

        virtual ~Task() {}

        void setArg(void *arg)
        {
            arg_ = arg;
        }

        virtual int run() = 0;

    protected:
        void *arg_;
        std::string taskName_;
    };

    class ThreadPool
    {
    public:
        ThreadPool(int threadNum = 8);
        ~ThreadPool();

    public:
        size_t addTask(Task *task);
        int size();
        void stop();
        Task *take();

    private:
        int createThreads();
        static void *threadFunc(void *arg);

    private:
        ThreadPool &operator=(const ThreadPool &) = delete;
        ThreadPool(const ThreadPool &) = delete;

    private:
        volatile bool isRunning_;      // 设置成volatile：异常原因变动
        int threadsNum_;               // 线程的数量
        pthread_t *threads_;           // 线程数组
        std::deque<Task *> taskQueue_; // 任务队列
        pthread_mutex_t mutex_;        // 锁变量
        pthread_cond_t cond_;          // 条件变量
    };
}

#endif