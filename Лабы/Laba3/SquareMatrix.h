#pragma once
#include "Matrix.h"

template<typename T>
class SquareMatrix : public Matrix<T> 
{
public:
    explicit SquareMatrix(size_t n) : Matrix<T>(n, n) {}

    void swapRows(size_t i, size_t j)
    {
        if (i >= this->rows || j >= this->rows)
        {
            throw std::out_of_range("Row index out of range");
        }

        // Меняем местами строки через std::swap
        std::swap(this->data.get(i), this->data.get(j));
    }

    void addRows(size_t src, size_t dest, T coeff = 1)
    {
        if (src >= this->rows || dest >= this->rows)
        {
            throw std::out_of_range("Row index out of range");
        }
        for (size_t k = 0; k < this->cols; ++k) 
        {
            T new_val = this->data.get(dest).get(k) + coeff * this->data.get(src).get(k);
            this->data.get(dest).set(new_val, k);
        }
    }

    void multiplyRow(size_t row, T coeff)
    {
        if (row >= this->rows) 
        {
            throw std::out_of_range("Row index out of range");
        }
        for (size_t k = 0; k < this->cols; ++k) 
        {
            T new_val = this->data.get(row).get(k) * coeff;
            this->data.get(row).set(new_val, k);
        }
    }
};