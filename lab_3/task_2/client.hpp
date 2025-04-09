#ifndef __CLIENT__
#define __CLIENT__


#include <random>
#include <vector>
#include <iomanip>
#include "server.hpp"


template <typename T> class client{
private:
    static std::ofstream log_file;
    static std::mutex file_mut;
    T (*task)(std::vector<T>&);
    std::string name;
    server<T>* serv;
    size_t num_calls;
    size_t num_args;

public:
    client(T (*task)(std::vector<T>&), size_t num_args, std::string&& name, server<T>* serv, size_t num_calls){
        this->task = task;
        this->num_args = num_args;
        this->name = name;
        this->serv = serv;
        this->num_calls = num_calls;
    }

    ~client(){
        name.clear();
    }

    void start(){
        std::unique_lock<std::mutex> file_lock(file_mut, std::defer_lock);
        size_t task_id;
        T res;
        std::vector<T> args(num_args);
        std::uniform_real_distribution<T> unif(1, 30);
        std::default_random_engine re;

        while(num_calls != 0){
            for(int i = 0; i < num_args; i++){
                args[i] = unif(re);
            }
            task_id = serv->add_task(task, args);
            res = serv->request_result(task_id);

            file_lock.lock();
            log_file << task_id << " " << name << " " << num_args << " ";
            for(const auto& arg : args){
                log_file << arg << " ";
            }
            log_file << res << "\n";
            file_lock.unlock();
            
            num_calls--;
        }

        args.clear();
    }


    static void open_log_file();
    static void close_log_file();
};


template<class T> std::ofstream client<T>::log_file;
template<class T> std::mutex client<T>::file_mut;


template<class T> inline void client<T>::open_log_file(){
    log_file.open("logs.txt", std::ios::out);
    log_file << std::hexfloat;
}


template<class T> inline void client<T>::close_log_file(){
    log_file.close();
}


#endif