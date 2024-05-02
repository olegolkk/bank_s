#ifndef FORMATTED_EXCEPTION_HEADER
#define FORMATTED_EXCEPTION_HEADER

#include <exception>

class FormattedException : public std::exception
{
public:
    FormattedException(const char* format, ...);
    ~FormattedException() throw();
    const char* what() const throw() { return message; };
    void add(const char* format, ...);
private:
    char* message;
};

#endif
