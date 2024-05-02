#ifndef ACE_UTILS_HEADER
#define ACE_UTILS_HEADER

#include <limits>
#include <string>
#include <cstring>
#include <list>
#include <algorithm>
#include "ENV_types.hpp"


typedef std::list<const double*> Allocator;

const int STD_CACHE_SIZE = 2000;

// making of data pointer (double*) to double
void make_data_double(const double*& data, const int size, const double*& ptr, Allocator& heap);

// fprintf for Fortran
void WriteF(const int UnitNumber, const char format[], ...);

// fprintf for Fortran with error
void ErrorF(const int ErrorNumber, const int UnitNumber, const char format[], ...);

// C++ read function from Fortran read function
void eread(STRING& key);

// C++ next read function from Fortran next read function
INT32 g2n(REAL64&, INT32&, STRING&, char&);

// Square of x
inline double sqr(double x)
{
    return x * x;
};

// Square of vector
inline double vsqr(double v[3])
{
    double result = 0;
    for (int i = 0; i < 3; ++i) result += sqr(v[i]);
    return result;
};

// 
double clamp(
    double value, double min, double max,
    double margin = 10 * std::numeric_limits<double>().epsilon()
);


#endif
