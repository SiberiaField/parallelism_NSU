#ifndef __MATRICES__
#define __MATRICES__


#include <memory>
#include <random>
#include <iostream>

namespace matrices {
    class matrix{
        private:
            double* data;
            std::size_t lines;
            std::size_t columns;
            std::size_t size;
        public:
            matrix() noexcept;
            matrix(std::size_t lines, std::size_t columns);
            virtual ~matrix() noexcept;

            #pragma acc routine seq
            double& at(std::size_t i, std::size_t j);
            #pragma acc routine seq
            const double& at(std::size_t i, std::size_t j) const;
            void init(std::size_t lines, std::size_t columns);
            void fill(const double value) noexcept;
            void random_fill(const double p, const double low, const double high) noexcept;
            void reshape(std::size_t lines, std::size_t columns);
            void mul(const matrix &a, const matrix &b);
            double standart_deviation();
            matrix& operator-=(const matrix& b);
            matrix& operator*=(const double scalar);

            #pragma acc routine seq
            std::size_t get_lines() const noexcept;
            #pragma acc routine seq
            std::size_t get_columns() const noexcept;
            #pragma acc routine seq
            std::size_t get_size() const noexcept;

            void update_host();
            void update_device();
    };
};


std::ostream& operator<<(std::ostream& os, const matrices::matrix& mat);


#endif
