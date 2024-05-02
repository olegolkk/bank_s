#include "formatted_exception.hpp"
#include "utils.hpp"
#include "ENV_HDF5.hpp"
#include <list>
#include <iostream>
#include <sstream>
#include <cstdarg>

// Fortran read function
extern "C" void eread_(char*, INT32*);

// Fortran next read function
extern "C" void g2n_(REAL64*, INT32*, char*, INT32*, char*, INT32*);

// C++ read function from Fortran read function
void eread(STRING& key)
{
    char key_data[HDF5_STRING_LENGTH + 1];
    int iLength = 0;
    eread_(key_data, &iLength);
    key_data[iLength] = 0;
    key = key_data;
}

// C++ next read function from Fortran next read function
INT32 g2n(REAL64& RD, INT32&ID, STRING& key, char& TR)
{
    char key_data[HDF5_STRING_LENGTH + 1];
    int iLength = 0;
    INT32 NR;
    g2n_(&RD, &ID, key_data, &iLength, &TR, &NR);
    key_data[iLength] = 0;
    key = key_data;
    return NR;
}

// making of data pointer (double*) to double
void make_data_double(const double*& data, const int size, const double*& ptr, Allocator& heap)
{
    if (size > 0)
    {
        ptr = new double[size];
        std::memcpy(const_cast<double*>(ptr), data, size * sizeof(double));
        heap.push_back(ptr);
    }
};

// Fortran write message
extern "C" void Write_(const char OutputString[], const int Length, const int UnitNumber);

// Fortran error message
extern "C" void Error_(const int ErrorNumber);

// Get Function value
extern "C" void GetFunction_(double* FunctionValue, const int iNumber, const double VariableValues[4], const int isMajorant = 0);

// fprintf for Fortran
void WriteF(const int UnitNumber, const char format[], ...)
{
    std::ostringstream fstring;
    va_list factor;
    va_start(factor, format);
    for (const char* c = format; *c; ++c)
    {
        if (*c != '%')
        {
            fstring << *c;
            continue;
        }
        switch (*++c)
        {
        case 's':
            fstring << va_arg(factor, char*);
            break;
        case 'd':
        case 'i':
            fstring << va_arg(factor, int);
            break;
        case 'f':
        case 'e':
            fstring << va_arg(factor, double);
            break;
        default:
            fstring << *c;
        }
    }
    va_end(factor);
    std::string s = fstring.str();
#ifndef NO_PRINT_TO_SCREEN
    std::cout << s.c_str() << std::endl;
#endif
    Write_(s.c_str(), s.length(), UnitNumber);
}

// fprintf for Fortran with error
void ErrorF(const int ErrorNumber, const int UnitNumber, const char format[], ...)
{
    std::ostringstream fstring;
    va_list factor;
    va_start(factor, format);
    for (const char* c = format; *c; ++c)
    {
        if (*c != '%')
        {
            fstring << *c;
            continue;
        }
        switch (*++c)
        {
        case 's':
            fstring << va_arg(factor, char*);
            break;
        case 'd':
        case 'i':
            fstring << va_arg(factor, int);
            break;
        case 'f':
        case 'e':
            fstring << va_arg(factor, double);
            break;
        default:
            fstring << *c;
        }
    }
    va_end(factor);
    std::string s = fstring.str();
    std::cout << s.c_str() << std::endl;
    std::cerr << s.c_str() << std::endl;
    Write_(s.c_str(), s.length(), UnitNumber);
    Error_(ErrorNumber);
}

// 
double clamp(double value, double min, double max, double margin)
{
    if (value < min)
    {
        if (value > min - margin) return min;
    }
    else if (value > max)
    {
        if (value < max + margin) return max;
    }
    else return value;
    throw FormattedException(
        "%e is out of bounds [%e; %e] with margin %e", value, min, max, margin
    );

};

// split Fortran string
extern "C" void c_string_split(char* filename, int* filename_)
{
    STRING path = PCharToString(filename, *filename_);
    V_STRING v_path = string_split(path, "\t");
    for (auto& s_path : v_path)
    {
        auto rpath = ENV::trim(s_path);
        if (dir_exists(rpath))
        {
            auto i_size = *filename_ - rpath.length();
            if (i_size > 0) rpath += STRING(i_size, ' ');
            std::memcpy(filename, rpath.c_str(), *filename_);
            return;
        }
    }
}
