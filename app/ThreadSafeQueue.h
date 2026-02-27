#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue
{
public:
    void push(const T& value)
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(value);
        cond.notify_one();
    }

    void close()
    {
        std::lock_guard<std::mutex> lock(mutex);
        closed = true;
        cond.notify_all();
    }

    bool pop(T& value)
    {
        std::unique_lock<std::mutex> lock(mutex);
        cond.wait(lock, [this] { return !queue.empty() || closed; });

        if (closed && queue.empty())
            return false;

        value = queue.front();
        queue.pop();
        return true;
    }

private:
    std::queue<T> queue;
    std::mutex mutex;
    std::condition_variable cond;
    bool closed = false;
};

#endif
