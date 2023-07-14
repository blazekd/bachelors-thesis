#include "test_helper.hpp"
#include "merging.hpp"
#include "timsort.hpp"
#include "mergesort_parallel.hpp"
#include "gfxtimsort.hpp"
#include "timsort_parallel_a.hpp"
#include "timsort_parallel_b.hpp"
#include "timsort_reverse.hpp"
#include <unistd.h>
#include <cstring>
#include <omp.h>

int numberOfTests = 1;
uint32_t numberOfThreads = 99999;

void print_supported_algorithms() {
    std::cout << "Supported sorting algorithms are:" << std::endl << std::endl;

    std::cout << "std::stable_sort" << std::endl;
    std::cout << "std::sort" << std::endl;
    std::cout << "merge_2_sort" << std::endl;
    std::cout << "it_merge_2_sort" << std::endl;
    std::cout << "it_merge_2_sort_rev" << std::endl;
    std::cout << "merge_3_sort" << std::endl;
    std::cout << "it_merge_3_sort" << std::endl;
    std::cout << "merge_4a_sort" << std::endl;
    std::cout << "it_merge_4a_sort" << std::endl;
    std::cout << "merge_4b_sort" << std::endl;
    std::cout << "it_merge_4b_sort" << std::endl;
    std::cout << "timsort" << std::endl;
    std::cout << "timsort_rev" << std::endl;
    std::cout << "gfx::timsort" << std::endl;
    std::cout << "timsort_parallel_a" << std::endl;
    std::cout << "timsort_parallel_b" << std::endl;
    std::cout << "merge_sort_parallel_a" << std::endl;
    std::cout << "merge_sort_parallel_b" << std::endl;
}


// load test files and test algorithms
int main(int argc, char* argv[]) {
    bool append = false;
    std::string outFile = "test/output.csv";
    bool testAll = true;
    bool testStableSort = false;
    bool testSort = false;
    bool testMerge2Sort = false;
    bool testItMerge2Sort = false;
    bool testItMerge2SortRev = false;
    bool testMerge3Sort = false;
    bool testItMerge3Sort = false;
    bool testMerge4aSort = false;
    bool testItMerge4aSort = false;
    bool testMerge4bSort = false;
    bool testItMerge4bSort = false;
    bool testTimSort = false;
    bool testTimSortRev = false;
    bool testGfxTimSort = false;
    bool testTimSortParallelA = false;
    bool testTimSortParallelB = false;
    bool testMergeSortParallelA = false;
    bool testMergeSortParallelB = false;
    
    while (true) {
        switch(getopt(argc, argv, "ac:n:o:t:h")) {
            case 'a':
                append = true;
                continue;
            case 'c':
                if (strtol(optarg, nullptr, 10) > 0)
                    numberOfThreads = (uint32_t)strtol(optarg, nullptr, 10);
                else {
                    std::cout << "Parameter -c must be positive number." << std::endl;
                    return 1;
                }
                continue;
            case 'n':
                if (strtol(optarg, nullptr, 10) > 0)
                    numberOfTests = (int)strtol(optarg, nullptr, 10);
                else {
                    std::cout << "Parameter -n must be positive number." << std::endl;
                    return 1;
                }
                continue;
            case 'o':
                outFile = optarg;
                continue;
            case 't':
                testAll = false;
                if (strcmp(optarg, "std::stable_sort") == 0)
                    testStableSort = true;
                else if (strcmp(optarg, "std::sort") == 0)
                    testSort = true;
                else if (strcmp(optarg, "merge_2_sort") == 0)
                    testMerge2Sort = true;
                else if (strcmp(optarg, "it_merge_2_sort") == 0)
                    testItMerge2Sort = true;
                else if (strcmp(optarg, "it_merge_2_sort_rev") == 0)
                    testItMerge2SortRev = true;
                else if (strcmp(optarg, "merge_3_sort") == 0)
                    testMerge3Sort = true;
                else if (strcmp(optarg, "it_merge_3_sort") == 0)
                    testItMerge3Sort = true;
                else if (strcmp(optarg, "merge_4a_sort") == 0)
                    testMerge4aSort = true;
                else if (strcmp(optarg, "it_merge_4a_sort") == 0)
                    testItMerge4aSort = true;
                else if (strcmp(optarg, "merge_4b_sort") == 0)
                    testMerge4bSort = true;
                else if (strcmp(optarg, "it_merge_4b_sort") == 0)
                    testItMerge4bSort = true;
                else if (strcmp(optarg, "timsort") == 0)
                    testTimSort = true;
                else if (strcmp(optarg, "timsort_rev") == 0)
                    testTimSortRev = true;
                else if (strcmp(optarg, "gfx::timsort") == 0)
                    testGfxTimSort = true;
                else if (strcmp(optarg, "timsort_parallel_a") == 0)
                    testTimSortParallelA = true;
                else if (strcmp(optarg, "timsort_parallel_b") == 0)
                    testTimSortParallelB = true;
                else if (strcmp(optarg, "merge_sort_parallel_a") == 0)
                    testMergeSortParallelA = true;
                else if (strcmp(optarg, "merge_sort_parallel_b") == 0)
                    testMergeSortParallelB = true;
                else {
                    std::cout << "Unknown sorting algorithm for argument -t: " << optarg << std::endl;
                    print_supported_algorithms();
                    return 1;
                }

                continue;
            case 'h':
            default :
                std::cout << "Arguments:" << std::endl;
                std::cout << "-a - appends to output file" << std::endl;
                std::cout << "-c - maximum number of threads for parallel algorithms" << std::endl;
                std::cout << "-n - number of test repetitions" << std::endl;
                std::cout << "-o - output file" << std::endl;
                std::cout << "-t - test only specific algorithm, can be specified multiple times" << std::endl;
                print_supported_algorithms();
                return 0;

            case -1:
                break;
        }

        break;
    }

    std::ofstream output;
    if (append)
        output.open(outFile, std::ios::app);
    else {
        output.open(outFile);
        output << "Test file;algorithm;less;my_compare;slow_compare;sorted;stable;compare count" << std::endl;
    }
    output << std::boolalpha;

    auto tests = load_tests();
    omp_set_num_threads((int)std::min(numberOfThreads, std::thread::hardware_concurrency()));
    if (testStableSort || testAll)
        FULLTESTWRAPPER(std::stable_sort, tests, output)
    if (testSort || testAll)
        FULLTESTWRAPPER(std::sort, tests, output)
    if (testMerge2Sort || testAll)
        FULLTESTWRAPPER(merge_2_sort, tests, output)
    if (testItMerge2Sort || testAll)
        FULLTESTWRAPPER(it_merge_2_sort, tests, output)
    if (testItMerge2SortRev || testAll)
        FULLTESTWRAPPER(it_merge_2_sort_rev, tests, output)
    if (testMerge3Sort || testAll)
        FULLTESTWRAPPER(merge_3_sort, tests, output)
    if (testItMerge3Sort || testAll)
        FULLTESTWRAPPER(it_merge_3_sort, tests, output)
    if (testMerge4aSort || testAll)
        FULLTESTWRAPPER(merge_4a_sort, tests, output)
    if (testItMerge4aSort || testAll)
        FULLTESTWRAPPER(it_merge_4a_sort, tests, output)
    if (testMerge4bSort || testAll)
        FULLTESTWRAPPER(merge_4b_sort, tests, output)
    if (testItMerge4bSort || testAll)
        FULLTESTWRAPPER(it_merge_4b_sort, tests, output)
    if (testTimSort || testAll)
        FULLTESTWRAPPER(timsort, tests, output)
    if (testTimSortRev || testAll)
        FULLTESTWRAPPER(timsort_rev, tests, output)
    if (testGfxTimSort || testAll)
        FULLTESTWRAPPER(gfx::timsort, tests, output)
    if (testTimSortParallelA || testAll)
        FULLTESTWRAPPER(timsort_parallel_a, tests, output)
    if (testTimSortParallelB || testAll)
        FULLTESTWRAPPER(timsort_parallel_b, tests, output)
    if (testMergeSortParallelA || testAll)
        FULLTESTWRAPPER(merge_sort_parallel_a, tests, output)
    if (testMergeSortParallelB || testAll)
        FULLTESTWRAPPER(merge_sort_parallel_b, tests, output)

    output.close();
}