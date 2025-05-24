#pragma once
#include "Vec.hpp"

template <typename T> class Matrix {
    private:
        std::vector<std::vector<T>> m; /* Internal matrix data */
        int rows;
        int cols;

    public:
        Matrix(int rows, int cols) : rows(rows), cols(cols) {}
        Matrix(std::vector<std::vector<T>> values) {
            rows = values.size();
            cols = values[0].size();

            /* Verify matrix is valid */
            for (int i=0; i<rows; ++i) {
                if (values[i].size() != cols) {
                    throw std::invalid_argument("Matrix rows are not all the same length.");
                }
            }
            m = values;
        }

        /* For accessing elements; used for non-const matrices */
        std::vector<T>& operator[] (int i) { return m[i]; }

        /* For read-only access; this is used when the matrix is declared as const */
        const std::vector<T>& operator[] (int i) const { return m[i]; }

        const Vec3 operator*(Vec3 vec) {
            if (cols != 3) {
                throw std::invalid_argument("Matrix must have 3 columns to multiply by a Vec3");
            }
            Vec3 result;
            result.x = m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z;
            result.y = m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z;
            result.z = m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z;

            return result;
        }

        const Vec4 operator*(Vec4 vec) {
            if (cols != 4) {
                throw std::invalid_argument("Matrix must have 4 columns to multiply by a Vec4");
            }
            Vec4 result;
            result.x = m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z + m[0][3] * vec.w;
            result.y = m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z + m[1][3] * vec.w;
            result.z = m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z + m[2][3] * vec.w;
            result.w = m[3][0] * vec.x + m[3][1] * vec.y + m[3][2] * vec.z + m[3][3] * vec.w;

            return result;
        }

        Matrix<T> operator*(int scalar) {
            Matrix<T> result(*this);
            for (int i=0; i<result.rows; ++i) {
                for (int j=0; j<result.cols; ++j) {
                    result[i][j] *= scalar;
                }
            }
            return result;
        }

        void print() {
            for (int i=0; i<rows; ++i) {
                for (int j=0; j<cols; ++j) {
                    std::cout << m[i][j] << " ";
                }
                std::cout << "\n";
            }
        }

        int getRows() const { return rows; }
        int getCols() const { return cols; }
};

template <typename T>
Matrix<T> operator*(int scalar, Matrix<T> matrix) {
    /* matrix not passed in by reference so we can just modify
    it and return it without affecting the original one passed in */
    for (int i=0; i<matrix.getRows(); ++i) {
        for (int j=0; j<matrix.getCols(); ++j) {
            matrix[i][j] *= scalar;
        }
    }
    return matrix;
}