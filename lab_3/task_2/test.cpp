#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <utility>

#include "funcs.hpp"


class test_error : public std::exception{
private:
    std::string error_message;

public:
    test_error(std::ostringstream& error_message_stream) noexcept{
        error_message = error_message_stream.str();
    }

    test_error(const test_error& other) noexcept{
        this->error_message = other.error_message;
    }

    ~test_error(){
        error_message.clear();
    }

    test_error& operator=(const test_error& other) noexcept{
        this->error_message = other.error_message;
        return *this;
    }

    const char* what() const noexcept override{
        return error_message.c_str();
    }
};


template <typename T>
std::pair<T, T> test_one_line(std::ifstream& log_file){
    std::string func_name;
    log_file >> func_name;

    std::size_t num_args; 
    log_file >> num_args;

    std::vector<T> args(num_args);
    for(int i = 0; i < num_args; i++){
        log_file >> args[i];
    }

    T logged_res;
    log_file >> logged_res;

    T real_res;
    if(func_name == "sqrt"){
        real_res = t_func::sqrt(args);
    }
    else if(func_name == "sin"){
        real_res = t_func::sin(args);
    }
    else if(func_name == "pow"){
        real_res = t_func::pow(args);
    }

    return {real_res, logged_res};
}


template <typename T>
bool eq(T x, T y, T eps){
    return std::fabs(std::fabs(x) - std::fabs(y)) <= eps;
}


template <typename T>
void test(std::ifstream& log_file){
    std::pair<T, T> res;
    std::size_t id;

    while(log_file >> id){
        res = test_one_line<T>(log_file);
        if(!eq<T>(res.first, res.second, 1e-3)){
            std::ostringstream error_message;
            error_message << "Error in task with id = " << id << ": ";
            error_message << res.first << " != " << res.second << "\n";
            throw test_error(error_message);
        }
    }

    if(log_file.bad()){
        throw std::ios_base::failure("I/O error while reading\n");
    }
}


int main(){
    int code = 0;
    std::ifstream log_file("logs.txt", std::ios::in);

    try{
        test<TYPE>(log_file);
        std::cout << "Completed without errors\n";
    }
    catch(const std::ios_base::failure& ex){
        std::cerr << ex.what();
        code = 1;
    } 
    catch(const test_error& ex){
        std::cerr << ex.what();
        code = 2;
    }
    
    log_file.close();
    return code;
}