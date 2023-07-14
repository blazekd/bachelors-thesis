#pragma once

#include <vector>
#include <string>

/*
 * created with help of following links:
 * https://github.com/python/cpython/blob/bcb198385dee469d630a184182df9dc1463e2c47/Objects/listsort.txt
 * https://github.com/python/cpython/blob/bcb198385dee469d630a184182df9dc1463e2c47/Objects/listobject.c
 * https://web.archive.org/web/20150716000631/https://android.googlesource.com/platform/libcore/+/gingerbread/luni/src/main/java/java/util/TimSort.java
 * https://github.com/timsort/cpp-TimSort/blob/master/include/gfx/timsort.hpp
 */

/**
 * Helper struct to store information about run.
 * @tparam It any RandomAccessIterator
 */
template <class It>
struct Slice {
    /**
     * Iterator pointing to first element in slice
     */
    It index;
    /**
     * Length of slice
     */
    int length;
};

/**
 * Class that stores inner state of Timsort
 * @tparam It any RandomAccessIterator
 * @tparam Compare function used to compare 2 elements in array
 */
template <class It, class Compare>
class TimSort {
protected:
    typedef typename It::value_type T;
    /**
     * Stack of found runs
     */
    Slice<It> runs[85];
    /**
     * Length of stack runs
     */
    uint32_t runsLen = 0;
    /**
     * Temporary array used during merging.
     */
    std::vector<T> tmp_;

    /**
     * Iterator to currently processed element when finding runs
     */
    It index;
    /**
     * Iterator to first element in array to be sorted
     */
    It first;
    /**
     * Iterator to last element in array to be sorted
     */
    It last;
    /**
     * Function used to compare two values in array
     */
    Compare comp;
    /**
     * Total length of array
     */
    int length;
    /**
     * When during merging one run wins more times than this value it will start using galloping. Initialized to
     * 7 by default.
     */
    int minGallop = 7;
    /**
     * Minimal length of run.
     */
    int minRun;
    /**
     * Find range using exponential search in run ranging [_first, _first + len] so that key is inside that range.
     * That means key must be in range [_first + 2^(k-1), _first + 2^k]
     * and *(_first + 2^(k-1)) < key <= *(_first + 2^k) must be true.
     *
     * Then using binary search find exact place where to insert key into run so _first + k < key <= _first + k + 1 is true.
     *
     * @param _first first iterator of run
     * @param key searched value
     * @param len length of run
     * @param hint start on this index, 0 <= hint < len
     * @return index where key should be inserted into run to keep being sorted and stable
     */
    int gallop_left(const It & _first, T key, int len, int hint) {
        int lastOfs = 0;
        int ofs = 1;
        if (comp(*(_first + hint), key)) { // key > a[base + hint]
            // Gallop right until a[base+hint+lastOfs] < key <= a[base+hint+ofs]
            int maxOfs = len - hint;
            while (ofs < maxOfs && comp(*(_first + (hint + ofs)), key)) {
                lastOfs = ofs;
                ofs = (ofs << 1) + 1;
                if (ofs <= 0)   // int overflow
                    ofs = maxOfs;
            }
            if (ofs > maxOfs)
                ofs = maxOfs;
            lastOfs += hint;
            ofs += hint;
        } else { // key <= a[base + hint]
            // Gallop left until a[base+hint-ofs] < key <= a[base+hint-lastOfs]
            int maxOfs = hint + 1;
            while (ofs < maxOfs && !comp(*(_first + (hint - ofs)), key)) {
                lastOfs = ofs;
                ofs = (ofs << 1) + 1;
                if (ofs <= 0)   // int overflow
                    ofs = maxOfs;
            }
            if (ofs > maxOfs)
                ofs = maxOfs;
            int tmp = lastOfs;
            lastOfs = hint - ofs;
            ofs = hint - tmp;
        }
        /*
         * We now know that a[b + lastOfs] < key <= a[b + ofs] is true. Do a binary
         * search, to find exact position.
         */
        lastOfs++;
        // binary search
        It pos = std::lower_bound(_first + lastOfs, _first + ofs, key, comp);
        return pos - _first;
    }
    /**
     * Find range using exponential search in run ranging [_first, _first + len] so that key is inside that range.
     * That means key must be in range [_first + 2^(k-1), _first + 2^k]
     * and *(_first + 2^(k-1)) < key <= *(_first + 2^k) must be true.
     *
     * Then using binary search find exact place where to insert key into run so _first + k <= key < _first + k + 1 is true.
     *
     * @param _first first iterator of run
     * @param key searched value
     * @param len length of run
     * @param hint start on this index, 0 <= hint < len
     * @return index where key should be inserted into run to keep being sorted and stable
     */
    int gallop_right(const It & _first, T key, int len, int hint) {
        int ofs = 1;
        int lastOfs = 0;
        if (comp(key, *(_first + hint))) { // key > a[base + hint]
            // Gallop left until a[b+hint - ofs] <= key < a[b+hint - lastOfs]
            int maxOfs = hint + 1;
            while (ofs < maxOfs && comp(key, *(_first + (hint - ofs)))) {
                lastOfs = ofs;
                ofs = (ofs << 1) + 1;
                if (ofs <= 0)   // int overflow
                    ofs = maxOfs;
            }
            if (ofs > maxOfs)
                ofs = maxOfs;
            int tmp = lastOfs;
            lastOfs = hint - ofs;
            ofs = hint - tmp;
        } else { // a[b + hint] <= key
            // Gallop right until a[b+hint + lastOfs] <= key < a[b+hint + ofs]
            int maxOfs = len - hint;
            while (ofs < maxOfs && !comp(key, *(_first + (hint + ofs)))) {
                lastOfs = ofs;
                ofs = (ofs << 1) + 1;
                if (ofs <= 0)   // int overflow
                    ofs = maxOfs;
            }
            if (ofs > maxOfs)
                ofs = maxOfs;
            lastOfs += hint;
            ofs += hint;
        }
        /*
         * We now know that a[b + lastOfs] <= key < a[b + ofs] is true. Do a binary
         * search, to find exact position.
         */
        lastOfs++;
        // binary search
        It pos = std::upper_bound(_first + lastOfs, _first + ofs, key, comp);
        return pos - _first;
    }
    /**
     * Merge two runs - [base1, base1 + len] and [base2, base2 + len2] using temporary vector tmp. Use one pair at a
     * time merge and when one array starts winning consistently use galloping.
     * Go back to one pair at a time if galloping becomes ineffective.
     * Copy remaining when one array is empty.
     *
     * This function is more memory effective when len1 < len2
     * Starts merging from beginning
     *
     * @param base1 first iterator in first run
     * @param len1 length of first run
     * @param base2 first iterator in second run
     * @param len2 length of second run
     */
    void merge_lo(It base1, int len1, It base2, int len2) {
        tmp_.assign(std::make_move_iterator(base1), std::make_move_iterator(base1 + len1));

        It cursor1 = tmp_.begin();         // Indexes into tmp array
        It cursor2 = base2;               // Indexes into second run
        It dest = base1;                  // Indexes into result run

        int localMinGallop = minGallop;

        // Move first element of second run and deal with degenerate cases
        *(dest++) = std::move(*(cursor2++));
        if (--len2 == 0 || len1 == 1)
            goto copyEnd;

        while (true) {
            int count1 = 0; // Number of times in a row that first run won
            int count2 = 0; // Number of times in a row that second run won
            //Use stardard merge one pair at a time until one side starts winning consistently
            do {
                if (comp(*cursor2, *cursor1)) {
                    *(dest++) = std::move(*(cursor2++));
                    count2++;
                    count1 = 0;
                    if (--len2 == 0)
                        goto copyEnd;
                } else {
                    *(dest++) = std::move(*(cursor1++));
                    count1++;
                    count2 = 0;
                    if (--len1 == 1)
                        goto copyEnd;
                }
            } while ((count1 | count2) < localMinGallop);
            /*
             * One run is winning so consistently that galloping may be a
             * huge win. So try that, and continue galloping until (if ever)
             * neither run appears to be winning consistently anymore.
             */
            do {
                count1 = gallop_right(cursor1, *cursor2,  len1, 0);
                if (count1 != 0) {
                    std::move(cursor1, cursor1 + count1, dest);
                    dest += count1;
                    cursor1 += count1;
                    len1 -= count1;
                    if (len1 <= 1)
                        goto copyEnd;
                }
                *(dest++) = std::move(*(cursor2++));
                if (--len2 == 0)
                    goto copyEnd;
                count2 = gallop_left(cursor2, *cursor1, len2, 0);
                if (count2 != 0) {
                    std::move(cursor2, cursor2 + count2, dest);
                    dest += count2;
                    cursor2 += count2;
                    len2 -= count2;
                    if (len2 == 0)
                        goto copyEnd;
                }
                *(dest++) = std::move(*(cursor1++));
                if (--len1 == 1)
                    goto copyEnd;
                localMinGallop--;
            } while ((count1 >= localMinGallop) | (count2 >= localMinGallop));
            if (localMinGallop < 0)
                localMinGallop = 0;
            localMinGallop += 2;  // Penalize for leaving gallop mode
        }
        copyEnd:
        // copy remaining of run
        minGallop = localMinGallop;
        if (len1 == 1) {
            std::move(cursor2, cursor2 + len2, dest);
            *(dest + len2) = std::move(*cursor1); //  Last element of run 1 to end of merge
        } else {
            std::move(cursor1, cursor1 + len1, dest);
        }
    }
    /**
     * Merge two runs - [base1, base1 + len] and [base2, base2 + len2] using temporary vector tmp. Use one pair at a
     * time merge and when one array starts winning consistently use galloping.
     * Go back to one pair at a time if galloping becomes ineffective.
     * Copy remaining when one array is empty.
     *
     * This function is more memory effective when len1 > len2
     * Starts merging from end
     *
     * @param base1 first iterator in first run
     * @param len1 length of first run
     * @param base2 first iterator in second run
     * @param len2 length of second run
     */
    void merge_hi(It base1, int len1, It base2, int len2) {
        tmp_.assign(std::make_move_iterator(base2), std::make_move_iterator(base2 + len2));
        It cursor1 = base1 + len1;            // Indexes into tmp array
        It cursor2 = tmp_.begin() + (len2 - 1);      // Indexes into tmp array
        It dest = base2 + (len2 - 1);               // Indexes into a

        int localMinGallop = minGallop;

        // Move last element of first run and deal with degenerate cases
        *(dest--) = std::move(*(--cursor1));
        if (--len1 == 0 || len2 == 1)
            goto copyEnd;

        while (true) {
            int count1 = 0; // Number of times in a row that first run won
            int count2 = 0; // Number of times in a row that second run won
            //Use standard merge one pair at a time until one side starts winning consistently

            --cursor1;
            do {
                if (comp(*cursor2, *cursor1)) {
                    *(dest--) = std::move(*(cursor1));
                    count1++;
                    count2 = 0;
                    if (--len1 == 0)
                        goto copyEnd;
                    --cursor1;
                } else {
                    *(dest--) = std::move(*(cursor2--));
                    count2++;
                    count1 = 0;
                    if (--len2 == 1) {
                        ++cursor1;
                        goto copyEnd;
                    }
                }
            } while ((count1 | count2) < localMinGallop);
            ++cursor1;
            /*
             * One run is winning so consistently that galloping may be a
             * huge win. So try that, and continue galloping until (if ever)
             * neither run appears to be winning consistently anymore.
             */
            do {
                count1 = len1 - gallop_right(base1, *cursor2, len1, len1 - 1);
                if (count1 != 0) {
                    dest -= count1;
                    cursor1 -= count1;
                    len1 -= count1;
                    std::move_backward(cursor1, cursor1 + count1, dest + (1 + count1));
                    if (len1 == 0)
                        goto copyEnd;
                }
                *(dest--) = std::move(*(cursor2--));
                if (--len2 == 1)
                    goto copyEnd;
                count2 = len2 - gallop_left(tmp_.begin(), *std::prev(cursor1), len2, len2 - 1);
                if (count2 != 0) {
                    dest -= count2;
                    cursor2 -= count2;
                    len2 -= count2;
                    std::move(std::next(cursor2), cursor2 + (count2 + 1), dest + 1);
                    if (len2 <= 1)
                        goto copyEnd;
                }
                (*dest--) = std::move(*(--cursor1));
                if (--len1 == 0)
                    goto copyEnd;
                localMinGallop--;
            } while (count1 >= localMinGallop || count2 >= localMinGallop);
            if (localMinGallop < 0)
                localMinGallop = 0;
            localMinGallop += 2;  // Penalize for leaving gallop mode
        }
        copyEnd:
        // copy remaining of run
        minGallop = localMinGallop;
        if (len2 == 1) {
            dest -= len1;
            std::move_backward(cursor1 - len1, cursor1, dest + (1 + len1));
            *dest = std::move(*cursor2);  // Move first element of second run to front of merge
        } else {
            std::move(tmp_.begin(), tmp_.begin() + len2, dest - (len2 - 1));
        }
    }
    /**
     * Merge run i and i + 1. Find how many elements can be skipped at beginning of run1 and end of run2.
     * Based on remaining lengths of runs choose if merge_lo or merge_hi should be used
     * so that only min(len1, len2) extra memory is needed.
     *
     * @param i number of run that should be merged
     */
    void merge_at(uint32_t i) {
        It base1 = runs[i].index;
        int len1 = runs[i].length;
        It base2 = runs[i + 1].index;
        int len2 = runs[i + 1].length;

        /*
         * Record the length of the combined runs
         * if i is the 3rd-last move over the last run so there is no gap
         */
        runs[i].length = len1 + len2;
        if (i == runsLen - 3) {
            runs[i + 1] = runs[i + 2];
        }
        --runsLen;
        /*
         * Find where the first element of run2 goes in run1. Prior elements
         * are already in place.
         */
        int k = gallop_right(base1, *base2, len1, 0);

        base1 += k;
        len1 -= k;
        if (len1 == 0)
            return;
        /*
         * Find where the last element of run1 goes in run2. Subsequent elements
         * are already in place.
         */
        len2 = gallop_left(base2, *(base1 + (len1 - 1)),len2, len2 - 1);
        if (len2 == 0)
            return;
        // Merge remaining runs, using tmp array with min(len1, len2) elements
        if (len1 <= len2)
            merge_lo(base1, len1, base2, len2);
        else
            merge_hi(base1, len1, base2, len2);
    }
    /**
     * Sort array [lo, hi] using binary insertion sort.
     * Array [lo, start] must be already sorted
     *
     * @param lo lower bound iterator
     * @param hi upper bound iterator
     * @param start first unsorted element iterator. Must be lo <= start < hi
     */
    void binary_insertion_sort(It lo, It hi, It start) {
        if (start == lo) {
            ++start;
        }
        for (; start < hi; ++start) {
            T pivot = std::move(*start);
            It const pos = std::upper_bound(lo, start, pivot, comp);
            for (It p = start; p > pos; --p) {
                *p = std::move(*std::prev(p));
            }
            *pos = std::move(pivot);
        }
    }
    /**
     * If n < 64 return n.
     * If n >= 64 compute number between 32 and 64 return 32 if n is power of 2.
     * Else return k, 32 <= k <= 64, such that n/k is close to, but
     * strictly less than, an exact power of 2.
     *
     * @param n number of total elements
     */
    int compute_min_run(int n) {
        int r = 0;           /* becomes 1 if any 1 bits are shifted off */
        while (n >= 64) {
            r |= n & 1;
            n >>= 1;
        }
        return n + r;
    }
    /**
     * Find run that is at least minRun long and save it to stack.
     * If run is strictly descending, reverse it.
     * If run is too short, fill it until it is at least minRun long.
     * Start finding at BEGINNING of the array.
     */
    void find_run() {
        It idx = index + 1;

        if (idx != last) {
            if (comp(*idx, *(idx - 1))) { // strictly descending run
                do {
                    idx++;
                } while (idx != last && comp(*idx, *(idx - 1)));
                // make ascending
                int halfLen = std::distance(index, idx) / 2;
                for (int j = 0; j < halfLen; ++j) {
                    T tmp = std::move(*(index + j));
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
            binary_insertion_sort(index, hi, idx);
            idx = hi;
        }

        // save run to stack
        runs[runsLen++]= { index, (int)std::distance(index, idx) };
        index = idx;
    }
public:
    /**
     * Starts sorting process.
     * Find run and check if length of runs satisfy these invariants:
     * 1. invariant: len[-3] > len[-2] + len[-1]
     * 2. invariant: len[-2] > len[-1]
     * Start merging until both invariants are reestablished
     *
     * In the end merge remaining runs.
     */
    void sort() {
        while (index != last) {
            find_run();
            while (runsLen > 1) {
                int n = runsLen - 2;
                if ((n > 0 && runs[n - 1].length <= runs[n].length + runs[n + 1].length) ||
                    (n > 1 && runs[n - 2].length <= runs[n - 1].length + runs[n].length)) {
                    if (runs[n - 1].length < runs[n + 1].length)
                        --n;
                }
                else if (n < 0 || runs[n].length > runs[n + 1].length) {
                    break;
                }
                merge_at(n);
            }
        }

        // merge remaining
        while (runsLen > 1) {
            int n = runsLen - 2;
            if (n > 0 && runs[n - 1].length < runs[n + 1].length)
                --n;
            merge_at(n);
        }
    }
    TimSort(It _first, It _last, Compare _comp) {
        length = std::distance(_first, _last);
        minRun = compute_min_run(length);
        first = index = _first;
        last = _last;
        comp = _comp;
    }
};


/**
 * Sorts the elements in the range [first, last) using provided compare function.
 * This sort is stable and adaptive. Runs in O(n*log n) time and uses O(n) memory.
 *
 * @param first First iterator
 * @param last Last iterator
 * @param comp Compare function
 */
template <class It, class Compare>
void timsort(It first, It last, Compare comp) {
    TimSort<It, Compare> tim(first, last, comp);
    tim.sort();
}

/**
 * Sorts the elements in the range [first, last) in ascending order.
 * This sort is stable and adaptive. Runs in O(n*log n) time and uses O(n) memory.
 *
 * @param first First iterator
 * @param last Last iterator
 */
template <class It>
void timsort(It first, It last) {
    timsort(first, last, std::less<>());
}






