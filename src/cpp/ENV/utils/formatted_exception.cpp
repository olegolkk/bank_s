#define _CRT_SECURE_NO_WARNINGS
#include <stdexcept>

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cstring>

#include "formatted_exception.hpp"

// TODO: use asprintf if available
int vxsprintf(char** strp, const char* fmt, va_list ap)
{
    int len = (*strp != nullptr) ? strlen(*strp) : -1;
    va_list ap2;
    va_copy(ap2, ap);
    int newlen = vsnprintf(*strp, len + 1, fmt, ap2);
    va_end(ap2);
    if (newlen > len) {
        void* s = realloc(*strp, newlen + 1);
        if (!s) return -1;
        *strp = static_cast<char*>(s);
        return vsprintf(*strp, fmt, ap);
    };
    return newlen;
};

FormattedException::FormattedException(const char* format, ...) : message(nullptr)
{
    va_list ap;
    va_start(ap, format);
    int size = vxsprintf(&message, format, ap);
    va_end(ap);
    if (size < 0) throw std::runtime_error("FormattedException: xsprintf failed");
};

FormattedException::~FormattedException() throw()
{
    free(message);
};

void FormattedException::add(const char* format, ...)
{
    char* s = nullptr;
    va_list ap;
    va_start(ap, format);
    int size = vxsprintf(&s, format, ap);
    va_end(ap);
    if (size < 0) throw std::runtime_error("FormattedException: xsprintf failed");

    int l = strlen(message);
    void* r = realloc(message, l + size + 1);
    if (!r) throw std::runtime_error("FormattedException: realloc failed");
    message = static_cast<char*>(r);
    std::memcpy(message + l, s, size + 1);
    free(s);
};
