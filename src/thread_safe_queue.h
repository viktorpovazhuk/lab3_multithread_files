//
// Created by vityha on 26.03.22.
//

#include <mutex>
#include <deque>
#include <condition_variable>

#ifndef LAB3_MUTITHREAD_FILES_THREAD_SAFE_QUEUE_H
#define LAB3_MUTITHREAD_FILES_THREAD_SAFE_QUEUE_H

template <typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue() = default;
    ~ThreadSafeQueue() = default;
    ThreadSafeQueue(const ThreadSafeQueue&) = delete;
    ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

    // Add element. If max elements in queue, make thread wait.
    void enque(const T& el){
        // sets the scope for mutex locking
        {
            std::unique_lock<std::mutex> lock(mMutex);
            if (mDeque.size() >= maxNumElements) {
                mCondVarEnq.wait(lock);
            }
            mDeque.push_back(el);
        }
        mCondVarDeq.notify_one();
    }

    // Return element if exists. Otherwise, make thread wait.
    T deque(){
        T el;
        // sets the scope for mutex locking
        {
            std::unique_lock<std::mutex> lock(mMutex);
            while (mDeque.empty()) {
                mCondVarDeq.wait(lock);
            }
            el = mDeque.front();
            mDeque.pop_front();
        }
        mCondVarEnq.notify_one();
        // TODO: won't produce data race?
        return el;
    }

    // Return size. Not reliable as size can change in next moment.
    size_t get_size() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mDeque.size();
    }

private:
    std::deque<T> mDeque;
    mutable std::mutex mMutex;
    std::condition_variable mCondVarEnq;
    std::condition_variable mCondVarDeq;
    int maxNumElements = 100;
};

#endif //LAB3_MUTITHREAD_FILES_THREAD_SAFE_QUEUE_H