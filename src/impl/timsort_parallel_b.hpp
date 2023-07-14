#pragma once


#include "gfxtimsort.hpp"
#include "merging.hpp"


/**
 * Splits hi-lo runs into half until lo=hi and then merges them using gfx::timmerge.
 * Uses OpenMP tasks for parallelism for larger parts.
 *
 * @param runs
 * @param lo index of first run
 * @param hi index of last run
 * @param comp compare function
 */
template <class It, class Compare>
void merge_runs_parallel(std::deque<MySlice<It>> & runs, uint64_t lo, uint64_t hi, Compare comp) {
    if (lo == hi)
        return;
    uint64_t dist = std::distance(runs[hi].start, runs[lo].end);

    uint64_t m = (lo + hi) / 2;
#pragma omp taskgroup
    {
#pragma omp task shared(runs) untied if (dist >= (1<<14))
        merge_runs_parallel(runs, lo, m, comp);
#pragma omp task shared(runs) untied if (dist >= (1<<14))
        merge_runs_parallel(runs, m + 1, hi, comp);
#pragma omp taskyield
    }
    gfx::timmerge(runs[hi].start, runs[m].start, runs[lo].end, comp);
}

/**
 * Sorts the elements in the range [first, last) using provided compare function.
 * Finds natural runs and then merge them using gfx::timmerge.
 * Uses OpenMP tasks for parallelism for larger parts.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 */
template <class It, class Compare>
void timsort_parallel_b(It first, It last, Compare comp) {


#pragma omp parallel
#pragma omp single
    {
        uint64_t size = std::distance(first, last);
        if (size <= 32){
            binary_insertion_sort(first, last, first, comp);
        } else {
            auto runs = find_run(first, last, comp);

            merge_runs_parallel(runs, 0, runs.size() - 1, comp);
        }
    }
}

/**
 * Sorts the elements in the range [first, last).
 * Finds natural runs and then merge them using gfx::timmerge.
 * Uses OpenMP tasks for parallelism for larger parts.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 */
template <class It>
void timsort_parallel_b(It first, It last) {
    timsort_parallel_b<It, decltype(std::less<>())>(first, last, std::less<>());
}
