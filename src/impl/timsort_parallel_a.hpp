#pragma once

#include "timsort.hpp"
#include "gfxtimsort.hpp"
#include "test_classes.hpp"
#include <thread>
#include <omp.h>

/**
 * Sorts [first, last) using timsort. If array is large enough use threads up to max hardware count of them.
 * Splits array to threads, sort them and then do final sorting in main thread.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 */
template <class It, class Compare>
void timsort_parallel_a(It first, It last, Compare comp) {
    int64_t length = std::distance(first, last);
    uint32_t threads = length >> 12;
    uint32_t maxThreads = omp_get_max_threads();
    threads = threads > maxThreads ? maxThreads : threads;
    if (threads > 1) {
        int64_t step = length / threads;
        //split array into multiple parts and sort parts
        #pragma omp parallel for
        for (uint32_t i = 0; i < threads; i++) {
            It first1 = first+(i*step);
            It last1 = i == threads - 1 ? last : first1+step-1;
            TimSort<It, Compare> tim(first1, last1, comp);
            tim.sort();
        }
    }
    timsort(first, last, comp);
}

/**
 * Sorts [first, last) using timsort. If array is large enough use threads up to max hardware count of them.
 * Splits array to threads, sort them and then do final sorting in main thread.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 */
template <class It>
void timsort_parallel_a(It first, It last) {
    timsort_parallel_a(first, last, std::less<>());
}
