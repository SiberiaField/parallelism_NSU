#ifndef __SERVER__
#define __SERVER__


#include <thread>
#include <future>
#include <condition_variable>
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <unordered_map>
#include <functional>


template <typename T> class server{
private:
    std::queue<std::pair<size_t, std::future<T>>> tasks;
    std::unordered_map<size_t, T> results;
    size_t global_task_id;
    std::mutex mut;
    std::condition_variable cv;
    bool stop_server;

public:
    server(){
        global_task_id = 0;
        stop_server = false;
    }


    ~server(){
        results.clear();
    }


    void start(){
        std::unique_lock<std::mutex> lock(mut, std::defer_lock);

        while(!stop_server){
            lock.lock();
            cv.wait(lock, [this] { return !tasks.empty() || stop_server; });
            if(!tasks.empty()){
                results.insert({tasks.front().first, tasks.front().second.get()});
                tasks.pop();
            }
            lock.unlock();
        }
    }


    void stop(){
        stop_server = true;
        cv.notify_all();
    }


    size_t add_task(T (*task)(std::vector<T>&), std::vector<T>& task_args){
        std::future<T> res = std::async(std::launch::async, std::bind(task, task_args));
        size_t curr_task_id;
        
        {
            std::lock_guard<std::mutex> lock(mut);
            curr_task_id = ++global_task_id;
            tasks.push({curr_task_id, std::move(res)});
        }
        cv.notify_one();

        return curr_task_id;
    }


    T request_result(int res_id){
        std::unique_lock<std::mutex> lock(mut, std::defer_lock);
        bool result_ready = false;
        T res;

        while(!result_ready){
            lock.lock();
            if(results.find(res_id) != results.end()){
                res = results[res_id];
                results.erase(res_id);
                result_ready = true;
            }
            lock.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        return res;
    }
};


#endif