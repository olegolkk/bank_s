/*
 * ENV_matrix.cpp
 */

#include "ENV_matrix_r64.hpp"

#define EPS 1e-10

#define EPS_CHECK(a) ((a < EPS) && (a > -EPS))


namespace ENV
{

    /* PUBLIC MEMBER FUNCTIONS
     ********************************/

     // print matrix
    void matrix_r64::print(const STRING file_name)
    {
        std::ofstream oFile(file_name);
        // calculated matrix output to file file_name
		oFile << rows_ << " " << cols_ << END_LINE;
        for (int j = 0; j < rows_; j++)
        {
            const REAL64* p_row = p_matrix[j];
            STRING s_line = STRING_EMPTY;
            for (int i = 0; i < cols_; i++)
                s_line += " " + ENV::stringify(p_row[i]);
            oFile << s_line << END_LINE;
        }
        oFile.close();
    }

     // swap two elements
    void matrix_r64::swap(const INT64 i, const INT64 j)
    {
        std::swap(p_matrix[i], p_matrix[j]);
        if (i_number != nullptr) std::swap(i_number[i], i_number[j]);
    }

    // rows reverse
    void matrix_r64::reverse()
    {
        if (i_number = nullptr)
        {
            i_number = new INT64[rows_];
            for (auto i = ENV_0_I64; (i < rows_); ++i) i_number[i] = i;
        }
        auto r2 = rows_ / ENV_2_I64;
        for (auto i = ENV_0_I64; (i < r2); ++i) swap(i, rows_ - i - 1);
    }

    // return rows reversed matrix
    matrix_r64 matrix_r64::reversed()
    {
        auto rm = *this;
        rm.reverse();
        return rm;
    }

    // sorting with saving initial array numbers
    void matrix_r64::sort(const LOGICAL reversed, const LOGICAL numbered)
    {
        if (i_number = nullptr)
        {
            i_number = new INT64[rows_];
            for (auto i = ENV_0_I64; (i < rows_); ++i) i_number[i] = i;
        }
        qsort(p_matrix, rows_, i_number);
        if (reversed) reverse();
        if (numbered) return;
        PTR_DELETE(i_number, []);
    }

    // return rows sorted matrix
    matrix_r64 matrix_r64::sorted(const LOGICAL reversed, const LOGICAL numbered)
    {
        auto rm = *this;
        rm.sort(reversed, numbered);
        return rm;
    }

    // probability function for matrix
    void matrix_r64::probability_function()
    {
        if (empty()) return;
        if (sum_row == nullptr) sum_row = new REAL64[rows_]{};
        sum_row[0] = p_matrix[0][nx_];
        for (auto i = ENV_1_I64; (i < rows_); ++i)
            sum_row[i] = sum_row[i - 1] + p_matrix[i][nx_];
        auto srow = sum_row[rows_ - 1];
        if (srow > ENV_0_R64)
        {
            for (auto i = ENV_0_I64; (i < rows_ - 1); ++i) sum_row[i] /= srow;
        }
        sum_row[rows_ - 1] = ENV_1_R64;
    }

    // new zero matrix of rows and cols
    matrix_r64::matrix_r64(const INT64 rows, const INT64 cols, const REAL64 value)
        : rows_(rows), cols_(cols), nx_(cols - 1)
    {
        if (empty()) return;
        allocate();
        for (INT64 i = 0; (i < rows_); ++i) std::fill_n(p_matrix[i], cols_, value);
        probability_function();
    }

    // new matrix of rows and cols with values of matrix a
    matrix_r64::matrix_r64(REAL64** a, const INT64 rows, const INT64 cols)
        : rows_(rows), cols_(cols), nx_(cols - 1)
    {
        allocate();
        if (empty()) return;
        for (INT64 i = 0; (i < rows_); ++i)
            for (INT64 j = 0; (j < cols_); ++j) p_matrix[i][j] = a[i][j];
        probability_function();
    }

    // new zero matrix of one element
    matrix_r64::matrix_r64(const REAL64* value, const INT64 n_cols)
        : rows_(ENV_1_I64), cols_(n_cols), nx_(n_cols - 1)
    {
        allocate();
        if (empty()) return;
        p_matrix[0][nx_] = ENV_1_I64;
        for (auto i = ENV_0_I64; (i < nx_); ++i) p_matrix[0][i] = value[i];
        probability_function();
    }

    // destructor
    matrix_r64::~matrix_r64()
    {
        clear();
    }

    // no implicit destructor
    void matrix_r64::clear()
    {
        if (!empty())
        {
            for (INT64 i = 0; i < rows_; ++i)
            {
                PTR_DELETE(p_matrix[i], []);
            }
            PTR_DELETE(p_matrix, []);
            PTR_DELETE(sum_row, []);
            PTR_DELETE(i_number, []);
        }
        rows_ = ENV_0_I64;
        cols_ = ENV_0_I64;
        nx_ = cols_ - 1;
        i_error = ENV_0_I32;
    }

    // get matrix col (col=i_col)
    matrix_r64 matrix_r64::get_col(const INT64 i_col)
    {
        matrix_r64 x(1, rows_);
        for (INT64 i = 0; (i < rows_); ++i)
            x[0][i] = p_matrix[i][i_col];
        return x;
    }

    // set matrix row by value
    void matrix_r64::set_row(const REAL64* value, const INT64 ir)
    {
        std::memcpy(p_matrix[ir], value, cols_ * sizeof(REAL64));
    }

    // set matrix row
    void matrix_r64::set_row(const REAL64 value, const INT64 ir)
    {
        std::fill_n(p_matrix[ir], cols_, value);
    }

    // set matrix col by value
    void matrix_r64::set_col(const REAL64 value, const INT64 ic)
    {
        for (auto i = ENV_0_I64; (i < rows_); ++i)
            p_matrix[i][ic] = value;
    }

    // set matrix col
    void matrix_r64::set_col(const REAL64* value, const INT64 ic)
    {
        for (auto i = ENV_0_I64; (i < rows_); ++i)
            p_matrix[i][ic] = value[i];
    }

    // copying of matrix m
    matrix_r64::matrix_r64(const matrix_r64& m)
    {
        clear();
        rows_ = m.rows_;
        cols_ = m.cols_;
        nx_ = cols_ - 1;
        if (empty()) return;
        allocate();
        for (INT64 i = 0; (i < rows_); ++i)
            std::memcpy(p_matrix[i], m.p_matrix[i], cols_ * sizeof(REAL64));
        if (m.sum_row != nullptr)
        {
            sum_row = new REAL64[cols_]{};
            std::memcpy(sum_row, m.sum_row, cols_ * sizeof(REAL64));
        }
        if (m.i_number != nullptr)
        {
            i_number = new INT64[cols_]{};
            std::memcpy(i_number, m.i_number, cols_ * sizeof(INT64));
        }
    }

    // copying of matrix
    matrix_r64 matrix_r64::operator=(const matrix_r64& m)
    {
        if (this == &m) return *this;
        if (rows_ != m.rows_ || cols_ != m.cols_)
        {
            clear();
            rows_ = m.rows_;
            cols_ = m.cols_;
            nx_ = cols_ - 1;
            allocate();
        }
        for (INT64 i = 0; (i < rows_); ++i)
            std::memcpy(p_matrix[i], m.p_matrix[i], cols_ * sizeof(REAL64));
        return *this;
    }

    // copying of vector
    matrix_r64 matrix_r64::operator=(const VECTOR<REAL64>& v)
    {
        if (rows_ != 1 || cols_ != v.size())
        {
            clear();
            rows_ = 1;
            cols_ = v.size();
            nx_ = cols_ - 1;
            allocate();
        }
        for (INT64 i = 0; (i < rows_); ++i)
            for (INT64 j = 0; (j < cols_); ++j)
                p_matrix[i][j] = v[i];
        return *this;
    }

    // equalizing of matrix
    matrix_r64 matrix_r64::operator=(const REAL64 value)
    {
        for (INT64 i = 0; (i < rows_); ++i)
            std::fill_n(p_matrix[i], cols_, value);
        return *this;
    }

    // adding the matrix by matrix
    matrix_r64 matrix_r64::operator+=(const matrix_r64& m)
    {
        for (INT64 i = 0; (i < rows_); ++i)
            for (INT64 j = 0; (j < cols_); ++j)
                p_matrix[i][j] += m.p_matrix[i][j];
        return *this;
    }

    // subtracting the matrix by matrix
    matrix_r64 matrix_r64::operator-=(const matrix_r64& m)
    {
        for (INT64 i = 0; (i < rows_); ++i)
            for (INT64 j = 0; (j < cols_); ++j)
                p_matrix[i][j] -= m.p_matrix[i][j];
        return *this;
    }

    // multiplying the matrix by matrix
    matrix_r64 matrix_r64::operator*=(const matrix_r64& m)
    {
        matrix_r64 temp(rows_, m.cols_);
        for (INT64 i = 0; (i < temp.rows_); ++i)
            for (INT64 j = 0; (j < temp.cols_); ++j)
                for (INT64 k = 0; k < cols_; ++k)
                    temp.p_matrix[i][j] += (p_matrix[i][k] * m.p_matrix[k][j]);
        return (*this = temp);
    }

    // multiplying the matrix by number
    matrix_r64 matrix_r64::operator*=(const REAL64 num)
    {
        for (INT64 i = 0; (i < rows_); ++i)
            for (INT64 j = 0; (j < cols_); ++j) p_matrix[i][j] *= num;
        return *this;
    }

    // dividing the matrix by number
    matrix_r64 matrix_r64::operator/=(const REAL64 num)
    {
        for (INT64 i = 0; (i < rows_); ++i)
            for (INT64 j = 0; (j < cols_); ++j) p_matrix[i][j] /= num;
        return *this;
    }

    // exponenting of the matrix by number
    matrix_r64 matrix_r64::operator^(const INT32 num)
    {
        matrix_r64 temp(*this);
        return exponent(temp, num);
    }

    // swap two rows of matrix
    void matrix_r64::swap_rows(const INT64 r1, const INT64 r2)
    {
        std::swap(p_matrix[r1], p_matrix[r2]);
    }

    // swap two cols of matrix
    void matrix_r64::swap_cols(const INT64 c1, const INT64 c2)
    {
        for (INT64 i = 0; (i < rows_); ++i)
            std::swap(p_matrix[i][c1], p_matrix[i][c2]);
    }

    // transpose the matrix
    matrix_r64 matrix_r64::transpose()
    {
        matrix_r64 ret(cols_, rows_);
        for (INT64 i = 0; (i < rows_); ++i)
            for (INT64 j = 0; (j < cols_); ++j)
                ret.p_matrix[j][i] = p_matrix[i][j];
        return ret;
    }

    /***** STATIC CLASS FUNCTIONS ***
     ********************************/

     // identity matrix
    matrix_r64 matrix_r64::create_identity(const INT64 n_size)
    {
        matrix_r64 temp(n_size, n_size);
        for (INT64 i = 0; (i < temp.rows_); ++i)
        {
            for (INT64 j = 0; (j < temp.cols_); ++j)
            {
                if (i == j) temp.p_matrix[i][j] = 1;
                else temp.p_matrix[i][j] = 0;
            }
        }
        return temp;
    }

    // solve the matrixed linear equation system
    matrix_r64 matrix_r64::solve(matrix_r64 A, matrix_r64 b)
    {
        // Gaussian elimination
        for (INT64 i = 0; (i < A.rows_); ++i)
        {
            if (A[i][i] == 0)
            {
                // pivot 0 - throw error
                error_message[1] = " Matrix::solve(), the coefficient matrix has 0 as a pivot.";
                ENV::StopByError(3005, &error_message);
            }
            for (INT64 j = i + 1; (j < A.rows_); ++j)
            {
                for (INT64 k = i + 1; (k < A.cols_); ++k)
                {
                    A[j][k] -= A[i][k] * (A[j][i] / A[i][i]);
                    if EPS_CHECK(A[j][k]) A[j][k] = 0;
                }
                //std::cout << b[0][j] << " = " << b[0][i] << " * " << A[j][i] << " / " << A[i][i] << END_LINE;
                b[0][j] -= b[0][i] * (A[j][i] / A[i][i]);
                if EPS_CHECK(A[j][0]) A[j][0] = 0;
                A[j][i] = 0;
            }
        }
        // Back substitution
        rows_ = 1;
        cols_ = b.cols_;
        allocate();
        auto c1 = cols_ - 1;
        p_matrix[0][c1] = b[0][c1] / A[c1][c1];
        if EPS_CHECK(p_matrix[0][c1]) p_matrix[0][c1] = 0;
        for (INT64 i = c1 - 1; i >= 0; --i)
        {
            REAL64 d_sum = ENV_0_R64;
            for (INT64 j = i + 1; (j < cols_); ++j)
            {
                d_sum += A[i][j] * p_matrix[0][j];
            }
            p_matrix[0][i] = (b[0][i] - d_sum) / A[i][i];
            if EPS_CHECK(p_matrix[0][i]) p_matrix[0][i] = 0;
        }
        return *this;
    }

    // solve the matrixed linear equation system with band
    matrix_r64 matrix_r64::band_solve(matrix_r64 A, matrix_r64 b, const INT64 k)
    {
        // optimized Gaussian elimination
        INT64 bandsBelow = (k - 1) / 2;
        for (INT64 i = 0; (i < A.rows_); ++i)
        {
            if (A.p_matrix[i][i] == 0)
            {
                // pivot 0 - throw exception
                error_message[1] = " Matrix::band_solve(), the coefficient matrix has 0 as a pivot.";
                ENV::StopByError(3005, &error_message);
            }
            for (INT64 j = i + 1; (j < A.rows_) && (j <= i + bandsBelow); ++j)
            {
                INT64 k = i + 1;
                while (k < A.cols_ && A.p_matrix[j][k])
                {
                    A.p_matrix[j][k] -= A.p_matrix[i][k] * (A.p_matrix[j][i] / A.p_matrix[i][i]);
                    k++;
                }
                b.p_matrix[j][0] -= b.p_matrix[i][0] * (A.p_matrix[j][i] / A.p_matrix[i][i]);
                A.p_matrix[j][i] = 0;
            }
        }
        // Back substitution
        matrix_r64 x(b.rows_, 1);
        x.p_matrix[x.rows_ - 1][0] = b.p_matrix[x.rows_ - 1][0] / A.p_matrix[x.rows_ - 1][x.rows_ - 1];
        for (INT64 i = x.rows_ - 2; i >= 0; --i)
        {
            REAL64 d_sum = ENV_0_R64;
            for (INT64 j = i + 1; (j < x.rows_); ++j)
                d_sum += A.p_matrix[i][j] * x.p_matrix[j][0];
            x.p_matrix[i][0] = (b.p_matrix[i][0] - d_sum) / A.p_matrix[i][i];
        }
        return x;
    }

    // *** functions on VECTORS ***
    REAL64 matrix_r64::dot_product(matrix_r64 a, matrix_r64 b)
    {
        REAL64 r_sum = ENV_0_R64;
        for (INT64 i = 0; i < a.rows(); ++i)
        {
            r_sum += a[i][0] * b[i][0];
        }
        return r_sum;
    }

    // *** functions on AUGMENTED matrices ***

    // augment the matrix
    matrix_r64 matrix_r64::augment(matrix_r64 A, matrix_r64 B)
    {
        matrix_r64 AB(A.rows_, A.cols_ + B.cols_);
        INT64   A_cols = A.cols(), B_cols = B.cols(),
                AB_rows = AB.rows(), AB_cols = AB.cols();
        for (INT64 i = 0; (i < AB_rows); ++i)
        {
            for (INT64 j = 0; (j < AB_cols); ++j)
            {
                AB[i][j] = (j < A_cols) ? A[i][j] : B[i][j - B_cols];
            }
        }
        return AB;
    }

    // Guassian eliminate for the matrix
    matrix_r64 matrix_r64::gaussian_eliminate()
    {
        matrix_r64 Ab(*this);
        INT64 rows = Ab.rows();
        INT64 cols = Ab.cols();
        INT64 Acols = cols - 1;
        INT64 i = 0; // row tracker
        INT64 j = 0; // column tracker
        // iterate through the rows
        while (i < rows)
        {
            // find a pivot for the row
            bool pivot_found = false;
            while ((j < Acols) && !pivot_found)
            {
                if (Ab(i, j) != 0)
                { // pivot not equal to 0
                    pivot_found = true;
                }
                else
                { // check for a possible swap
                    INT64 max_row = i;
                    REAL64 max_val = 0;
                    for (INT64 k = i + 1; (k < rows); ++k)
                    {
                        REAL64 cur_abs = (Ab[k][j] >= 0) ? Ab[k][j] : -Ab[k][j];
                        if (cur_abs > max_val)
                        {
                            max_row = k;
                            max_val = cur_abs;
                        }
                    }
                    if (max_row != i)
                    {
                        Ab.swap_rows(max_row, i);
                        pivot_found = true;
                    }
                    else
                    {
                        j++;
                    }
                }
            }
            // perform elimination as normal if pivot was found
            if (pivot_found)
            {
                for (INT64 t = i + 1; (t < rows); ++t)
                {
                    for (INT64 s = j + 1; (s < cols); ++s)
                    {
                        Ab[t][s] -= Ab[i][s] * (Ab[t][j] / Ab[i][j]);
                        if EPS_CHECK(Ab[t][s]) Ab[t][s] = 0.0;
                    }
                    Ab[t][j] = 0;
                }
            }
            i++;
            j++;
        }
        return Ab;
    }

    // reducing the matrix
    matrix_r64 matrix_r64::rowReduceFromGaussian()
    {
        matrix_r64 R(*this);
        INT64 rows = R.rows();
        INT64 cols = R.cols();
        INT64 i = rows - 1; // row tracker
        INT64 j = cols - 2; // column tracker
        // iterate through every row
        while (i >= 0)
        {
            // find the pivot column
            INT64 k = j - 1;
            while (k >= 0)
            {
                if (R[i][k] != 0) j = k;
                k--;
            }
            // zero out elements above pivots if pivot not 0
            if (R(i, j) != 0)
            {
                for (INT64 t = i - 1; (t >= 0); --t)
                {
                    for (INT64 s = 0; (s < cols); ++s)
                    {
                        if (s != j)
                        {
                            R[t][s] -= R[i][s] * (R[t][j] / R[i][j]);
                            if EPS_CHECK(R[t][s]) R[t][s] = 0;
                        }
                    }
                    R(t, j) = 0;
                }
                // divide row by pivot
                for (INT64 k = j + 1; (k < cols); ++k)
                {
                    R[i][k] /= R[i][j];
                    if EPS_CHECK(R[i][k]) R[i][k] = 0;
                }
                R[i][j] = 1;
            }
            i--;
            j--;
        }
        return R;
    }

    // inverse the matrix
    matrix_r64 matrix_r64::inverse()
    {
        matrix_r64 I = matrix_r64::create_identity(rows_);
        matrix_r64 AI = matrix_r64::augment(*this, I);
        matrix_r64 U = AI.gaussian_eliminate();
        matrix_r64 IAInverse = U.rowReduceFromGaussian();
        matrix_r64 AInverse(rows_, cols_);
        for (INT64 i = 0; (i < AInverse.rows_); ++i)
        {
            for (INT64 j = 0; (j < AInverse.cols_); ++j)
                AInverse(i, j) = IAInverse(i, j + cols_);
        }
        return AInverse;
    }

    /*** PRIVATE HELPER FUNCTIONS ***
     ********************************/

     // matrix data allocator
    void matrix_r64::allocate()
    {
        PTR_DELETE(p_matrix, []);
        if (empty()) return;
        p_matrix = new REAL64 * [rows_];
        for (INT64 i = 0; (i < rows_); ++i) p_matrix[i] = new REAL64[cols_]{};
        nx_ = cols_ - 1;
    }

    // unknown exp()
    matrix_r64 matrix_r64::exponent(const matrix_r64& m, const INT32 num)
    {
        if (num == 0)
        {
            return create_identity(m.rows_);
        }
        else if (num == 1)
        {
            return m;
        }
        else if (num % 2 == 0)
        {  // num is even
            return exponent(m * m, num / 2);
        }
        else
        {                    // num is odd
            return m * exponent(m * m, (num - 1) / 2);
        }
    }

    // quick sorting with saving initial array numbers
    void matrix_r64::qsort(REAL64** x, const INT64 i_size, INT64* a)
    {
        INT64 i = 0, j = i_size - 1; // начальные значения
        REAL64* p;
        p = x[i_size >> 1];   // выбираем середину
        // процедура разделения
        do
        {
            while (x[i][nx_] < p[nx_]) i++;
            while (x[j][nx_] > p[nx_]) j--;
            if (i <= j)
            {
                std::swap(x[i], x[j]);
                std::swap(a[i], a[j]);
                i++; j--;
            }
        } while (i <= j);
        // рекурсивные вызовы, если есть, что сортировать
        if (j > 0) qsort(x, j + 1, a);
        if (i_size > i) qsort(&(x[i]), i_size - i, a + i);
    }

    /***** NON-MEMBER FUNCTIONS *****
     ********************************/

     // Binary Operations with Matrixes

    matrix_r64 operator+(const matrix_r64& m1, const matrix_r64& m2)
    {
        matrix_r64 temp(m1);
        return (temp += m2);
    }

    matrix_r64 operator-(const matrix_r64& m1, const matrix_r64& m2)
    {
        matrix_r64 temp(m1);
        return (temp -= m2);
    }

    matrix_r64 operator*(const matrix_r64& m1, const matrix_r64& m2)
    {
        matrix_r64 temp(m1);
        return (temp *= m2);
    }

    matrix_r64 operator*(const matrix_r64& m, REAL64 num)
    {
        matrix_r64 temp(m);
        return (temp *= num);
    }

    matrix_r64 operator*(REAL64 num, const matrix_r64& m)
    {
        return (m * num);
    }

    matrix_r64 operator/(const matrix_r64& m, REAL64 num)
    {
        matrix_r64 temp(m);
        return (temp /= num);
    }

    /*** HDF READ/WRITE FUNCTIONS ***
     ********************************/

#ifdef ENV_HDF5_HPP

#ifdef H5Cpp_H

     // ***Write single formula to HDF - file
     // parent_id - Parent Group identifier by H5::Group
    void matrix_r64::fwriteHDF(H5::Group* parent_id, const STRING& group)
    {
        STRING GroupName = group.empty() ? FPT_HDF_DATA_NAMES[0] : group;
        // Open an existing group in the specified file group.
        H5::Group group_id; // Group identifier
        if (GroupName.empty()) group_id = *parent_id;
        else
        {
            try
            {
                group_id = H5::Group(parent_id->createGroup(GroupName));
            }
            catch (const H5::GroupIException&)
            {
                //auto fn = parent_id->getFileName();
                //auto pn = parent_id->fromClass();
                error_message[1] = " FunctionParserType::fwriteHDF(), cannot write to output HDF-file";
                error_message[2] = " '" + STRING() + "'.";
                error_message[3] = " Group Create Error '" + STRING() + GroupName + "'.";
                ENV::StopByError(3005, &error_message);
            }
        }
        // write to the HDF-group the header data
        V_STRING vs_tmp = vslice(FPT_HDF_DATA_NAMES, 1, 4);
        if (ENV_H5::HDF_HeaderWriteSimple(&group_id, STRING_EMPTY, vs_tmp, VECTOR<INT32>({ ByteCodeSize, ImmedSize, StackSize, StackPtr })))
        {
            //auto fn = parent_id->getFileName();
            //auto pn = parent_id->fromClass();
            error_message[1] = " FunctionParserType::fwriteHDF(), cannot write to output HDF-file '" + STRING() + "'.";
            error_message[2] = " GROUP='" + STRING() + GroupName + "',";
            error_message[3] = " NAMES=[" + FPT_HDF_DATA_NAMES[1] + "'";
            for (size_t i = 2; i < 5; ++i)
                error_message[3] += ", '" + FPT_HDF_DATA_NAMES[i] + "'";
            error_message[3] += "]";
            ENV::StopByError(3005, &error_message);
        }
        // Write ByteCode array dataset.
        if (ENV_H5::HDF_Write(ByteCode, ByteCodeSize, group_id, FPT_HDF_DATA_NAMES[5]) < 0)
        {
            error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[5] + "' cannot be written.";
            ENV::StopByError(3005, &error_message);
        }
        // Write Immed array dataset.
        if (ENV_H5::HDF_Write(Immed, ImmedSize, group_id, FPT_HDF_DATA_NAMES[6]) < 0)
        {
            error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[6] + "' cannot be written.";
            ENV::StopByError(3005, &error_message);
        }
        // Write Stack array dataset.
        if (ENV_H5::HDF_Write(Stack, StackSize, group_id, FPT_HDF_DATA_NAMES[7]) < 0)
        {
            error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[7] + "' cannot be written.";
            ENV::StopByError(3005, &error_message);
        }
        // Write Formula Name dataset.
        if (Name[0].empty()) Name[0] = GroupName;
        if (ENV_H5::HDF_Write_Strings(Name, group_id, FPT_HDF_DATA_NAMES[8]) < 0)
        {
            error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[8] + "' cannot be written.";
            ENV::StopByError(3005, &error_message);
        }
        // Write Temperature Range array dataset.
        if (ENV_H5::HDF_Write(tRange, 2, group_id, FPT_HDF_DATA_NAMES[9]) < 0)
        {
            error_message[1] = " FunctionParserType::fwriteHDF(), HDF Write Error. '" + FPT_HDF_DATA_NAMES[9] + "' cannot be written.";
            ENV::StopByError(3005, &error_message);
        }
        try
        {
            // close last group
            if (!GroupName.empty()) group_id.close();
        }
        catch (H5::Exception&)
        {
            error_message[1] = " FunctionParserType::fwriteHDF(), Group Close Error '" + GroupName + "'.";
            ENV::StopByError(3005, &error_message);
        }
    }

    // ***Read single formula from HDF-file
    // parent_id - Parent Group identifier by H5::Group
    void matrix_r64::freadHDF(const H5::Group& parent_id, const STRING& group)
    {
        // Open an existing group in the specified file group.
        STRING GroupName = group.empty() ? FPT_HDF_DATA_NAMES[0] : group;
        H5::Group group_id;            // Group identifier
        if (GroupName.empty()) group_id = parent_id;
        else
        {
            try
            {
                group_id = H5::Group(parent_id.openGroup(GroupName));
            }
            catch (const H5::GroupIException&)
            {
                error_message[1] = " FunctionParserType::freadHDF(), cannot read from input HDF-file.";
                error_message[2] = " Group Open Error '" + GroupName + "'.";
                ENV::StopByError(3005, &error_message);
            }
        }
        clear();
        // read from the HDF-group the header data
        VECTOR<INT32> rdata(4, ENV_0_I32);
        V_STRING vs_tmp = vslice(FPT_HDF_DATA_NAMES, 1, 4);
        if (ENV_H5::HDF_HeaderReadSimple(group_id, STRING_EMPTY, vs_tmp, rdata))
        {
            error_message[1] = " FunctionParserType::freadHDF(), cannot read from input HDF-file.";
            error_message[2] = " GROUP='" + GroupName + "',";
            error_message[3] = " NAMES=[" + FPT_HDF_DATA_NAMES[1] + "'";
            for (auto i = 2; i < 5; ++i)
                error_message[3] += ", '" + FPT_HDF_DATA_NAMES[i] + "'";
            error_message[3] += "]";
            ENV::StopByError(3005, &error_message);
        }
        ByteCodeSize = rdata[0];
        ImmedSize = rdata[1];
        StackSize = rdata[2];
        StackPtr = rdata[3];
        // Read ByteCode array dataset.
        if (ENV_H5::HDF_Read(ByteCode, group_id, FPT_HDF_DATA_NAMES[5]) < 0)
        {
            error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + ENV::trim(FPT_HDF_DATA_NAMES[5]) + "' cannot be read.";
            ENV::StopByError(3005, &error_message);
        }
        // Read Immed array dataset.
        if (ENV_H5::HDF_Read(Immed, group_id, FPT_HDF_DATA_NAMES[6]) < 0)
        {
            error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + ENV::trim(FPT_HDF_DATA_NAMES[6]) + "' cannot be read.";
            ENV::StopByError(3005, &error_message);
        }
        // Read Stack array dataset.
        if (ENV_H5::HDF_Read(Stack, group_id, FPT_HDF_DATA_NAMES[7]) < 0)
        {
            error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + ENV::trim(FPT_HDF_DATA_NAMES[7]) + "' cannot be read.";
            ENV::StopByError(3005, &error_message);
        }
        // Read Formula Name dataset.
        if (ENV_H5::HDF_Read_Strings(Name, group_id, FPT_HDF_DATA_NAMES[8]) < 0)
        {
            error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + ENV::trim(FPT_HDF_DATA_NAMES[8]) + "' cannot be read.";
            ENV::StopByError(3005, &error_message);
        }
        // Read Temperature Range array dataset.
        if (ENV_H5::HDF_Read(tRange, group_id, FPT_HDF_DATA_NAMES[9]) < 0)
        {
            error_message[1] = "FunctionParserType::freadHDF(), HDF Read Error. '" + ENV::trim(FPT_HDF_DATA_NAMES[9]) + "' cannot be read.";
            ENV::StopByError(3005, &error_message);
        }
        try
        {
            // close last group
            if (!GroupName.empty()) group_id.close();
        }
        catch (H5::Exception&)
        {
            error_message[1] = " FunctionParserType::freadHDF(), Group Close Error '" + GroupName + "'.";
            ENV::StopByError(3005, &error_message);
        }
    }

#else

     // HDF read martrix
     // parent_id - Parent Group identifier by hid_t
    LOGICAL matrix_r64::freadHDF(const hid_t parent_id, const STRING& group, const bool compressed)
    {
        STRING GroupName = (group.empty()) ? MATRIX_HDF_DATA_NAMES[0] : group;
        // Open an existing group in the specified file group.
        hid_t group_id; // Group identifier
        if (GroupName.empty()) group_id = parent_id;
        else
        {
            group_id = ENV_H5::HDF_group_open(parent_id, GroupName);
            if (group_id < 0)
            {
                error_message[1] = " Matrix::freadHDF(), cannot read from input HDF-file,";
                error_message[2] = " Group Open Error '" + GroupName + "'.";
                ENV::StopByError(3005, &error_message);
            }
        }
        STRING ArrayType = ENV_H5::ARRAY_TYPE_REAL64;
        VECTOR<hid_t> h5type{ HDF5_TYPE_INT64, ENV_H5::HDF5_TYPE_NAME, HDF5_TYPE_REAL64 };
        //
        // 1) Read Lengths : rows and columns
        INT64 rows = ENV_0_I64, cols = ENV_0_I64;
        hid_t attr_id = H5Aopen(group_id, MATRIX_HDF_DATA_NAMES[1].c_str(), H5P_DEFAULT); // Attribute identifier
        if (attr_id < 0) return -3;
        if (H5Aread(attr_id, h5type[0], &rows) < 0) return -4;
        if (H5Aclose(attr_id) < 0) return -5;
        attr_id = H5Aopen(group_id, MATRIX_HDF_DATA_NAMES[2].c_str(), H5P_DEFAULT); // Attribute identifier
        if (attr_id < 0) return -3;
        if (H5Aread(attr_id, h5type[0], &cols) < 0) return -4;
        if (H5Aclose(attr_id) < 0) return -5;
        // 2) Read type
        attr_id = H5Aopen(group_id, MATRIX_HDF_DATA_NAMES[3].c_str(), H5P_DEFAULT);  // Attribute identifier
        if (attr_id < 0) return -6;
        char array_type[HDF5_NAME_LENGTH + 1]{};
        auto attr_type = H5Aget_type(attr_id);
        herr_t ierror = (H5Tget_class(attr_type) == H5T_STRING) ? H5Aread(attr_id, H5Tget_native_type(attr_type, H5T_DIR_ASCEND), &array_type) : -7;
        H5Tclose(attr_type);
        if (ierror < 0) return -7;
        if (H5Aclose(attr_id) < 0) return -8;
        if (ArrayType.compare(ENV::trim(array_type)) != 0) return -9;
        PTR_DELETE(p_matrix, []);
        if ((rows > ENV_0_I64) && (cols > ENV_0_I64))
        {
            ENV_H5::HDF5_DIM2[0] = rows;
            ENV_H5::HDF5_DIM2[1] = cols;
            hid_t dspace_id = H5Screate_simple(2, ENV_H5::HDF5_DIM2, NULL); // Dataspace identifier for array
            if (dspace_id < 0) return -10;
            // Dataset identifier for array
            hid_t dset_id = H5Dopen2(group_id, MATRIX_HDF_DATA_NAMES[4].c_str(), H5P_DEFAULT);  // uncompressed data
            if (dset_id < 0) return -11;
            REAL64* p_idata = new REAL64[rows * cols];
            herr_t ierror = H5Dread(dset_id, h5type[2], dspace_id, H5Dget_space(dset_id), H5P_DEFAULT, p_idata);
            rows_ = rows;
            cols_ = cols;
            allocate();
            for (auto i = ENV_0_I64; i < rows; ++i)
                std::memcpy(p_matrix[i], p_idata + i * cols, cols_ * sizeof(REAL64));
            PTR_DELETE(p_idata, []);
            H5Dclose(dset_id);
            H5Sclose(dspace_id);
            if (ierror < 0) return -12;
        }
        // close last group
        if (!group.empty()) ENV_H5::HDF_group_close(group_id, group);
        return 0;
    }

    // HDF write martrix
    // parent_id - Parent Group identifier by hid_t
    LOGICAL matrix_r64::fwriteHDF(const hid_t parent_id, const STRING& group, const bool compressed)
    {
        // Open an existing group in the specified file group.
        STRING GroupName = (group.empty()) ? MATRIX_HDF_DATA_NAMES[0] : group;
        hid_t group_id;            // Group identifier
        if (GroupName.empty()) group_id = parent_id;
        else
        {
            group_id = ENV_H5::HDF_group_open(parent_id, GroupName, "create");
            if (group_id < 0)
            {
                error_message[1] = " FunctionParserType::fwriteHDF(), cannot write to output HDF-file,";
                error_message[2] = " Group Create Error '" + GroupName + "'.";
                ENV::StopByError(3005, &error_message);
            }
        }
        STRING ArrayType = ENV_H5::ARRAY_TYPE_REAL64;
        VECTOR<hid_t> h5type{ HDF5_TYPE_INT64, ENV_H5::HDF5_TYPE_NAME, HDF5_TYPE_REAL64 };
        //
        // Create the Header dataset space.Set the size to be the current size.
        ENV_H5::HDF5_DIM1[0] = 1;
        hid_t dspace_id = H5Screate_simple(1, ENV_H5::HDF5_DIM1, NULL);
        // 1) Write Lengths : rows and columns
        hid_t attr_id = H5Acreate(group_id, MATRIX_HDF_DATA_NAMES[1].c_str(), h5type[0], dspace_id, H5P_DEFAULT, H5P_DEFAULT);   // Attribute identifier
        if (attr_id < 0) return -4;
        INT64 rows = rows_, cols = cols_;
        if (H5Awrite(attr_id, h5type[0], &rows) < 0) return -5;
        if (H5Aclose(attr_id) < 0) return -6;
        attr_id = H5Acreate(group_id, MATRIX_HDF_DATA_NAMES[2].c_str(), h5type[0], dspace_id, H5P_DEFAULT, H5P_DEFAULT);   // Attribute identifier
        if (attr_id < 0) return -4;
        if (H5Awrite(attr_id, h5type[0], &cols) < 0) return -5;
        if (H5Aclose(attr_id) < 0) return -6;
        // 2) Write type
        attr_id = H5Acreate(group_id, MATRIX_HDF_DATA_NAMES[3].data(), h5type[1], dspace_id, H5P_DEFAULT, H5P_DEFAULT);  // Attribute identifier
        if (attr_id < 0) return -7;
        if (H5Awrite(attr_id, h5type[1], ArrayType.data()) < 0) return -8;
        if (H5Aclose(attr_id) < 0) return -9;
        H5Sclose(dspace_id);
        // 3) Write array
        if ((rows > ENV_0_I64) && (cols > ENV_0_I64))
        {
            ENV_H5::HDF5_DIM2[0] = rows;
            ENV_H5::HDF5_DIM2[1] = cols;
            hid_t dspace_id = H5Screate_simple(2, ENV_H5::HDF5_DIM2, NULL); // Dataspace identifier for array
            hid_t dset_id = 0;  // Dataset identifier for array
            hid_t plist_id = 0; // Properties identifier = plist(compression)
            // Compression check
            bool is_compressed = (compressed && (rows > ENV_H5::COMPRESSION_START_LENGTH));
            if (is_compressed)
            {
                // compression and write
                hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
                if (plist_id < 0) return -10;
                if (H5Pset_chunk(plist_id, 1, ENV_H5::COMPRESSION_CHUNK_SIZE) < 0) return -11;
                if (H5Pset_deflate(plist_id, ENV_H5::COMPRESSION_LEVEL) < 0) return -12;
                dset_id = H5Dcreate2(group_id, MATRIX_HDF_DATA_NAMES[4].c_str(), h5type[2], dspace_id, H5P_DEFAULT, plist_id, H5P_DEFAULT);
            }
            // uncompressed write
            else dset_id = H5Dcreate2(group_id, MATRIX_HDF_DATA_NAMES[4].c_str(), h5type[2], dspace_id, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
            // write vector
            REAL64* p_idata = new REAL64[rows * cols];
            for (auto i = ENV_0_I64; i < rows; ++i) std::memcpy(p_idata + i * cols, p_matrix[i], cols_ * sizeof(REAL64));
            herr_t ierror = H5Dwrite(dset_id, h5type[2], dspace_id, H5Dget_space(dset_id), H5P_DEFAULT, p_idata);
            PTR_DELETE(p_idata, []);
            // plist(compression) close
            if (is_compressed) H5Pclose(plist_id);
            H5Dclose(dset_id);
            H5Sclose(dspace_id);
            if (ierror < 0) return -13;
        }
        // close last group
        if (!group.empty()) ENV_H5::HDF_group_close(group_id, group);
        return 0;
    }

#endif
#endif

}

