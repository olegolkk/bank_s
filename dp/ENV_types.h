#pragma once

#include <corecrt_math.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#define ENDL std::endl
#define FSTREAM std::fstream
#define VECTOR std::vector
#define STRING std::string
#define LOGICAL std::int32_t
#define INT08 std::int8_t
#define INT32 std::int32_t
#define INT64 std::int64_t
#define REAL32 float
#define REAL64 double
#define REAL128 long double
size_t const
ENV_CHAR_STRING_LENGTH = 300,
ENV_CHAR_NAME_LENGTH = 24;

INT08 const
ENV_0_I08 = 0i8, ENV_1_I08 = 1i8, ENV_1M_I08 = -1i8;
INT32 const
ENV_0_I32 = +0i32,
ENV_1_I32 = +1i32,
ENV_1M_I32 = -1i32,
ENV_2_I32 = +2i32,
ENV_2M_I32 = -2i32;
INT64 const
ENV_BUFFER = 1000000i64,
ENV_0_I64 = +0i64,
ENV_1_I64 = +1i64,
ENV_1M_I64 = -1i64,
ENV_2_I64 = +2i64,
ENV_2M_I64 = -2i64;
REAL32 const
ENV_0_R32 = +0.0f,
ENV_1_R32 = +1.0f,
ENV_PI_R32 = atanf(ENV_1_R32) * 4.0f,
ENV_RADIAN_R32 = ENV_PI_R32 / 180.0f,
ENV_1M_R32 = -1.0f,
ENV_2_R32 = +2.0f,
ENV_LOGTWO_R32 = logf(ENV_2_R32),
ENV_HUGE_R32 = 1E20f,
ENV_TINY_R32 = std::numeric_limits<REAL32>::min();
REAL64 const
ENV_0_R64 = +0.0,
ENV_1_R64 = +1.0,
ENV_PI_R64 = atan(ENV_1_R64) * 4.0,
ENV_RADIAN_R64 = ENV_PI_R64 / 180.0,
ENV_1M_R64 = -1.0,
ENV_2_R64 = +2.0,
ENV_2M_R64 = -2.0,
ENV_LOGTWO_R64 = log(ENV_2_R64),
ENV_HUGE_R64 = 1E200,
ENV_TINY_R64 = std::numeric_limits<REAL64>::min();
REAL128 const
ENV_0_R128 = +0.0l,
ENV_1_R128 = +1.0l,
ENV_PI_R128 = atanl(ENV_1_R128) * 4.0l,
ENV_RADIAN_R128 = ENV_PI_R128 / 180.0l,
ENV_HUGE_R128 = 1E200l,
ENV_TINY_R128 = std::numeric_limits<REAL128>::min();
STRING const
ENV_LINES = "==================================================================";   // 67 symbols
// REAL128 ENV_PI_R128 =
// 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095
#ifdef VERSION
#include "version.hpp"
!Constants:
!GIT_VERSION - current version from git,
!GIT_TIME_RELEASE - time and data of version release,
!GIT_TIME_BUILD - time and data of version build.
#endif

STRING const PHYSICS_GroupName = "";

// Print Version to channel wch.
void ENV_PrintVersion(FSTREAM wch) // (INT32 const wch)
{
    // wch << trim(ENV_LINES);
#ifdef VERSION
#ifdef DEBUG
    wch << " Version (Debug) SHA   : " + GIT_VERSION + ENDL
#else
    wch, << Version(Release) SHA : " + GIT_VERSION + ENDL
#endif
        wch << " Release Date and Time : " + GIT_TIME_RELEASE + ENDL
        wch << ENV_LINES + ENDL
        wch << " Build Date and Time   : " + GIT_TIME_BUILD + ENDL
        wch << ENV_LINES + ENDL
#endif
}

// Finds left bound in intervals by value.
int ENV_BinarySearch(REAL64 const value, REAL64 const intervals[])
{
    int right = sizeof(intervals) / sizeof(double) - 1;
    if (value >= intervals[right]) return right;
    int left = 0;
    if (value < intervals[left]) return left - ENV_1_I32;
    int res;
    while (right - left > ENV_1_I32)
    {
        int mid = (right + left) >> 1;
        if (value >= intervals[mid]) left = mid;
        else right = mid;
    }
    res = left;
}

/*
 Stop By Error with error message.
 Error Value format for KIR:
 [-](<T> * 1000 + <M>)
 (
     KIR Error Value :
 negative - for Fortran,
 positive - for C++.
 )
 T (error type) :
    1 - User input error
    2 - Error while task modeling
    3 - Technical error(opening / closing files, reading / writing, allocating arrays, etc)
 M (module or submodule) :
    1 - Bank module
    2 - MIK / BurnUp module
    3 - Meta and Iteration submodule of Control module
    4 - Control module
    5 - Environment module
    6 - Geometry module
    7 - Physics module
    8 - Registration module
    9 - Source module
   10 - Trajectory module
 ~
 other:
 32167 - Abnormal error(unknown)
*/
void ENV_StopByError(INT32 iError, VECTOR<STRING>* error_message = nullptr, FSTREAM* FileName = nullptr)
{
    std::cout << " " << ENDL;
    std::cout << "ERROR CODE: " << iError << ENDL;
    if (FileName)
    {
        *FileName << " " << ENDL;
        *FileName << "ERROR CODE:" << iError << ENDL;
    }
    if (error_message)
        for (auto emi = error_message->begin(); emi != error_message->end(); ++emi)
        {
            if (emi->length())
            {
                std::cout << *emi << ENDL;
                if (FileName) *FileName << *emi << ENDL;
            }
        }
    exit(iError);
}

// Print Warning message
void ENV_PrintWarning(VECTOR<STRING>* warning_message = nullptr, FSTREAM* FileName = nullptr)
{
    if (warning_message)
    {
        for (auto wmi = warning_message->begin(); wmi != warning_message->end(); ++wmi)
            if (wmi->length())
            {
                std::cout << *wmi << ENDL;
                if (FileName) *FileName << *wmi << ENDL;
            }
    }
}

// Left trim
STRING const ltrim(STRING const& s)
{
    size_t start = s.find_first_not_of(" \n\r\t\f\v");
    return (start == STRING::npos) ? STRING("") : s.substr(start);
}

// Right trim
STRING const rtrim(STRING const& s)
{
    size_t end = s.find_last_not_of(" \n\r\t\f\v");
    return (end == STRING::npos) ? STRING("") : s.substr(0, end + 1);
}

// All trim
STRING const trim(STRING const& s)
{
    return rtrim(ltrim(s));
}

// Vector slice
template<typename T>
VECTOR<T> vslice(VECTOR<T> const& v, int const m, int const n)
{
    auto first = v.cbegin() + m;
    auto last = v.cbegin() + n + 1;
    VECTOR<T> vec(first, last);
    return vec;
}

