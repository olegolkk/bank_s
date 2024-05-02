#ifndef FUNCTION1D_HEADER
#define FUNCTION1D_HEADER

#include <functional>
#include <algorithm>
#include <exception>
#include <sstream>

#include <cstddef>

typedef std::function<double(double)> function1d;

template<class ForwardIt, class T>
ForwardIt upper_bound(ForwardIt first, ForwardIt last, const T& value)
{
    ForwardIt it;
    typename std::iterator_traits<ForwardIt>::difference_type count, step;
    count = std::distance(first, last);

    while (count > 0)
    {
        it = first;
        step = count >> 1;
        std::advance(it, step);
        if (!(value < *it))
        {
            first = ++it;
            count -= step + 1;
        }
        else
            count = step;
    }
    return first;
}

class OutOfRange : public std::exception
{
    double m_left;
    double m_right;
    double m_value;
    std::string message;
public:

    OutOfRange(double left, double right, double value) :
        m_left(left), m_right(right), m_value(value)
    {
        std::stringstream ss;
        ss << value << " is out of range [" << left << "; " << right << ']';
        message = ss.str();
    };

    ~OutOfRange() throw() {};

    double left()  const { return m_left; };
    double right() const { return m_right; };
    double value() const { return m_value; };

    const char* what() const throw() {
        return message.c_str();
    };
};

double identity(double x);

typedef std::function<double(
    const double*, const double*, int, double, int
    )> Interpolator;

double interpolate_linear(double x1, double y1, double x2, double y2, double x);
double interpolate_log_lin(
    double x1, double y1, double x2, double y2, double x
);
double interpolate_lin_log(
    double x1, double y1, double x2, double y2, double x
);
double interpolate_log_log(
    double x1, double y1, double x2, double y2, double x
);

Interpolator interpolator_2p(
    const std::function<double(double, double, double, double, double)>&
    interpolator
);

double pointwise_eval(
    const double* x,
    const double* y,
    double        z,
    int        size,
    const function1d& left,
    const function1d& right,
    const Interpolator& interpolator
);

class PointwiseFunction1d {
public:
    int size = 0;
    const double* x = nullptr;
    const double* y = nullptr;
    function1d left;
    function1d right;
    Interpolator interpolator = interpolator_2p(interpolate_linear);
    int lnu = 2;

    PointwiseFunction1d();
    PointwiseFunction1d(int size, const double* x, const double* y);
    PointwiseFunction1d(int size, const double* c);

    double operator()(double z, int* previous = nullptr) const;
    explicit operator bool() const;
};

function1d operator+(const function1d& f, const function1d& g);
function1d operator*(const function1d& f, const function1d& g);

function1d& operator+=(function1d& f, const function1d& g);

function1d operator*(double x, const function1d& f);

int binary_search(const double* vector, int size, double value);
int hunt_search(const double* vector, int size, int previous, double value);

#endif
