#pragma once

#include <iostream>
#include <atomic>


struct StableTestClass {
    int a;
    int b;
    int c;
    bool operator==(const StableTestClass& y) const {
        return a == y.a && b == y.b && c == y.c;
    }
    bool operator<(const StableTestClass& y) const {
        if (a == y.a) {
            if (b == y.b) {
                return c < y.c;
            } else
                return b < y.b;
        } else
            return a < y.a;
    }
};

bool stable_comp1(const StableTestClass & a, const StableTestClass & b) {
    return a.a < b.a;
}
bool stable_comp2(const StableTestClass & a, const StableTestClass & b) {
    return a.b < b.b;
}
bool stable_comp3(const StableTestClass & a, const StableTestClass & b) {
    return a.c < b.c;
}

struct LargeTestClass {
    //random variables
    uint64_t name = 5022;
    int y = 3;
    double z = 50.8;
    
    //variables used for sorting
    StableTestClass a;
    StableTestClass b;
    StableTestClass c;
    int d;
    bool operator<(const LargeTestClass& y) const {
        if (a == y.a) {
            if (b == y.b) {
                if (c == y.c)
                    return d < y.d;
                else
                    return c < y.c;
            } else
                return b < y.b;
        } else
            return a < y.a;
    }
};




template <class T>
bool my_compare(const T & x, const T & y) {
    return x < y;
}

std::atomic<uint64_t> compareCount;

template <class T>
bool count_compare(const T & x, const T & y) {
    compareCount++;
    return x < y;
}

//https://stackoverflow.com/questions/1152333/force-compiler-to-not-optimize-side-effect-less-statements
//https://en.cppreference.com/w/cpp/language/cv
//volatile variable won't be optimalized
template <typename T>
bool slow_compare(const T & x, const T & y) {
    volatile int z = 0;
    while (true) {
        //ignore volatile warning, since we don't care about the value anyway
#pragma GCC diagnostic ignored "-Wvolatile"
        if (++z == 10)
            break;

    }
    return (x < y);
}