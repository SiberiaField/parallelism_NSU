#include "sin_vec.hpp"


int main(){
    Sin_Vec sv;
    float seconds;
    std::cout << "SUM " << sv.sum(&seconds) << "\n";
    std::cout << "TIME " << seconds << " sec\n";
    std::cout << "TYPE " << TYPE_NAME << "\n";
    return 0;
}