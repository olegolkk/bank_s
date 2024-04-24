#pragma once

#ifndef ENV_TYPES_HPP
#define ENV_TYPES_HPP

#include <cstring>
#include <cstdint>
#include <cmath>
#include <numeric>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <thread>
#include <functional>


#ifdef _MSC_VER
#define forceinline __forceinline
#elif defined(__GNUC__)
#define forceinline inline __attribute__((__always_inline__))
#elif defined(__CLANG__)
#if __has_attribute(__always_inline__)
#define forceinline inline __attribute__((__always_inline__))
#else
#define forceinline inline
#endif
#else
#define forceinline inline
#endif

//#ifdef _DEBUG
//#define __CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
//#define new DEBUG_NEW
//#endif // _DEBUG

#define END_LINE std::endl
#define FSTREAM std::fstream
#define VECTOR std::vector
#define STRING std::string
#define V_STRING VECTOR<STRING>
#define STRING_EMPTY STRING()
#define LOGICAL std::int32_t
#define INT08 std::int8_t
#define INT32 std::int32_t
#define INT64 std::int64_t
#define REAL32 float
#define REAL64 double
#define REAL128 long double
// Structure for time saving
#define CTIMER std::chrono::system_clock::time_point
// Delete with nullify
#define PTR_DELETE_(ptr, brackets, ...) \
    if (ptr != nullptr) \
    { \
        delete brackets ptr; \
        ptr = nullptr; \
    }
#define INVOKE_PTR_DELETE_(ARGS) PTR_DELETE_ ARGS
#define PTR_DELETE(...) INVOKE_PTR_DELETE_((__VA_ARGS__, ))
#ifdef KIR_VERSION
#include "version.hpp"
!Constants:
!GIT_VERSION - current version from git,
!GIT_TIME_RELEASE - time and data of version release,
!GIT_TIME_BUILD - time and data of version build.
#endif

// Print Version to channel wch.
static void PrintVersion(const FSTREAM & wch) // (INT32 const wch)
{
    // wch << STRING_LINE;
#ifdef KIR_VERSION
#ifdef DEBUG
    wch << " Version (Debug) SHA   : " + GIT_VERSION + END_LINE
#else
    wch << " Version (Release) SHA : " + GIT_VERSION + END_LINE
#endif
        wch << " Release Date and Time : " + GIT_TIME_RELEASE + END_LINE
        wch << STRING_LINE + END_LINE
        wch << " Build Date and Time   : " + GIT_TIME_BUILD + END_LINE
        wch << STRING_LINE + END_LINE
#endif
}

namespace ENV
{
    const INT08
        ENV_0_I08 = INT8_C(0), ENV_1_I08 = INT8_C(1), ENV_1M_I08 = INT8_C(-1);
    const INT32
        ENV_0_I32 = INT32_C(0),
        ENV_1_I32 = INT32_C(1),
        ENV_1M_I32 = INT32_C(-1),
        ENV_2_I32 = INT32_C(2),
        ENV_2M_I32 = INT32_C(-2);
    const INT64
        ENV_0_I64 = INT64_C(0),
        ENV_1_I64 = INT64_C(1),
        ENV_1M_I64 = INT64_C(-1),
        ENV_2_I64 = INT64_C(2),
        ENV_2M_I64 = INT64_C(-2),
        BUFFER = INT64_C(1000000);
    const REAL32
        ENV_0_R32 = +0.0f,
        ENV_1_R32 = +1.0f,
        PI_R32 = atanf(1.0f) * 4.0f,
        RADIAN_R32 = PI_R32 / 180.0f,
        ENV_1M_R32 = -1.0f,
        ENV_2_R32 = +2.0f,
        LOGTWO_R32 = logf(ENV_2_R32),
        HUGE_R32 = 1E20f,
        TINY_R32 = std::numeric_limits<REAL32>::min();
    const REAL64
        ENV_0_R64 = +0.0,
        ENV_1_R64 = +1.0,
        PI_R64 = atan(1.0) * 4.0,
        RADIAN_R64 = PI_R64 / 180.0,
        ENV_1M_R64 = -1.0,
        ENV_2_R64 = +2.0,
        ENV_2M_R64 = -2.0,
        LOGTWO_R64 = log(ENV_2_R64),
        HUGE_R64 = 1E200,
        TINY_R64 = std::numeric_limits<REAL64>::min();
    const REAL128
        ENV_0_R128 = +0.0l,
        ENV_1_R128 = +1.0l,
        PI_R128 = atanl(ENV_1_R128) * 4.0l,
        RADIAN_R128 = PI_R128 / 180.0l,
        HUGE_R128 = 1E200l,
        TINY_R128 = std::numeric_limits<REAL128>::min();
    const STRING
        STRING_LINE = STRING("==================================================================");   // 67 symbols
    // REAL128 ENV_PI_R128 =
    // 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679821480865132823066470938446095

    const  STRING PHYSICS_GroupName = STRING_EMPTY;

    /*
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
    */
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
    static void StopByError(INT32 iError, V_STRING* error_message = nullptr, FSTREAM* FileOfOut = nullptr)
    {
        std::cout << " " << END_LINE;
        std::cout << "ERROR CODE: " << iError << END_LINE;
        if (FileOfOut != nullptr)
        {
            *FileOfOut << " " << END_LINE;
            *FileOfOut << "ERROR CODE:" << iError << END_LINE;
        }
        if (error_message != nullptr)
            for (auto emi = error_message->begin(); emi != error_message->end(); ++emi)
            {
                if (emi->length())
                {
                    std::cout << *emi << END_LINE;
                    if (FileOfOut != nullptr) *FileOfOut << *emi << END_LINE;
                }
            }
        exit(iError);
    }

    // Print Warning message
    forceinline void PrintWarning(V_STRING* warning_message = nullptr, FSTREAM* FileOfOut = nullptr)
    {
        if (warning_message)
        {
            for (auto wmi = warning_message->begin(); wmi != warning_message->end(); ++wmi)
                if (wmi->length())
                {
                    std::cout << *wmi << END_LINE;
                    if (FileOfOut != nullptr) *FileOfOut << *wmi << END_LINE;
                }
        }
    }

    // Left trim
    forceinline const STRING ltrim(const STRING& s)
    {
        size_t start = s.find_first_not_of(" \n\r\t\f\v");
        return (start == STRING::npos) ? STRING("") : s.substr(start);
    }

    // Right trim
    forceinline const STRING rtrim(const STRING& s)
    {
        size_t end = s.find_last_not_of(" \n\r\t\f\v");
        return (end == STRING::npos) ? STRING("") : s.substr(0, end + 1);
    }

    // All trim
    forceinline const STRING trim(const STRING& s)
    {
        return rtrim(ltrim(s));
    }

    // STRING slice
    forceinline STRING s_slice(const STRING& s, const int b, const int e)
    {
        size_t ssize = s.length();
        size_t length = (e > ssize) ? ssize : (e + 1);
        auto sbegin = s.begin();
        return (e < b) ? STRING_EMPTY : STRING(sbegin + b, sbegin + length);
    }

    // Vector slice
    template<typename T>
    forceinline VECTOR<T> v_slice(const VECTOR<T>& v, const int b, const int e)
    {
        auto vsize = v.size();
        size_t length = (e > vsize) ? vsize : (e + 1);
        auto vbegin = v.begin();
        return (e < b) ? VECTOR<T>() : VECTOR<T>(vbegin + b, vbegin + length);
    }

    template <typename T, size_t N>
    forceinline size_t sizeof_array(T(&)[N])
    {
        return N;
    }

    struct size_less
    {
        template<class T> bool operator()(const T& a, const T& b) const
        {
            return a.size() < b.size();
        }
    };

    // get max length of strings in vector
    forceinline size_t max_line_length(V_STRING& lines)
    {
        return std::max_element(lines.begin(), lines.end(), size_less())->size();
    }

    forceinline size_t max_line_length(STRING* lines, const size_t l_lines)
    {
        return std::max_element(lines, lines + (l_lines - 1), size_less())->size();
    }

    forceinline bool isCharInText(const char c, const STRING& Text)
    {
        return std::any_of(Text.begin(), Text.end(), [&](auto item) { return (c == item); });
    }

    forceinline bool isStringInVector(const STRING s, const V_STRING& Text)
    {
        return std::any_of(Text.begin(), Text.end(), [&](auto item) { return (s == item); });
    }

    template <typename T>
    forceinline int signum(const T val)
    {
        return (T(0) < val) - (val < T(0));
    }

    template <typename T>
    forceinline STRING add_AI(const STRING s, const T v)
    {
        return s + std::to_string(v);
    }

    template<typename T>
    forceinline STRING stringify(const T& input)
    {
        std::ostringstream output;
        output << input;
        return output.str();
    }
    forceinline void ReplaceStringInPlace(STRING& subject, const STRING& search, const STRING& replace)
    {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != STRING::npos)
        {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
    }

    // for string vector join
    forceinline STRING vector_join(const V_STRING vs, const STRING& delimiter = STRING_EMPTY)
    {
        if (vs.empty()) return STRING_EMPTY;
        STRING rs = vs[0];
        if (vs.size() > 1)
            for (auto s = vs.begin() + 1; s != vs.end(); ++s) rs += delimiter + *s;
        return rs;
    }

    // string splitting by delimiter
    forceinline V_STRING string_split(const STRING& s, const STRING& delimiter = STRING(" "))
    {
        V_STRING res;
        if (s.empty())
        {
            res.push_back(STRING_EMPTY);
            return res;
        }
        size_t pos_start = 0, pos_end, delimiter_len = delimiter.length();
        while ((pos_end = s.find(delimiter, pos_start)) != STRING::npos)
        {
            STRING token = s.substr(pos_start, pos_end - pos_start);
            token = trim(token);
            pos_start = pos_end + delimiter_len;
            if (token.empty()) continue;
            res.push_back(token);
        }
        STRING token = trim(s.substr(pos_start));
        if (!token.empty()) res.push_back(token);
        if (res.size() == 0) res.push_back(STRING_EMPTY);
        return res;
    }

    // String To Double
    forceinline bool StrToReal64(const STRING& s, REAL64& r)
    {
        size_t iposition = 0;
        r = std::stod(s, &iposition);
        return (iposition == s.length());
    }

    // String To Int
    forceinline bool StrToInt(const STRING& s, int& i)
    {
        size_t iposition = 0;
        i = std::stoi(s, &iposition);
        return (iposition == s.length());
    }

    // Converter of Char Pointer to String
    forceinline const STRING PCharToString(char* data, const int length)
    {
        char* s_tmp = new char[length + 1];
        std::memcpy(s_tmp, data, length);
        s_tmp[length] = ENV_0_I08;
        STRING s(s_tmp);
        PTR_DELETE(s_tmp, []);
        return s;
    }

    // Sum of Array
    forceinline int array_sum(int a[], const int n)
    {
        int initial_sum = 0;
        return std::accumulate(a, a + n, initial_sum);
    }

    // Timer
    forceinline CTIMER chrono_timer()
    {
        return std::chrono::system_clock::now();
    }

    // Elapsed Time in seconds
    forceinline double elapsed_ctime(const CTIMER start, const CTIMER* end = nullptr)
    {
        std::chrono::duration<double> d = ((end) ? *end : chrono_timer()) - start;
        return d.count();
    }

    /*
    // for clock_gettime()
    double _ctimediff(const struct timespec& start, const struct timespec& end)
    {
        return (end.tv_nsec >= start.tv_nsec)
            ? (end.tv_nsec - start.tv_nsec) / 1e6 + (end.tv_sec - start.tv_sec) * 1e3
            : (start.tv_nsec - end.tv_nsec) / 1e6 + (end.tv_sec - start.tv_sec - 1) * 1e3;
    }

    // Timer class
    template <class DT = std::chrono::nanoseconds, class ClockT = std::chrono::steady_clock>
    class Timer
    {
        using timep_t = typename ClockT::time_point;
        timep_t _start = ClockT::now(), _end = {};
    public:
        // 1st - tick
        void tick()
        {
            _end = timep_t{};
            _start = ClockT::now();
        }
        // 2nd - tock
        void tock()
        {
            _end = ClockT::now();
        }
        // the duration of the tick-tock time
        template <class T = DT>
        auto duration() const
        {
            //gsl_Expects(_end != timep_t{} && "toc before reporting");
            return std::chrono::duration_cast<T>(_end - _start);
        }
    };
    */

    // File Existing Check
    forceinline bool file_exists(STRING const& file_path)
    {
        return std::ifstream(file_path).good();
    }

    //// Directory Existing Check
    //forceinline bool dir_exists(STRING const& dir_path)
    //{
    //    std::filesystem::directory_entry entry{dir_path};
    //    return entry.exists();
    //}

    // Fortran function RND ~U(0;1)
    extern "C" double fltrn_();
    // RND ~U(0;1) with multiplying on max
    inline double mmk_random(double max_value = ENV_1_R64)
    {
        return fltrn_() * max_value;
    };

    //// random number generator
    typedef std::function<double(double)> RNG;

    // function of RND ~U(0;1) with multiplying on mmk_random
//    inline RNG rng(mmk_random);
    // function of RND ~U(0;1) with multiplying on x
//    inline double rnd(double x = 1.0) { return rng(x); }

}

#endif
