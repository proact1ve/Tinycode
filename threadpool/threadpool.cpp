#include "threadpool.h"
#include <iostream>
#include <assert.h>

namespace my_threadpool
{
    /**
     * @brief Construct a new Thread Pool:: Thread Pool object
     *
     * @param threadsNum
     */
    ThreadPool::ThreadPool(int threadsNum)
    {
        threadsNum_ = threadsNum;
        isRunning_ = true;
        createThreads();
        pthread_mutex_init(&mutex_, NULL);
        pthread_cond_init(&cond_, NULL);
    }
    /**
     * @brief Destroy the Thread Pool:: Thread Pool object
     *
     */
    ThreadPool::~ThreadPool()
    {
        stop();

        pthread_mutex_destroy(&mutex_);
        pthread_cond_destroy(&cond_);

        for (auto it = taskQueue_.begin(); it != taskQueue_.end(); it++)
        {
            delete *it;
        }
    }
    /**
     * @brief 创建线程
     *
     * @return int
     */
    int ThreadPool::createThreads()
    {
        threads_ = (pthread_t *)malloc(sizeof(pthread_t) * threadsNum_);

        for (int i = 0; i < threadsNum_; i++)
        {
            pthread_create(&threads_[i], NULL, threadFunc, this);
        }
        return 0;
    }
    /**
     * @brief 获取队列task的大小
     *
     * @return int
     */
    int ThreadPool::size()
    {
        pthread_mutex_lock(&mutex_);
        int size = taskQueue_.size();
        pthread_mutex_unlock(&mutex_);
        return size;
    }
    /**
     * @brief 向任务队列中添加任务
     *
     * @param task 任务指针
     * @return size_t 队列中任务个数
     */
    size_t ThreadPool::addTask(Task *task)
    {
        pthread_mutex_lock(&mutex_);

        taskQueue_.push_back(task);
        size_t size = taskQueue_.size();

        pthread_mutex_unlock(&mutex_);
        pthread_cond_signal(&cond_);
        return size;
    }
    /**
     * @brief 从任务队列取出任务
     *
     * @return Task* 任务指针
     */
    Task *ThreadPool::take()
    {
        Task *task = NULL;
        while (!task)
        {
            pthread_mutex_lock(&mutex_);
            while (taskQueue_.empty() && isRunning_)
            {
                pthread_cond_wait(&cond_, &mutex_);
            }

            if (!isRunning_)
            {
                pthread_mutex_unlock(&mutex_);
                break;
            }
            else if (taskQueue_.empty())
            {
                pthread_mutex_unlock(&mutex_);
                continue;
            }

            if (taskQueue_.empty())
            {
                perror("taskquque is empty");
            }

            task = taskQueue_.front();
            taskQueue_.pop_front();
            pthread_mutex_unlock(&mutex_);
        }

        return task;
    }
    /**
     * @brief 停止线程
     *
     */
    void ThreadPool::stop()
    {
        if (!isRunning_)
            return;
        isRunning_ = false;

        pthread_cond_broadcast(&cond_);

        for (int i = 0; i < threadsNum_; i++)
        {
            pthread_join(threads_[i], NULL);
        }

        free(threads_);
        threads_ = NULL;
    }

    void *ThreadPool::threadFunc(void *arg)
    {
        ThreadPool *pool = static_cast<ThreadPool *>(arg);
        while (pool->isRunning_)
        {
            // 从队列中取出
            Task *task = pool->take();
            if (!task)
            {
                std::cout << "Thread is = " << pthread_self() << " will exit\n"
                          << std::endl;
                break;
            }
            assert(task);
            task->run();
        }
        return 0;
    }
}