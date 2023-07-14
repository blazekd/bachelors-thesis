#pragma once

#include <algorithm>
#include <deque>
#include <iostream>

/*
 * HELPER FUNCTIONS
 */

/**
 * Keeps information about start and end of run
 * @tparam It any RandomAccessIterator
 */
template <class It>
struct MySlice {
    /**
     * Iterator pointing to first element in run
     */
    It start;
    /**
     * Iterator pointing to last element in run
     */
    It end;
    MySlice() = default;
    MySlice(It start, It end) : start(start), end(end) {}
};

/**
 * Sort array [lo, hi] using binary insertion sort.
 * Array [lo, start] must be already sorted
 * @param lo lower bound iterator
 * @param hi upper bound iterator
 * @param start first unsorted element iterator. Must be lo <= start < hi
 * @param comp compare function
 */
template <class It, class Compare>
void binary_insertion_sort(It lo, It hi, It start, Compare comp) {
    while (start != hi) {
        //binary search
        It left = std::upper_bound(lo, start, *start, comp);
        It k = start++;
        //move elements
        while (k != left) {
            typename It::value_type tmp = *k;
            *k = *(k - 1);
            *(k - 1) = tmp;
            --k;
        }
    }
}

/**
 * Sort array [lo, hi] using binary insertion sort.
 * Array [sorted, hi] must be already sorted
 * @param lo lower bound iterator
 * @param hi upper bound iterator
 * @param sorted first sorted element iterator. Must be lo <= sorted <= hi
 * @param comp compare function
 */
template <class It, class Compare>
void binary_insertion_sort_end(It lo, It hi, It sorted, Compare comp) {
    typedef typename It::value_type T;

    if (sorted == hi) {
        --sorted;
    }
    for (; lo < sorted; --sorted) {
        T pivot = std::move(*(sorted - 1));

        It const pos = std::lower_bound(sorted, hi, pivot, comp);
        for (It p = sorted - 1; p < pos - 1; ++p) {
            *p = std::move(*std::next(p));
        }
        *(pos - 1) = std::move(pivot);
    }
}

/**
 * If n < 64 return n.
 * If n >= 64 compute number between 32 and 64 return 32 if n is power of 2.
 * Else return k, 32 <= k <= 64, such that n/k is close to, but
 * strictly less than, an exact power of 2.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 */
template <typename It>
int calculate_min_run(It first, It last) {
    int n = std::distance(first, last);
    int r = 0;           /* becomes 1 if any 1 bits are shifted off */
    while (n >= 64) {
        r |= n & 1;
        n >>= 1;
    }
    return n + r;
}

/**
 * Check if array is smaller than limit and if so sort it using binary insertion sort.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 * @param length current size of array
 * @param limit max size of array to sort it
 * @return true if it sorted else false
 */
template <typename It, typename Compare>
bool check_small_and_sort(It first, It last, Compare comp, int length, int limit) {
    if (length <= limit) {
        binary_insertion_sort(first, last, first, comp);
        return true;
    }
    return false;
}

/**
 * Check if array is smaller than limit and if so sort it using binary insertion sort.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 * @param limit max size of array to sort it
 * @return true if it sorted else false
 */
template <typename It, typename Compare>
bool check_small_and_sort(It first, It last, Compare comp, int limit) {
    return check_small_and_sort(first, last, comp, std::distance(first, last), limit);
}

/**
 * Find ALL runs that is at least minRun long and save it to stack.
 * If run is strictly descending, reverse it.
 * If run is too short, fill it until it is at least minRun long.
 * Start finding at END of the array.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 * @return deque<MySlice<It>> of found runs
 */
template <typename It, typename Compare>
std::deque<MySlice<It>> find_run_rev(It first, It last, Compare comp) {
    int minRun = calculate_min_run(first, last);

    if (check_small_and_sort(first, last, comp, minRun))
        return {};

    std::deque<MySlice<It>> runs;
    It index = last;

    while (index != first) {
        It idx = index - 1;
        if (comp(*idx, *(idx - 1))) { // strictly descending run
            do {
                idx--;
            } while (idx != first && comp(*idx, *(idx - 1)));
            // make ascending
            int halfLen = std::distance(idx, index) / 2;
            for (int j = 0; j < halfLen; ++j) {
                typename It::value_type tmp = std::move(*(idx + j));
                *(idx + j) = std::move(*(index - (j + 1)));
                *(index - (j + 1)) = std::move(tmp);
            }
        } else { // ascending run
            do {
                idx--;
            } while (idx != first && comp(*(idx - 1), *idx));
        }
        //doplnit na minRun
        int force = std::distance(idx, index);
        if (force < minRun) {
            It lo = std::distance(first, index) < minRun ? first : index - minRun;
            binary_insertion_sort_end(lo, index, idx, comp);
            idx = lo;
        }

        runs.push_back(MySlice(idx, index));
        index = idx;
    }
    return runs;
}

/**
 * Find ALL runs that is at least minRun long and save it to stack.
 * If run is strictly descending, reverse it.
 * If run is too short, fill it until it is at least minRun long.
 * Start finding at BEGINNING of the array.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 * @return deque<MySlice<It>> of found runs
 */
template <typename It, typename Compare>
std::deque<MySlice<It>> find_run(It first, It last, Compare comp) {
    int minRun = calculate_min_run(first, last);

    if (check_small_and_sort(first, last, comp, minRun))
        return {};

    std::deque<MySlice<It>> runs;

    It index = first;
    while (index != last) {
        It idx = index + 1;
        if (idx != last) {
            if (comp(*idx, *(idx - 1))) { // strictly descending run
                do {
                    idx++;
                } while (idx != last && comp(*idx, *(idx - 1)));
                // make ascending
                int halfLen = std::distance(index, idx) / 2;
                for (int j = 0; j < halfLen; ++j) {
                    typename It::value_type tmp = std::move(*(index + j));
                    *(index + j) = std::move(*(idx - (j + 1)));
                    *(idx - (j + 1)) = std::move(tmp);
                }
            } else { // ascending run
                do {
                    idx++;
                } while (idx != last && comp(*(idx - 1), *idx));
            }
        }
        int force = std::distance(index, idx);
        if (force < minRun) {
            It hi = std::distance(idx, last) < minRun ? last : index + minRun;
            binary_insertion_sort(index, hi, idx, comp);
            idx = hi;
        }

        // save run to stack
        runs.push_front(MySlice(index, idx));
        index = idx;
    }

    return runs;
}

/*
 * MERGE FUNCTIONS
 */

/**
 * Merge two SORTED arrays [first1, last1) and [first2, last2) and save to dest
 *
 * @param first1 iterator to first element of first array
 * @param last1 iterator to last element of first array
 * @param first2 iterator to first element of second array
 * @param last2 iterator to last element of second array
 * @param dest iterator to destination array
 * @param comp compare function
 */
template <class It, class Compare>
void merge_2(It first1, It last1, It first2, It last2, It dest, Compare comp) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first2, *first1))
            *(dest++) = *(first2++);
        else
            *(dest++) = *(first1++);
    }
    // Copy any remaining elements of the first run
    while (first1 != last1)
        *(dest++) = *(first1++);
    // Copy any remaining elements of the second run
    while (first2 != last2)
        *(dest++) = *(first2++);
}

/**
 * Merge three SORTED arrays [first1, last1), [first2, last2) and [first3, last3)
 * and save to dest
 * Scans all 3 arrays to determine the minimum
 * 
 * @param first1 iterator to first element of first array
 * @param last1 iterator to last element of first array
 * @param first2 iterator to first element of second array
 * @param last2 iterator to last element of second array
 * @param first3 iterator to first element of third array
 * @param last3 iterator to last element of third array
 * @param dest iterator to destination array
 * @param comp compare function
 */
template <class It, class Compare>
void merge_3(It first1, It last1, It first2, It last2, It first3, It last3, It dest, Compare comp) {
    // Merge the three runs into the temporary array
    while (first1 != last1 && first2 != last2 && first3 != last3) {
        if (comp(*first2, *first1)) {
            if (comp(*first3, *first2))
                *(dest++) = *(first3++);
            else
                *(dest++) = *(first2++);
        } else {
            if (comp(*first3, *first1))
                *(dest++) = *(first3++);
            else
                *(dest++) = *(first1++);
        }
    }

    if (first1 == last1)
        merge_2(first2, last2, first3, last3, dest, comp);

    if (first2 == last2)
        merge_2(first1, last1, first3, last3, dest, comp);

    if (first3 == last3)
        merge_2(first1, last1, first2, last2, dest, comp);
}

/**
 * Merge four SORTED arrays [first1, last1), [first2, last2), [first3, last3)
 * and [first4, last4) and save to first1. That means [first1, last4) must be continuous.
 * First merge [first1, last1) with [first2, last2) and [first3, last3) with
 * [first4, last4) to tmp array and then merge these two arrays into first1. 
 * 
 * @param first1 iterator to first element of first array
 * @param last1 iterator to last element of first array
 * @param first2 iterator to first element of second array
 * @param last2 iterator to last element of second array
 * @param first3 iterator to first element of third array
 * @param last3 iterator to last element of third array
 * @param first4 iterator to first element of fourth array
 * @param last4 iterator to last element of fourth array
 * @param comp compare function
 */
template <class It, class Compare>
void merge_4a(It first1, It last1, It first2, It last2, It first3, It last3, It first4, It last4, Compare comp) {
    std::vector<typename It::value_type> tmp(std::distance(first1, last4));

    It first = tmp.begin();
    It half = std::next(tmp.begin(), std::distance(first1, first3));
    It dest = tmp.begin();
    It finalDest = first1;
    It mid = half;
    It last = tmp.end();

    while (first1 != last1 && first2 != last2) {
        if (comp(*first2, *first1))
            *(dest++) = *(first2++);
        else
            *(dest++) = *(first1++);
    }

    // Copy any remaining elements of the first run
    while (first1 != last1)
        *(dest++) = *(first1++);

    // Copy any remaining elements of the second run
    while (first2 != last2)
        *(dest++) = *(first2++);

    while (first3 != last3 && first4 != last4) {
        if (comp(*first4, *first3))
            *(dest++) = *(first4++);
        else
            *(dest++) = *(first3++);
    }

    // Copy any remaining elements of the first run
    while (first3 != last3)
        *(dest++) = *(first3++);

    // Copy any remaining elements of the second run
    while (first4 != last4)
        *(dest++) = *(first4++);


    while (first != mid && half != last) {
        if (comp(*half, *first))
            *(finalDest++) = *(half++);
        else
            *(finalDest++) = *(first++);
    }

    // Copy any remaining elements of the first run
    while (first != mid)
        *(finalDest++) = *(first++);

    // Copy any remaining elements of the second run
    while (half != last)
        *(finalDest++) = *(half++);

}

/**
 * Merge four SORTED arrays [first1, last1), [first2, last2), [first3, last3)
 * and [first4, last4) and save to dest
 * Scans all 4 arrays to determine the minimum
 * 
 * @param first1 iterator to first element of first array
 * @param last1 iterator to last element of first array
 * @param first2 iterator to first element of second array
 * @param last2 iterator to last element of second array
 * @param first3 iterator to first element of third array
 * @param last3 iterator to last element of third array
 * @param first4 iterator to first element of fourth array
 * @param last4 iterator to last element of fourth array
 * @param dest iterator to destination array
 * @param comp compare function
 */
template <class It, class Compare>
void merge_4b(It first1, It last1, It first2, It last2, It first3, It last3, It first4, It last4, It dest, Compare comp) {
    while (first1 != last1 && first2 != last2 && first3 != last3 && first4 != last4) {
        if (comp(*first2, *first1)) {
            if (comp(*first3, *first2)) {
                if (comp(*first4, *first3))
                    *(dest++) = *(first4++);
                else
                    *(dest++) = *(first3++);
            }
            else {
                if (comp(*first4, *first2))
                    *(dest++) = *(first4++);
                else
                    *(dest++) = *(first2++);
            }
        } else {
            if (comp(*first3, *first1)) {
                if (comp(*first4, *first3))
                    *(dest++) = *(first4++);
                else
                    *(dest++) = *(first3++);
            }
            else {
                if (comp(*first4, *first1))
                    *(dest++) = *(first4++);
                else
                    *(dest++) = *(first1++);
            }
        }
    }

    if (first1 == last1)
        merge_3(first2, last2, first3, last3, first4, last4, dest, comp);
    if (first2 == last2)
        merge_3(first1, last1, first3, last3, first4, last4, dest, comp);

    if (first3 == last3)
        merge_3(first1, last1, first2, last2,first4, last4, dest, comp);

    if (first4 == last4)
        merge_3(first1, last1, first2, last2, first3, last3, dest, comp);
}


/**
 * Merge runs with following strategy: 1 with 2, 3 with 4, ...
 * Repeat until there is only 1 run left.
 * 
 * @param runs 
 * @param comp compare function
 */
template <typename It, typename Compare>
void merge_runs_final(std::deque<MySlice<It>> & runs, Compare comp) {
    while (runs.size() > 1) {
        int remainder = runs.size() % 2;
        int size = runs.size() / 2;
        for (int i = 0; i < size; ++i) {
            MySlice x = runs.front();
            runs.pop_front();
            MySlice xx = runs.front();
            runs.pop_front();
            It a = xx.start;
            It b = xx.end;
            It c = x.end;
            std::vector<typename It::value_type> tmp(std::distance(a, c));
            merge_2(a, b, b, c, tmp.begin(), comp);
            x.start = xx.start;
            std::move(tmp.begin(), tmp.end(), x.start);
            runs.push_back(x);
        }

        for (int i = 0; i < remainder; ++i) {
            runs.push_back(runs.front());
            runs.pop_front();
        }
    }
}


/*
 * SORTS
 */

/**
 * Sorts [first, last) recursively splitting into two. Uses merge_2 to merge.
 * 
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 */
template <class It, class Compare>
void merge_2_sort(It first, It last, Compare comp) {
    int64_t size = std::distance(first, last);
    if (check_small_and_sort(first, last, comp, size, 32))
        return;
    It mid = std::next(first, size / 2);
    merge_2_sort(first, mid, comp);
    merge_2_sort(mid, last, comp);
    std::vector<typename It::value_type> tmp(std::distance(first, last));
    merge_2(first, mid, mid, last, tmp.begin(), comp);

    std::move(tmp.begin(), tmp.end(), first);
}

/**
 * Sorts [first, last) recursively splitting into two. Uses merge_2 to merge.
 * 
 * @param first iterator to first element
 * @param last iterator to last element
 */
template <class It>
void merge_2_sort(It first, It last) {
    merge_2_sort(first, last, std::less<>());
}

/**
 * Sorts [first, last). Finds all runs and then merge them two at a time. 
 * Uses merge_2 to merge.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 */
template <class It, class Compare>
void it_merge_2_sort(It first, It last, Compare comp) {
    std::deque<MySlice<It>> runs = find_run(first, last, comp);

    merge_runs_final(runs, comp);
}

/**
 * Sorts [first, last). Finds all runs and then merge them two at a time. 
 * Uses merge_2 to merge.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 */
template <class It>
void it_merge_2_sort(It first, It last) {
    it_merge_2_sort(first, last, std::less<>());
}

/**
 * Sorts [first, last). Finds all runs starting from END and then merge them two at a time. 
 * Uses merge_2 to merge.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 */
template <class It, class Compare>
void it_merge_2_sort_rev(It first, It last, Compare comp) {
    std::deque<MySlice<It>> runs = find_run_rev(first, last, comp);

    merge_runs_final(runs, comp);
}

/**
 * Sorts [first, last). Finds all runs starting from END and then merge them two at a time. 
 * Uses merge_2 to merge.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 */
template <class It>
void it_merge_2_sort_rev(It first, It last) {
    it_merge_2_sort_rev(first, last, std::less<>());
}

/**
 * Sorts [first, last) recursively splitting into three. Uses merge_3 to merge.
 * 
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 */
template <class It, class Compare>
void merge_3_sort(It first, It last, Compare comp) {
    int64_t size = std::distance(first, last);
    if (check_small_and_sort(first, last, comp, size, 32))
        return;

    It mid1 = std::next(first, size / 3);
    It mid2 = std::next(first, 2*(size / 3));
    merge_3_sort(first, mid1, comp);
    merge_3_sort(mid1, mid2, comp);
    merge_3_sort(mid2, last, comp);
    std::vector<typename It::value_type> tmp(std::distance(first, last));
    merge_3(first, mid1, mid1, mid2, mid2, last, tmp.begin(), comp);
    std::move(tmp.begin(), tmp.end(), first);
}

/**
 * Sorts [first, last) recursively splitting into three. Uses merge_3 to merge.
 * 
 * @param first iterator to first element
 * @param last iterator to last element
 */
template <class It>
void merge_3_sort(It first, It last) {
    merge_3_sort(first, last, std::less<>());
}

/**
 * Sorts [first, last). Finds all runs and then merge them three at a time. 
 * Uses merge_3 to merge.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 */
template <class It, class Compare>
void it_merge_3_sort(It first, It last, Compare comp) {
    std::deque<MySlice<It>> runs = find_run(first, last, comp);

    while (runs.size() > 2) {
        int size = runs.size() / 3;
        int remainder = runs.size() % 3;

        for (int i = 0; i < size; ++i) {
            MySlice x = runs.front();
            runs.pop_front();
            MySlice xx = runs.front();
            runs.pop_front();
            MySlice xxx = runs.front();
            runs.pop_front();
            It first1 = xxx.start;
            It mid1 = xxx.end;
            It mid2 = xx.end;
            It last3 = x.end;
            std::vector<typename It::value_type> tmp(std::distance(first1, last3));
            merge_3(first1, mid1, mid1, mid2, mid2, last3, tmp.begin(), comp);
            x.start = xxx.start;
            std::move(tmp.begin(), tmp.end(), x.start);
            runs.push_back(x);
        }

        for (int i = 0; i < remainder; ++i) {
            runs.push_back(runs.front());
            runs.pop_front();
        }
    }

    merge_runs_final(runs, comp);
}

/**
 * Sorts [first, last). Finds all runs and then merge them three at a time. 
 * Uses merge_3 to merge.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 */
template <class It>
void it_merge_3_sort(It first, It last) {
    it_merge_3_sort(first, last, std::less<>());
}

/**
 * Sorts [first, last) recursively splitting into four. Uses merge_4a to merge.
 * 
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 */
template <class It, class Compare>
void merge_4a_sort(It first, It last, Compare comp) {
    int64_t size = std::distance(first, last);
    if (check_small_and_sort(first, last, comp, size, 32))
        return;

    int quarter = size / 4;

    It quarter1 = std::next(first, quarter);
    It quarter2 = std::next(quarter1, quarter);
    It quarter3 = std::next(quarter2, quarter);

    merge_4a_sort(first, quarter1, comp);
    merge_4a_sort(quarter1, quarter2, comp);
    merge_4a_sort(quarter2, quarter3, comp);
    merge_4a_sort(quarter3, last, comp);
    merge_4a(first, quarter1, quarter1, quarter2, quarter2, quarter3, quarter3, last, comp);
}

/**
 * Sorts [first, last) recursively splitting into four. Uses merge_4a to merge.
 * 
 * @param first iterator to first element
 * @param last iterator to last element
 */
template <class It>
void merge_4a_sort(It first, It last) {
    merge_4a_sort(first, last, std::less<>());
}

/**
 * Sorts [first, last). Finds all runs and then merge them four at a time. 
 * Uses merge_4a to merge.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 */
template <class It, class Compare>
void it_merge_4a_sort(It first, It last, Compare comp) {
    std::deque<MySlice<It>> runs = find_run(first, last, comp);

    while (runs.size() > 3) {
        int remainder = runs.size() % 4;
        int size = runs.size() / 4;
        for (int i = 0; i < size; ++i) {
            MySlice x = runs.front();
            runs.pop_front();
            MySlice xx = runs.front();
            runs.pop_front();
            MySlice xxx = runs.front();
            runs.pop_front();
            MySlice xxxx = runs.front();
            runs.pop_front();
            It first1 = xxxx.start;
            It quarter1 = xxxx.end;
            It quarter2 = xxx.end;
            It quarter3 = xx.end;
            It last4 = x.end;

            merge_4a(first1, quarter1, quarter1, quarter2, quarter2, quarter3, quarter3, last4, comp);
            x.start = xxxx.start;

            runs.push_back(x);
        }

        for (int i = 0; i < remainder; ++i) {
            runs.push_back(runs.front());
            runs.pop_front();
        }

    }

    merge_runs_final(runs, comp);
}

/**
 * Sorts [first, last). Finds all runs and then merge them four at a time. 
 * Uses merge_4a to merge.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 */
template <class It>
void it_merge_4a_sort(It first, It last) {
    it_merge_4a_sort(first, last, std::less<>());
}

/**
 * Sorts [first, last) recursively splitting into four. Uses merge_4b to merge.
 * 
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 */
template <class It, class Compare>
void merge_4b_sort(It first, It last, Compare comp) {
    int64_t size = std::distance(first, last);
    if (check_small_and_sort(first, last, comp, size, 32))
        return;

    int quarter = size / 4;

    It quarter1 = std::next(first, quarter);
    It quarter2 = std::next(quarter1, quarter);
    It quarter3 = std::next(quarter2, quarter);

    merge_4b_sort(first, quarter1, comp);
    merge_4b_sort(quarter1, quarter2, comp);
    merge_4b_sort(quarter2, quarter3, comp);
    merge_4b_sort(quarter3, last, comp);
    std::vector<typename It::value_type> tmp(std::distance(first, last));
    merge_4b(first, quarter1, quarter1, quarter2, quarter2, quarter3, quarter3, last, tmp.begin(), comp);
    std::move(tmp.begin(), tmp.end(), first);
}

/**
 * Sorts [first, last) recursively splitting into four. Uses merge_4b to merge.
 * 
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 */
template <class It>
void merge_4b_sort(It first, It last) {
    merge_4b_sort(first, last, std::less<>());
}

/**
 * Sorts [first, last). Finds all runs and then merge them four at a time. 
 * Uses merge_4b to merge.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 * @param comp compare function
 */
template <class It, class Compare>
void it_merge_4b_sort(It first, It last, Compare comp) {
    std::deque<MySlice<It>> runs = find_run(first, last, comp);

    while (runs.size() > 3) {
        int remainder = runs.size() % 4;
        int size = runs.size() / 4;
        for (int i = 0; i < size; ++i) {
            MySlice x = runs.front();
            runs.pop_front();
            MySlice xx = runs.front();
            runs.pop_front();
            MySlice xxx = runs.front();
            runs.pop_front();
            MySlice xxxx = runs.front();
            runs.pop_front();
            It first1 = xxxx.start;
            It quarter1 = xxxx.end;
            It quarter2 = xxx.end;
            It quarter3 = xx.end;
            It last4 = x.end;

            std::vector<typename It::value_type> tmp(std::distance(first1, last4));
            merge_4b(first1, quarter1, quarter1, quarter2, quarter2, quarter3, quarter3, last4, tmp.begin(), comp);
            x.start = xxxx.start;
            std::move(tmp.begin(), tmp.end(), x.start);
            runs.push_back(x);
        }

        for (int i = 0; i < remainder; ++i) {
            runs.push_back(runs.front());
            runs.pop_front();
        }
    }

    merge_runs_final(runs, comp);
}

/**
 * Sorts [first, last). Finds all runs and then merge them four at a time. 
 * Uses merge_4b to merge.
 *
 * @param first iterator to first element
 * @param last iterator to last element
 */
template <class It>
void it_merge_4b_sort(It first, It last) {
    it_merge_4b_sort(first, last, std::less<>());
}
