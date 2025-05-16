#include <iostream>
#include <unordered_map>

#include <boost/program_options.hpp>

#include "solve.hpp"


namespace opt = boost::program_options;

double eps, step;
unsigned iters, plate_shape;


void parse_args(int argc, char** argv){
    opt::options_description desc("Usage");
    desc.add_options()
        ("iters,i", opt::value<unsigned>(&iters)->default_value(100))
        ("eps,e", opt::value<double>(&eps)->default_value(1e-6))
        ("plate_shape,N", opt::value<unsigned>(&plate_shape)->default_value(10))
        ("step,s", opt::value<double>(&step)->default_value(1e-3))
        ("help,h", "produce help message")
    ;
    opt::variables_map vm;
    opt::store(opt::parse_command_line(argc, argv, desc), vm);
    opt::notify(vm);
}


int main(int argc, char** argv){
    parse_args(argc, argv);
    if(eps < 1e-6 || iters > (unsigned)1e+6){
        std::cerr << "\nParameters out of range. They must be: eps >= 1e-6, iters <= 1e+6\n";
        return 1;
    }

    matrices::matrix a(plate_shape * plate_shape, plate_shape * plate_shape);
    std::cout << "Create a\n";
    init_solving_matrix(a, plate_shape);
    std::cout << "Init a\n";
    a.update_device();
    std::cout << "Update a on device\n\n";

    matrices::matrix x(plate_shape, plate_shape);
    std::cout << "Create x\n";
    init_solving_vector(x, plate_shape);
    std::cout << "Init x\n";
    x.update_device();
    std::cout << "Update x on device\n\n";

    matrices::matrix b(plate_shape * plate_shape, 1);
    std::cout << "Create b\n";
    b.random_fill(0.3, -50, 50);
    std::cout << "Init b\n";
    b.update_device();
    std::cout << "Update b on device\n\n";

    std::unordered_map<char, double> results;
    std::cout << "Call solve function\n";
    results = solve(a, b, x, step, eps, iters);
    std::cout << "\n\nTime: " << results.at('t') << " Iters: " << results.at('i') << " Error: " << results.at('e') << '\n';

    x.update_host();
    x.reshape(plate_shape, plate_shape);
    std::cout << "\nResult:\n" << x;

    return 0;
}
