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
        ("eps,e", opt::value<double>(&eps)->default_value(1e-4))
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

    std::size_t solving_matrix_shape = plate_shape * plate_shape;

    // Init temperature vector B and copy to GPU
    matrices::matrix b;
    b.lines = solving_matrix_shape;
    b.columns = 1;
    b.size = b.lines;
    b.data = new double[b.size];
    matrices::random_fill(b, 0.3, -50, 50);
    #pragma acc enter data copyin(b)
    #pragma acc enter data copyin(b.data[0:b.size])
    std::cout << "Init b\n";

    // Count deviation of matrix B one time to GPU
    double b_deviation = 0;
    #pragma acc enter data copyin(b_deviation)

    std::cout << "Start computing b_deviation...\n";
    #pragma acc data present(b, b.data[0:b.size], b_deviation) 
    {
        #pragma acc parallel loop gang reduction(+:b_deviation)
        for(std::size_t i = 0; i < b.lines; i++){
            #pragma acc loop vector reduction(+:b_deviation)
            for(std::size_t j = 0; j < b.columns; j++){
                b_deviation += b.data[i * b.columns + j] * b.data[i * b.columns + j];
            }
        }
    }

    // Update value on host and remove from GPU
    #pragma acc update self(b_deviation)
    #pragma acc exit data delete(b_deviation)
    b_deviation = sqrtf64(b_deviation);
    std::cout << "End computing\n";

    // Init solving vector X and copy it to GPU
    matrices::matrix x;
    x.columns = x.lines = plate_shape;
    x.size = x.columns * x.lines;
    x.data = new double[x.size];
    init_solving_vector(x, plate_shape);
    #pragma acc enter data copyin(x)
    #pragma acc enter data copyin(x.data[0:x.size])
    std::cout << "Init x\n";

    // Create helping matrix Solve_tmp and copy it to GPU
    matrices::matrix solve_tmp;
    solve_tmp.lines = solving_matrix_shape;
    solve_tmp.columns = 1;
    solve_tmp.size = solve_tmp.lines;
    solve_tmp.data = new double[solve_tmp.size];
    #pragma acc enter data copyin(solve_tmp)
    #pragma acc enter data create(solve_tmp.data[0:solve_tmp.size])
    std::cout << "Create solve_tmp\n";

    // Create block of solving matrix and copy it to GPU
    matrices::matrix block;
    block.columns = solving_matrix_shape;
    block.lines = (16 * 1024 * 1024 * 8) / (solving_matrix_shape * 8);
    if (block.lines > solving_matrix_shape){
        block.lines = solving_matrix_shape;
    }
    block.size = block.lines * block.columns;
    block.data = new double[block.size];
    #pragma acc enter data copyin(block)
    #pragma acc enter data create(block.data[0:block.size])
    std::cout << "Create block\n";

    // Init some needed vars and copy some of them to GPU
    double convergence;
    long long proceeded_iters, global_line_idx;
    double tmp, solve_tmp_deviation = 0, total_updating_time = 0;
    #pragma acc enter data create(tmp, global_line_idx) copyin(solve_tmp_deviation, step)


    std::cout << "\nStart solving\n";
    const auto start_solving{std::chrono::steady_clock::now()};
    for(proceeded_iters = 0; proceeded_iters < iters; proceeded_iters++){
        // Compute mat_mul of A and B on GPU. Result is saved in Solve_tmp
        for (global_line_idx = 0; global_line_idx < solving_matrix_shape; global_line_idx++){
            #pragma acc update device(global_line_idx)

            // Update block if necessary
            if (((global_line_idx + 1) % block.lines) == 1){
                const auto start_updating{std::chrono::steady_clock::now()};
                for (long long block_line_idx = 0; block_line_idx < block.lines; block_line_idx++){
                    if ((global_line_idx + block_line_idx) >= solving_matrix_shape){
                        break;
                    }

                    long long column = global_line_idx + block_line_idx;
                    block.data[block_line_idx * block.columns + column] = -4;

                    // Fill elements on the left and on the right from diagonal element
                    if ((global_line_idx + block_line_idx + 1) % plate_shape == 1){
                        if (global_line_idx != 0){
                            block.data[block_line_idx * block.columns + (column - 1)] = 0;
                        }
                        block.data[block_line_idx * block.columns + (column + 1)] = 1;
                    }
                    else if ((global_line_idx + block_line_idx + 1) % plate_shape == 0){
                        if (global_line_idx != solving_matrix_shape - 1){
                            block.data[block_line_idx * block.columns + (column + 1)] = 0;
                        }
                        block.data[block_line_idx * block.columns + (column - 1)] = 1;
                    }
                    else{
                        block.data[block_line_idx * block.columns + (column + 1)] = block.data[block_line_idx * block.columns + (column - 1)] = 1;
                    }
                     
                    // Fill zeros on the right of diagonal elements |..., 1, -4, 1, 0, 0, 0...|
                    for (long long k = column + 2, zeros = 0; (zeros < plate_shape - 2) && (k < block.columns); k++, zeros++){
                        block.data[block_line_idx * block.columns + k] = 0;
                    }
                    // Put one after these zeros if there is a free space |..., 1, -4, 1, 0, 0, 0, 1, ...|
                    if (column + plate_shape < block.columns){
                        block.data[block_line_idx * block.columns + (column + plate_shape)] = 1;
                    }
                    // Fill rest of line with zeros |..., 1, -4, 1, 0, 0, 0, 1, 0, 0, 0|
                    for (long long k = column + plate_shape + 1; k < block.columns; k++){
                        block.data[block_line_idx * block.columns + k] = 0;
                    }

                    // Fill zeros on the left of diagonal elements |..., 0, 0, 0, 1, -4, 1, ...|
                    for (long long k = column - 2, zeros = 0; (zeros < plate_shape - 2) && (k > -1); k--){
                        block.data[block_line_idx * block.columns + k] = 0;
                    }
                    // Put one after these zeros if there is a free space |..., 1, 0, 0, 0, 1, -4, 1, ...|
                    if (column - plate_shape > -1){
                        block.data[block_line_idx * block.columns + (column - plate_shape)] = 1;
                    }
                    // Fill rest of line with zeros |0, 0, 0, 1, 0, 0, 0, 1, -4, 1, ...|
                    for (long long k = column - plate_shape - 1; k > -1; k--){
                        block.data[block_line_idx * block.columns + k] = 0;
                    }
                }

                #pragma acc update device(block)
                #pragma acc update device(block.data[0:block.size])
                const auto end_updating{std::chrono::steady_clock::now()};
                const std::chrono::duration<double> elapsed_seconds{end_updating - start_updating};
                total_updating_time += elapsed_seconds.count();
            }

            #pragma acc data present(tmp, block, block.data[0:block.size], x, x.data[0:x.size], solve_tmp, solve_tmp.data[0:solve_tmp.size], global_line_idx)
            {
                #pragma acc parallel loop gang private(tmp)
                for(std::size_t j = 0; j < x.columns; j++){
                    tmp = 0;
                    #pragma acc loop vector reduction(+:tmp)
                    for(std::size_t k = 0; k < block.columns; k++){
                        tmp += block.data[(global_line_idx % block.lines) * block.columns + k] * x.data[k * x.columns + j];
                    }
                    solve_tmp.data[global_line_idx * solve_tmp.columns + j] = tmp;
                }
            }
        }


        // Compute B - Solve_tmp on GPU
        #pragma acc data present(solve_tmp, solve_tmp.data[0:solve_tmp.size], b, b.data[0:b.size])
        {
            #pragma acc parallel loop gang
            for(std::size_t i = 0; i < solve_tmp.lines; i++){
                #pragma acc loop vector
                for(std::size_t j = 0; j < solve_tmp.columns; j++){
                    solve_tmp.data[i * solve_tmp.columns + j] -= b.data[i * b.columns + j];
                }
            }
        }
        

        // Compute deviation of Solve_tmp on GPU and update it on host
        #pragma acc data present(solve_tmp_deviation, solve_tmp, solve_tmp.data[0:solve_tmp.size])
        {
            #pragma acc parallel
            {
                solve_tmp_deviation = 0;
            }

            #pragma acc parallel loop gang reduction(+:solve_tmp_deviation)
            for(std::size_t i = 0; i < solve_tmp.lines; i++){
                #pragma acc loop vector reduction(+:solve_tmp_deviation)
                for(std::size_t j = 0; j < solve_tmp.columns; j++){
                    solve_tmp_deviation += solve_tmp.data[i * solve_tmp.columns + j] * solve_tmp.data[i * solve_tmp.columns + j];
                }
            }

            #pragma acc update self(solve_tmp_deviation)
        }
        

        // Compute error on host
        convergence = solve_tmp_deviation / b_deviation;
        if(convergence < eps){
            break;
        }


        // Compute Solve_tmp *= step on GPU
        #pragma acc data present(solve_tmp, solve_tmp.data[0:solve_tmp.size], step)
        {
            #pragma acc parallel loop gang
            for(std::size_t i = 0; i < solve_tmp.lines; i++){
                #pragma acc loop vector
                for(std::size_t j = 0; j < solve_tmp.columns; j++){
                    solve_tmp.data[i * solve_tmp.columns + j] *= step;
                }
            }
        }
        

        // Compute X - Solve_tmp on GPU
        #pragma acc data present(x, x.data[0:x.size], solve_tmp, solve_tmp.data[0:solve_tmp.size])
        {
            #pragma acc parallel loop gang
            for(std::size_t i = 0; i < x.lines; i++){
                #pragma acc loop vector
                for(std::size_t j = 0; j < x.columns; j++){
                    x.data[i * x.columns + j] -= solve_tmp.data[i * solve_tmp.columns + j];
                }
            }
        }
            

        // Print amount of iters on host
        if(proceeded_iters % 10 == 0){
            std::cout << "Iters: " << proceeded_iters << "\n";
        }
    }
    const auto end_solving{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsed_seconds{end_solving - start_solving};


    // Print metrics
    std::cout << "\n\nTime: " << elapsed_seconds.count() - total_updating_time << " Iters: " << proceeded_iters << " Error: " << convergence << '\n';

    // Update x on host and print it
    #pragma acc update self(x.data[0:x.size])

    // Clean GPU and host memory
    #pragma acc exit data delete(block.data[0:block.size], x.data[0:x.size], b.data[0:b.size], solve_tmp.data[0:solve_tmp.size])
    #pragma acc exit data delete(block, x, b, solve_tmp)
    #pragma acc exit data delete(tmp, solve_tmp_deviation, step, global_line_idx)

    delete block.data;
    delete x.data;
    delete b.data;
    delete solve_tmp.data;

    return 0;
}
