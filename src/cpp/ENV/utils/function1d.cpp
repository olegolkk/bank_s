#include "function1d.hpp"

#include <algorithm>

#include <cmath>

#include "formatted_exception.hpp"



double identity(double x)
{
    return x;
};

PointwiseFunction1d::PointwiseFunction1d() : size(0), x(nullptr), y(nullptr) {};

PointwiseFunction1d::PointwiseFunction1d(
    int size, const double* x, const double* y
) : size(size), x(x), y(y) {};


PointwiseFunction1d::PointwiseFunction1d(int size, const double* c) :size(size), x(c) { lnu = 1; }


int hunt_search(const double* vector, int size, int previous, double value) {
    int i = previous;
    if (value >= vector[i]) {
        int step = 1;
        while ((i += step) < size && value >= vector[i]) step <<= 1;
        i -= step;
        int ind = upper_bound(
                vector + i, vector + std::min(i + step, size), value
            ) - vector;
        return ind == size ? ind - 2 : ind - 1;
        /*return upper_bound(
            vector + i, vector + std::min(i + step, size), value
        ) - vector - 1;*/
    }
    else {
        int step = 16;
        while (i > step && value < vector[i -= step]) step <<= 1;
        if (value < vector[i]) {
            int ind = upper_bound(vector, vector + i, value) - vector;
            return ind == 0 ? 0 : ind - 1;
            //return upper_bound(vector, vector + i, value) - vector - 1;
        }
        else
            return upper_bound(vector + i, vector + i + step, value) - vector - 1;
    };
};

double PointwiseFunction1d::operator()(double z, int* previous) const {
    if (size == 0) return 0.0;
    if (lnu == 1) {
        double result = 0;
        for (int i = size; i--;) {
            result *= z;
            result += x[i];
        }
        return result;
    }
    if (z < x[0]) {
        if (left) return left(z);
        throw OutOfRange(x[0] * 1e6, x[size - 1] * 1e6, z * 1e6);
    }
    else if (z > x[size - 1]) {
        if (right) return right(z);
        throw OutOfRange(x[0] * 1e6, x[size - 1] * 1e6, z * 1e6);
    };

    int i;
    if (previous)
        if (*previous == 0)
            if (z < x[1])
                i = 0;
            else // special case when the previous value is wanted but not initialized
                i = *previous = upper_bound(x, x + size, z) - x - 1;
        else
            i = *previous = hunt_search(x, size, *previous, z);
    else
        i = upper_bound(x, x + size, z) - x - 1;

    return interpolator(x, y, size, z, i);
};

PointwiseFunction1d::operator bool() const {
    return size != 0;
};

function1d operator+(const function1d& f, const function1d& g) {
    return [=](double x) -> double { return f(x) + g(x); };
};

function1d operator*(const function1d& f, const function1d& g) {
    return [=](double x) -> double { return f(x) * g(x); };
};

function1d& operator+=(function1d& f, const function1d& g) {
    if (!f) return f = g;
    return f = f + g;
};

function1d operator*(double x, const function1d& f) {
    return [=](double z) -> double { return x * f(z); };
};

double pointwise_eval(
    const double* x,
    const double* y,
    double        z,
    int        size,
    const function1d& left,
    const function1d& right,
    const Interpolator& interpolator
) {
    if (z < x[0]) {
        if (left) return left(z);
        throw OutOfRange(x[0] * 1e6, x[size - 1] * 1e6, z * 1e6);
    };
    if (z > x[size - 1]) {
        if (right) return right(z);
        throw OutOfRange(x[0] * 1e6, x[size - 1] * 1e6, z * 1e6);
    };
    return interpolator(x, y, size, z, upper_bound(x, x + size, z) - 1 - x);
};




int binary_search(const double* vector, int size, double value) {
    int result = upper_bound(vector, vector + size, value) - vector;
    if (result == 0 || result == size)
        throw OutOfRange(vector[0] * 1e6, vector[size - 1] * 1e6, value * 1e6);
    return result - 1;
};

double interpolate_linear(
    double x1, double y1, double x2, double y2, double x
) {
    return y1 + (y2 - y1) * (x - x1) / (x2 - x1);
};

double interpolate_log_lin(
    double x1, double y1, double x2, double y2, double x
) {
    return y1 + (y2 - y1) * std::log(x / x1) / std::log(x2 / x1);
};

double interpolate_lin_log(
    double x1, double y1, double x2, double y2, double x
) {
    return y1 * std::pow(y2 / y1, (x - x1) / (x2 - x1));
};

double interpolate_log_log(
    double x1, double y1, double x2, double y2, double x
) {
    return y1 * std::pow(y2 / y1, std::log(x / x1) / std::log(x2 / x1));
};

Interpolator interpolator_2p(
    const std::function<double(double, double, double, double, double)>&
    interpolator
) {
    return [=](
        const double* x, const double* y, int size, double value, int index
        ) -> double {
            return interpolator(x[index], y[index], x[index + 1], y[index + 1], value);
    };
};
