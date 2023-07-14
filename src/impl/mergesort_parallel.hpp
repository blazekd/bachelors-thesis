#pragma once

#include "gfxtimsort.hpp"

/**
 * Split into two and recursively call then merge the two parts again. Uses binary sort for small parts.
 * Uses OpenMP tasks for parallelism for larger parts.
 *
 * Based on MergeSort V3 from https://cw.fel.cvut.cz/old/_media/courses/b4m35pag/lab6_slides_advanced_openmp.pdf
 *
 * @param left left iterator
 * @param right right iterator
 * @param comp compare function
 */
template <typename It, typename Compare>
void merge_sort_parallel_a_rec(It left, It right, Compare comp) {
    if (left < right) {
        if (right - left >= 32) {
            It mid = left + std::distance(left, right) / 2;
#pragma omp taskgroup
            {
#pragma omp task untied if(right - left >= (1<<14))
                merge_sort_parallel_a_rec(left, mid, comp);
#pragma omp task untied if(right - left >= (1<<14))
                merge_sort_parallel_a_rec(mid, right, comp);
#pragma omp taskyield
            }
            std::inplace_merge(left, mid, right, comp);
        } else {
            binary_insertion_sort(left, right, left, comp);
        }
    }
}

/**
 * Sorts the elements in the range [first, last) using provided compare function.
 * This sort is stable. Uses OpenMP tasks for parallelism for larger parts.
 *
 * Based on MergeSort V3 from https://cw.fel.cvut.cz/old/_media/courses/b4m35pag/lab6_slides_advanced_openmp.pdf
 *
 * @param first First iterator
 * @param last Last iterator
 * @param comp Compare function
 */
template <typename It, typename Compare>
void merge_sort_parallel_a(It first, It last, Compare comp) {
#pragma omp parallel
#pragma omp single
    merge_sort_parallel_a_rec(first, last, comp);
}

/**
 * Sorts the elements in the range [first, last).
 * This sort is stable. Uses OpenMP tasks for parallelism for larger parts.
 *
 * Based on MergeSort V3 from https://cw.fel.cvut.cz/old/_media/courses/b4m35pag/lab6_slides_advanced_openmp.pdf
 *
 * @param first First iterator
 * @param last Last iterator
 */
template <typename It>
void merge_sort_parallel_a(It first, It last) {
    merge_sort_parallel_a(first, last, std::less<>());
}

/**
 * Split into two and recursively call then merge the two parts again. Uses binary sort for small parts.
 * Uses OpenMP tasks for parallelism for larger parts.
 *
 * Same as merge_sort_parallel_a_rec but uses gfx::timmerge for merging.
 *
 * Based on MergeSort V3 from https://cw.fel.cvut.cz/old/_media/courses/b4m35pag/lab6_slides_advanced_openmp.pdf
 *
 * @param left left iterator
 * @param right right iterator
 * @param comp compare function
 */
template <typename It, typename Compare>
void merge_sort_parallel_b_rec(It left, It right, Compare comp) {
    if (left < right) {
        if (right - left >= 32) {
            It mid = left + std::distance(left, right) / 2;
#pragma omp taskgroup
            {
#pragma omp task untied if(right - left >= (1<<14))
                merge_sort_parallel_b_rec(left, mid, comp);
#pragma omp task untied if(right - left >= (1<<14))
                merge_sort_parallel_b_rec(mid, right, comp);
#pragma omp taskyield
            }
            gfx::timmerge(left, mid, right, comp);
        } else {
            binary_insertion_sort(left, right, left, comp);
        }
    }
}

/**
 * Sorts the elements in the range [first, last) using provided compare function.
 * This sort is stable. Uses OpenMP tasks for parallelism for larger parts.
 *
 * Same as merge_sort_parallel_a but uses gfx::timmerge for merging.
 *
 * Based on MergeSort V3 from https://cw.fel.cvut.cz/old/_media/courses/b4m35pag/lab6_slides_advanced_openmp.pdf
 *
 * @param first First iterator
 * @param last Last iterator
 * @param comp Compare function
 */
template <typename It, typename Compare>
void merge_sort_parallel_b(It first, It last, Compare comp) {
#pragma omp parallel
#pragma omp single
    merge_sort_parallel_b_rec(first, last, comp);
}

/**
 * Sorts the elements in the range [first, last).
 * This sort is stable. Uses OpenMP tasks for parallelism for larger parts.
 *
 * Same as merge_sort_parallel_a but uses gfx::timmerge for merging.
 *
 * Based on MergeSort V3 from https://cw.fel.cvut.cz/old/_media/courses/b4m35pag/lab6_slides_advanced_openmp.pdf
 *
 * @param first First iterator
 * @param last Last iterator
 */
template <typename It>
void merge_sort_parallel_b(It first, It last) {
    merge_sort_parallel_b(first, last, std::less<>());
}