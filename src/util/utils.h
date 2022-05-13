#ifndef __UTILS__
#define __UTILS__
#include <chrono>
#include <sstream>
#include <iostream>

class Timer 
{
    using clock = std::chrono::high_resolution_clock;
public:
    Timer() { start(); }
    void start() {
        _start = clock::now();
    };
    double elapsed() const {
        return std::chrono::duration<double>(clock::now() - _start).count();
    };
private:
    clock::time_point _start;
};

std::ostream& log(std::ostream& os = std::cout);

inline void print() { std::cout << std::endl; }
template <typename T, typename... TAIL>
void print(const T& t, TAIL... tail) {
    std::cout << t << ' ';
    print(tail...);
}

template <typename... T>
void printlog(T... t) {
    log();
    print(t...);
}

#endif