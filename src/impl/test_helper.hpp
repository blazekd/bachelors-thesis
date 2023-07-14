#pragma once

#include <filesystem>
#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include "test_classes.hpp"

//checks if sort is stable
#define check_stable(sort, is_stable) { \
                        std::vector<StableTestClass> myTest = load_stable("test/stable.in"); \
                        std::vector<StableTestClass> correct = load_stable("test/stable.in"); \
                        std::stable_sort(correct.begin(), correct.end(), stable_comp1); \
                        sort(myTest.begin(), myTest.end(), stable_comp1); \
                        std::stable_sort(correct.begin(), correct.end(), stable_comp2); \
                        sort(myTest.begin(), myTest.end(), stable_comp2); \
                        std::stable_sort(correct.begin(), correct.end(), stable_comp3); \
                        sort(myTest.begin(), myTest.end(), stable_comp3); \
                        if (myTest.size() != correct.size()) \
                            is_stable = false; \
                        for (uint64_t i = 0; i < myTest.size() && is_stable; ++i) { \
                            if (myTest[i].a != correct[i].a || myTest[i].b != correct[i].b || myTest[i].c != correct[i].c) \
                                is_stable = false; \
                        } \
                    }

//measure time of sorting, and check if is sorted
#define TEST(sort, output, load_func, sorted) { \
                        uint64_t result = 0; \
                        for (int i = 0; i < numberOfTests; ++i) { \
                            auto vect = load_func(test.fileName); \
                            auto start = std::chrono::high_resolution_clock::now(); \
                            sort(vect.begin(), vect.end()); \
                            auto stop = std::chrono::high_resolution_clock::now();  \
                            result += std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count(); \
                            sorted = sorted & std::is_sorted(vect.begin(), vect.end()); \
                        } \
                        output << ";" << (result/numberOfTests); \
                    }
//measure time of sorting with custom sorting function and check if is sorted
#define TESTC(sort, compare, output, load_func, sorted) { \
                        uint64_t result = 0; \
                        for (int i = 0; i < numberOfTests; ++i) { \
                            auto vect = load_func(test.fileName); \
                            auto start = std::chrono::high_resolution_clock::now(); \
                            sort(vect.begin(), vect.end(), compare); \
                            auto stop = std::chrono::high_resolution_clock::now();  \
                            result += std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count(); \
                            sorted = sorted & std::is_sorted(vect.begin(), vect.end(), compare); \
                        } \
                        output << ";" << (result/numberOfTests); \
                    }

//run test for all compare functions and check if is stable
#define FULLTEST(fileName, sort, output, type, load_func) \
                        output << fileName << ";" << #sort; \
                        compareCount = 0; \
                        bool sorted = true; \
                        TEST(sort, output, load_func, sorted) \
                        TESTC(sort, my_compare<type>, output, load_func, sorted) \
                        TESTC(sort, slow_compare<type>, output, load_func, sorted) \
                        { \
                           auto vect = load_func(fileName); \
                           sort(vect.begin(), vect.end(), count_compare<type>); \
                        } \
                        bool stable = true; \
                        check_stable(sort, stable) \
                        output << ";" << sorted << ";" << stable << ";" << compareCount << std::endl;


// choose correct type and load_function based on test.type
#define FULLTESTWRAPPER(sort, tests, output) \
                        for (const auto& test: tests) { \
                            switch (test.type) { \
                                case NUMBER: { \
                                    FULLTEST(test.fileName, sort, output, int, load_data<int>) \
                                    break; \
                                } \
                                case STRING: { \
                                    FULLTEST(test.fileName, sort, output, std::string, load_strings) \
                                    break; \
                                }            \
                                case LARGETESTCLASS: {  \
                                    FULLTEST(test.fileName, sort, output, LargeTestClass, load_data<LargeTestClass>) \
                                    break; \
                                } \
                                default: \
                                    break; \
                            } \
                        }


// possible types for testing, NONE is for ignoring random files.
enum ETestType {
    NONE, NUMBER, STRING, LARGETESTCLASS
};

/**
 * Loads data for stable_test
 *
 * @param filename 
 * @return vector of StableTestClass
 */
std::vector<StableTestClass> load_stable(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary); // input file stream
    std::vector<StableTestClass> vect;
    file.seekg(0, std::ios::end);
    int fileSize = (int)(file.tellg()/sizeof(StableTestClass));
    file.seekg(0, std::ios::beg);
    vect.reserve(fileSize);
    // read the contents of the file into the vector
    StableTestClass inf;
    while(file.read(reinterpret_cast<char*>(&inf), sizeof(StableTestClass))) {
        vect.push_back(inf);
    }

    return vect;
}

/**
 * Splits string using given delimiter
 * Taken from: https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
 *
 * @param s given string
 * @param delim delimiter
 * @return vector of strings split by delimiter
 */
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}

/**
 * Structure holding information about test
 */
struct TestInfo {
    ETestType type;
    std::string fileName;
    explicit TestInfo(std::string fileName) : fileName(fileName) {
        std::string name = *split(fileName,'/').rbegin();
        std::string testType = *split(name, '_').begin();
        if (testType == "number")
            type = NUMBER;
        else if (testType == "string")
            type = STRING;
        else if (testType == "largeclass")
            type = LARGETESTCLASS;
        else
            type = NONE;
    }
};

/**
 * Loads all tests from directory test
 * @return vector of TestInfo
 */
std::vector<TestInfo> load_tests() {
    std::vector<TestInfo> tests;
    for (const auto & entry : std::filesystem::directory_iterator("test")) {
        if (*split(entry.path(),'.').rbegin() == "in")
            tests.emplace_back(entry.path());
    }
    return tests;
}

/**
 * Loads data for given type
 * @param fileName
 * @return vector of T
 */
template <class T>
std::vector<T> load_data(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary); // input file stream
    std::vector<T> vect;
    file.seekg(0, std::ios::end);
    int fileSize = (int)(file.tellg()/sizeof(T));
    file.seekg(0, std::ios::beg);
    vect.reserve(fileSize);
    // read the contents of the file into the vector
    T inf;
    while(file.read(reinterpret_cast<char*>(&inf), sizeof(inf)))
        vect.push_back(inf);

    return vect;
}

/**
 * Loads data for string tests
 * @param fileName
 * @return vector of strings
 */
std::vector<std::string> load_strings(const std::string& fileName) {
    std::ifstream file(fileName); // input file stream
    std::vector<std::string> strings;
    file.seekg(0, std::ios::end);
    int fileSize = (int)(file.tellg()/sizeof(char));
    file.seekg(0, std::ios::beg);
    // read the contents of the file into the vector
    for (int i = 0; i < fileSize; ++i) {
        std::string next;
        while (i != fileSize) {
            char c = file.get();
            if (c == '\n') {
                strings.push_back(next);
                break;
            }
            next += c;
            ++i;
        }
    }

    return strings;
}

