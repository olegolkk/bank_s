/* 64-bit real matrix_r64 Class
 * ENV_matrix.hpp
 */

#pragma once

#ifndef ENV_MATRIX_HPP
#define ENV_MATRIX_HPP

#include "ENV_types.hpp"
//#include "ENV_HDF5.hpp"

namespace ENV
{
    namespace
    {
        V_STRING MATRIX_HDF_DATA_NAMES =
        {
        "Matrix",
        "Count of Rows",
        "Count of Columns",
        "Type",
        "Data"
        };
        //--------------------------------------------------------------------------------
        static V_STRING error_message{ "Matrix Error: ", "", "", "" };
    }
    class matrix_r64
    {
    public:
        INT32 i_error = ENV_0_I32;
        REAL64* sum_row = nullptr;
        INT64* i_number = nullptr;
        const STRING type_of_matrix = "R64";

        // print matrix
        void print(const STRING file_name);
        // return the number of rows
        inline INT64 rows() { return rows_; }
        inline INT64 length() { return rows_; }
        // return the number of columns
        inline INT64 cols() { return cols_; }
        inline INT64 length_x() { return cols_; }
        // swap two elements
        void swap(const INT64 i, const INT64 j);
        // rows reverse
        void reverse();
        // return rows reversed matrix
        matrix_r64 reversed();
        // return rows sorted matrix
        matrix_r64 sorted(const LOGICAL reversed = 0, const LOGICAL numbered = 1);

        // sorting with saving initial array numbers
        void sort(const LOGICAL reversed = 0, const LOGICAL numbered = 1);
        // probability function for matrix
        void probability_function();

        // get type of matrix
        const STRING get_type_of_matrix() { return type_of_matrix; }

        // new empty matrix
        matrix_r64() {}
        // new matrix with rows and cols
        matrix_r64(const INT64, const INT64 n_cols = 3, const REAL64 value = ENV_0_R64);
        // new matrix of rows and cols with values of matrix
        matrix_r64(REAL64**, const INT64 rows = ENV_0_I64, const INT64 n_cols = 3);
        // new zero matrix of one element
        matrix_r64(const REAL64* value, const INT64 n_cols = 3);
        // destructor
        ~matrix_r64();
        // no implicit destructor
        void clear();
        // is matrix empty
        LOGICAL empty() { return (rows_ < ENV_1_I64) || (cols_ < ENV_1_I64); }
        // get matrix element
        const REAL64 get_element(const INT64 i_row = ENV_0_I64, const INT64 i_col = ENV_0_I64)
        { return p_matrix[i_row][i_col]; }
        // set matrix element
        void set_element(const REAL64 value, const INT64 i_row = ENV_0_I64, const INT64 i_col = ENV_0_I64)
        { p_matrix[i_row][i_col] = value; }
        // get row (row=x) of matrix
        inline REAL64* operator[](const INT64 x)
        { return p_matrix[x]; }
        // get matrix row (row=i_row)
        REAL64* get_row(const INT64 i_row = ENV_0_I64)
        { return p_matrix[i_row]; }
        // get matrix col (col=i_col)
        matrix_r64 get_col(const INT64 i_col = ENV_0_I64);
        // set matrix row by value
        void set_row(const REAL64 value, const INT64 ir = ENV_0_I64);
        // set matrix row
        void set_row(const REAL64* value, const INT64 ir = ENV_0_I64);
        // set matrix col by value
        void set_col(const REAL64 value, const INT64 ic = ENV_0_I64);
        // set matrix col
        void set_col(const REAL64* value, const INT64 ic = ENV_0_I64);
        // copying of matrix
        matrix_r64(const matrix_r64&);
        // copying of matrix
        matrix_r64 operator=(const matrix_r64&);
        // copying of vector
        matrix_r64 operator=(const VECTOR<REAL64>&);
        // equalizing of matrix
        matrix_r64 operator=(const REAL64 value);

        // element(row=x, col=y) of matrix
        inline REAL64& operator()(const INT64 x, const INT64 y) { return p_matrix[x][y]; }
        // row=x of matrix
        inline REAL64* row(const INT64 x) { return p_matrix[x]; }

        // adding the matrix by matrix
        matrix_r64 operator+=(const matrix_r64&);
        // subtracting the matrix by matrix
        matrix_r64 operator-=(const matrix_r64&);
        // multiplying the matrix by matrix
        matrix_r64 operator*=(const matrix_r64&);
        // multiplying the matrix by number
        matrix_r64 operator*=(const REAL64);
        // dividing the matrix by number
        matrix_r64 operator/=(const REAL64);
        // exponenting of the matrix by number
        matrix_r64 operator^(const INT32);

        // swap two rows of matrix
        void swap_rows(const INT64, const INT64);
        // swap two cols of matrix
        void swap_cols(const INT64 c1, const INT64 c2);
        // transpose the matrix
        matrix_r64 transpose();

        // unknown exp()
        matrix_r64 exponent(const matrix_r64&, const INT32);
        // identity matrix
        matrix_r64 create_identity(const INT64);
        // solve the matrixed linear equation system
        matrix_r64 solve(matrix_r64, matrix_r64);
        // solve the matrixed linear equation system with band
        matrix_r64 band_solve(matrix_r64, matrix_r64, const INT64);

        // *** functions on vectors ***
        REAL64 dot_product(matrix_r64, matrix_r64);

        // *** functions on augmented matrices ***

        // augment the matrix
        matrix_r64 augment(matrix_r64, matrix_r64);
        // Guassian eliminate for the matrix
        matrix_r64 gaussian_eliminate();
        // reducing the matrix
        matrix_r64 rowReduceFromGaussian();
        // inverse the matrix
        matrix_r64 inverse();

        /*** HDF READ/WRITE FUNCTIONS ***
         ********************************/

#ifdef ENV_HDF5_HPP
#ifdef H5Cpp_H
        // HDF read martrix
        LOGICAL freadHDF(const H5::Group* parent_id, const STRING& group = STRING_EMPTY, const bool compressed = false);
        // HDF write martrix
        LOGICAL fwriteHDF(H5::Group* parent_id, const STRING& group = STRING_EMPTY, const bool compressed = false);
#else
        // HDF read martrix
        LOGICAL freadHDF(const hid_t parent_id, const STRING& group = STRING_EMPTY, const bool compressed = false);
        // HDF write martrix
        LOGICAL fwriteHDF(const hid_t parent_id, const STRING& group = STRING_EMPTY, const bool compressed = false);
#endif
#endif

    private:
        INT64 rows_ = ENV_0_I64, cols_ = ENV_0_I64, nx_ = ENV_1M_I64;
        REAL64** p_matrix = nullptr;

        // matrix data allocator
        void allocate();
        // quick sorting with saving initial array numbers
        void qsort(REAL64**, const INT64, INT64*);

    };

    // Binary Operations with Matrixes
    matrix_r64 operator+(const matrix_r64&, const matrix_r64&);
    matrix_r64 operator-(const matrix_r64&, const matrix_r64&);
    matrix_r64 operator*(const matrix_r64&, const matrix_r64&);
    matrix_r64 operator*(const matrix_r64&, const REAL64);
    matrix_r64 operator*(const REAL64, const matrix_r64&);
    matrix_r64 operator/(const matrix_r64&, const REAL64);

}

#endif // ENV_MATRIX_HPP
