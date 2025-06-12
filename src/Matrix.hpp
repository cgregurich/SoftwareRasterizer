#pragma once
#include "Vec.hpp"
#include <vector>

template <typename T> class Matrix {
    private:
        std::vector<std::vector<T>> m; /* Internal matrix data */
        int rows;
        int cols;

    public:
        Matrix() = default;
        Matrix(int rows, int cols) : rows(rows), cols(cols) {
            /* Make space for data based on given rows and cols
            and default-initialize data */
            m.resize(rows);
            for (int r=0; r<rows; ++r) {
                m[r].resize(cols);
            }
        }
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

        bool operator==(const Matrix<T>& rightMatrix) {
            return this->m == rightMatrix.m;
        }

        bool operator!=(const Matrix<T>& rightMatrix) {
            return !(*(this) == rightMatrix);
        }

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

        Vec4 operator*(Vec4 vec) const {
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

        Matrix<T> operator*(Matrix<T> other) {

            /*
            A light, incomplete explanation...
            Example: 2x3 * 3x2 matrix

            A: 2x3 matrix (2 rows 3 cols)
            {
              { 2  3  4 }
              { 5  6  7 }
            }

            B: 3x2 matrix (3 rows, 2 cols)
            {
                { 1  2 }
                { 3  4 }
                { 5  6 }
            }

            Result:
            {
                { ?  ? }
                { ?  ? }
            }
            Matrix multiplication is only valid if A.cols == B.rows
            The result has rows = A.rows and cols = B.cols
            */

            if (this->cols != other.rows) {
                throw std::invalid_argument("Matrix dimensions are not compatible for multiplication");
            }

            Matrix<T> result(this->rows, other.cols);

            for (int rowA=0; rowA<this->rows; ++rowA) {
                for (int colB=0; colB<other.cols; ++colB) {
                    T sum = 0;
                    for (int i=0; i<this->cols; ++i) {
                        sum += m[rowA][i] * other[i][colB];
                    }
                    result[rowA][colB] = sum;
                }
            }
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
            std::cout << std::endl;
        }

        int getRows() const { return rows; }
        int getCols() const { return cols; }

    /* todo: probably want to return a reference to the matrix to allow chaining
    eg. matrix.applyTranslation().applyScale() */
    void applyTranslation (double tX, double tY, double tZ) {
        /* Check matrix size */
        if (this->rows != 4 || this->cols != 4) {
            throw std::invalid_argument("Matrix must be 4x4");
        }

        m[0][3] = tX;
        m[1][3] = tY;
        m[2][3] = tZ;

        // return matrix;
    }

    // void applyScale(double sX, double sY, double sZ) {
    //     /* Check matrix size */
    //     if (this->rows != 4 || this->cols != 4) {
    //         throw std::invalid_argument("Matrix must be 4x4");
    //     }
    //     Matrix<T> scaleMatrix({
    //         {sX, 0, 0, 0},
    //         {0, sY, 0, 0},
    //         {0, 0, sZ, 0},
    //         {0, 0, 0,  1}
    //     });
        

    // }
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

// template <typename T>
// Matrix<T> operator*(Matrix<T> matrix1, Matrix<T> matrix2) {
//     return 
// }