#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <random>
#include <climits>
#include <algorithm>
#include <filesystem>
#include "test_classes.hpp"

using namespace std;

/**
 * WELLRNG state, needs to be initialized to random bits
 */
static unsigned long state[16];
/**
 * WELLRNG state index, init should also reset this to 0
 */
static unsigned int index = 0;
/**
 * Pseudorandom number generator
 * More info https://en.wikipedia.org/wiki/Well_equidistributed_long-period_linear
 * @returns 32 bit random unsigned number
 */
unsigned long WELLRNG512() {
    unsigned long a, b, c, d;
    a = state[index];
    c = state[(index+13)&15];
    b = a^c^(a<<16)^(c<<15);
    c = state[(index+9)&15];
    c ^= (c>>11);
    a = state[index] = b^c;
    d = a^((a<<5)&0xDA442D24UL);
    index = (index + 15)&15;
    a = state[index];
    state[index] = a^b^d^(a<<2)^(b<<18)^(c<<28);
    return state[index];
}

/**
 * Initializes state for WELLRNG512.
 */
void init_random() {
    index = 0;
    for (int i = 0; i < 16; ++i) {
        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<mt19937::result_type> dist(INT_MIN,INT_MAX);

        state[i] = (int)dist(rng);
    }
}

/**
 * Types of possible tests
 */
enum EType {
    NUMBERRNG, STRINGRNG, NUMBERSORTED, STRINGSORTED, NUMBERMULTIPLESORTED, STRINGMULTIPLESORTED, TESTCLASSSTABLE,
    NUMBERREVERSE, STRINGREVERSE, STRINGDIFFERENTSIZE, NUMBERPYRAMID, STRINGPYRAMID, LARGETESTCLASSRNG, NUMBERDUPLICATE,
    STRINGDUPLICATE
};

/**
 * Holds information about single test case
 */
struct TestCase {
    string fileName;
    EType type;
    int64_t length;
    int parts;
    TestCase(string fileName, EType type, int64_t length, int parts = 1) : fileName(fileName), type(type), length(length/parts), parts(parts) {}

    /**
     * Generates and saves data for this test case.
     */
    void run() {
        switch (type) {
            case NUMBERRNG:
                numberRNG();
                break;
            case STRINGRNG:
                stringRNG();
                break;
            case NUMBERSORTED:
                numberSorted();
                break;
            case STRINGSORTED:
                stringSorted();
                break;
            case NUMBERMULTIPLESORTED:
                numberMultipleSorted();
                break;
            case STRINGMULTIPLESORTED:
                stringMultipleSorted();
                break;
            case TESTCLASSSTABLE:
                stable();
                break;
            case NUMBERREVERSE:
                numberReverse();
                break;
            case STRINGREVERSE:
                stringReverse();
                break;
            case STRINGDIFFERENTSIZE:
                stringDifferentSize();
                break;
            case NUMBERPYRAMID:
                numberPyramid();
                break;
            case STRINGPYRAMID:
                stringPyramid();
                break;
            case LARGETESTCLASSRNG:
                largeTestClassRNG();
                break;
            case NUMBERDUPLICATE:
                numberDuplicate();
                break;
            case STRINGDUPLICATE:
                stringDuplicate();
                break;
        }
    }

    void numberRNG() {
        vector<int> vector;
        vector.reserve(length);
        init_random();

        for (int i = 0; i < length; ++i) {
            vector.push_back((int)WELLRNG512());
        }
        save(vector);
    }

    void numberMultipleSorted() {
        vector<int> vector;
        vector.reserve(length*parts);

        for (int i = 0; i < parts; ++i) {
            for (int j = 0; j < length; ++j) {
                vector.push_back(j-(length/2));
            }
        }

        save(vector);
    }

    void stringRNG() {
        vector<string> vector;
        vector.reserve(length);
        init_random();

        for (int i = 0; i < length; ++i) {
            string x;
            for (int j = 0; j < 5; ++j) {
                bool big = WELLRNG512() % 2 == 1;
                x += (char)(WELLRNG512() % 26 + (big ? 65 : 97));
            }
            vector.push_back(x);
        }
        save_string(vector);
    }

    void stringMultipleSorted() {
        vector<string> vector;
        vector.reserve(length*parts);

        for (int i = 0; i < parts; ++i) {
            for (int j = 0; j < length; ++j) {
                string x;
                x += (char)((j / (26*26*26*26)) % 26 + 65);
                x += (char)((j / (26*26*26)) % 26 + 65);
                x += (char)((j / (26*26)) % 26 + 65);
                x += (char)((j / 26) % 26 + 65);
                x += (char)(j % 26 + 65);
                vector.push_back(x);
            }
        }
        save_string(vector);
    }

    void stable() {
        vector<StableTestClass> vector;
        vector.reserve(length);
        init_random();

        StableTestClass tc;
        for (int a = 0; a < 5; ++a) {
            tc.a = a*10;
            for (int b = 0; b < 5; ++b) {
                tc.b = WELLRNG512() % 300;
                for (int c = 0; c < 5; ++c) {
                    tc.c = WELLRNG512() % 10;
                    vector.push_back(tc);
                }
            }
        }
        save(vector);
    }

    StableTestClass getOneStable() {
        StableTestClass tc;
        tc.a = WELLRNG512() % 15;
        tc.b = WELLRNG512() % 300;
        tc.c = WELLRNG512() % 10;
        return tc;
    }

    void numberSorted() {
        vector<int> vector;
        vector.reserve(length);

        for (int j = 0; j < length; ++j) {
            vector.push_back((int)(j-(length/2)));
        }

        save(vector);
    }

    void stringSorted() {
        vector<string> vector;
        vector.reserve(length);
        for (int j = 0; j < length; ++j) {
            string x;
            x += (char)((j / (26*26*26*26)) % 26 + 65);
            x += (char)((j / (26*26*26)) % 26 + 65);
            x += (char)((j / (26*26)) % 26 + 65);
            x += (char)((j / 26) % 26 + 65);
            x += (char)(j % 26 + 65);
            vector.push_back(x);
        }
        save_string(vector);
    }

    void numberReverse() {
        vector<int> vector;
        vector.reserve(length);

        for (int j = 0; j < length; ++j) {
            vector.push_back((int)(j-(length/2)));
        }
        std::reverse(vector.begin(), vector.end());
        save(vector);
    }

    void stringReverse() {
        vector<string> vector;
        vector.reserve(length);
        for (int j = 0; j < length; ++j) {
            string x;
            x += (char)((j / (26*26*26*26)) % 26 + 65);
            x += (char)((j / (26*26*26)) % 26 + 65);
            x += (char)((j / (26*26)) % 26 + 65);
            x += (char)((j / 26) % 26 + 65);
            x += (char)(j % 26 + 65);
            vector.push_back(x);
        }
        std::reverse(vector.begin(), vector.end());

        save_string(vector);
    }

    void numberPyramid() {
        vector<int> vector;
        vector.reserve(length);

        for (int j = 0; j < length/2; ++j) {
            vector.push_back((int)(j-(length/4)));
        }
        std::reverse_copy(vector.begin(), vector.end(),vector.end());

        save(vector);
    }

    void stringPyramid() {
        vector<string> vector;
        vector.reserve(length);
        for (int j = 0; j < length/2; ++j) {
            string x;
            x += (char)((j / (26*26*26*26)) % 26 + 65);
            x += (char)((j / (26*26*26)) % 26 + 65);
            x += (char)((j / (26*26)) % 26 + 65);
            x += (char)((j / 26) % 26 + 65);
            x += (char)(j % 26 + 65);
            vector.push_back(x);
        }
        vector.insert(vector.end(), vector.rbegin(), vector.rend());
        save_string(vector);
    }

    void stringDifferentSize() {
        vector<string> vector;
        vector.reserve(length);
        init_random();

        for (int i = 0; i < length; ++i) {
            string x;
            int size = (int)(WELLRNG512() % 5 + 2);
            for (int j = 0; j < size; ++j) {
                bool big = WELLRNG512() % 2 == 1;
                x += (char)(WELLRNG512() % 26 + (big ? 65 : 97));
            }
            vector.push_back(x);
        }
        save_string(vector);
    }

    void largeTestClassRNG() {
        std::vector<LargeTestClass> vector;
        init_random();
        for (int i = 0; i < length; ++i) {
            LargeTestClass tc;

            tc.a = getOneStable();
            tc.b = getOneStable();
            tc.c = getOneStable();
            tc.d = WELLRNG512();
            vector.push_back(tc);
        }

        std::random_device rd;
        std::mt19937 g(rd());

        std::shuffle(vector.begin(), vector.end(), g);
        save(vector);
    }

    void numberDuplicate() {
        vector<int> vector;
        init_random();
        vector.reserve(length);

        for (int j = 0; j < length; ++j) {
            vector.push_back((int)(WELLRNG512() % 256));
        }

        save(vector);
    }

    void stringDuplicate() {
        vector<string> vector;
        init_random();
        vector.reserve(length);
        for (int j = 0; j < length; ++j) {
            int c = WELLRNG512() % 256;
            string x;
            x += (char)((c / (26*26*26*26)) % 26 + 65);
            x += (char)((c / (26*26*26)) % 26 + 65);
            x += (char)((c / (26*26)) % 26 + 65);
            x += (char)((c / 26) % 26 + 65);
            x += (char)(c % 26 + 65);
            vector.push_back(x);
        }
        save_string(vector);
    }


    /**
     * Saves vector as binary file.
     * @tparam T type of data
     * @param vect vector of data
     */
    template <class T>
    void save(const vector<T> & vect) {
        ofstream fout("test/" + fileName+".in", ios::out | ios::binary);
        fout.write((char*)&vect[0], vect.size() * sizeof(T));
        fout.close();
    }

    /**
     * Saves vectorof strings as file with each string on line.
     * @param vect vector of string
     */
    void save_string(const vector<std::string> & vect) {
        ofstream fout("test/" + fileName+".in", ios::out);
        for (const std::string & x: vect) {
            fout << x << "\n";
        }
        fout.close();
    }
};


int main() {
    if (!std::filesystem::is_directory("test") || !std::filesystem::exists("test")) {
        std::filesystem::create_directory("test");
    }
    vector<TestCase> tests;
    tests.emplace_back("number_rng_50000", NUMBERRNG, 50000);
    tests.emplace_back("number_rng_500000", NUMBERRNG, 500000);
    tests.emplace_back("number_rng_5000000", NUMBERRNG, 5000000);
    tests.emplace_back("number_rng_50000000", NUMBERRNG, 50000000);
    tests.emplace_back("number_rng_500000000", NUMBERRNG, 500000000);
    tests.emplace_back("string_rng_5000", STRINGRNG, 5000);
    tests.emplace_back("string_rng_50000", STRINGRNG, 50000);
    tests.emplace_back("string_rng_500000", STRINGRNG, 500000);
    tests.emplace_back("string_rng_5000000", STRINGRNG, 5000000);

    tests.emplace_back("number_sorted_50000", NUMBERSORTED, 50000);
    tests.emplace_back("number_sorted_500000", NUMBERSORTED, 500000);
    tests.emplace_back("number_sorted_5000000", NUMBERSORTED, 5000000);
    tests.emplace_back("number_sorted_50000000", NUMBERSORTED, 50000000);
    tests.emplace_back("string_sorted_5000", STRINGSORTED, 5000);
    tests.emplace_back("string_sorted_50000", STRINGSORTED, 50000);
    tests.emplace_back("string_sorted_500000", STRINGSORTED, 500000);
    tests.emplace_back("string_sorted_5000000", STRINGSORTED, 5000000);

    tests.emplace_back("number_multsorted8_50000", NUMBERMULTIPLESORTED, 50000,8);
    tests.emplace_back("number_multsorted8_500000", NUMBERMULTIPLESORTED, 500000,8);
    tests.emplace_back("number_multsorted8_5000000", NUMBERMULTIPLESORTED, 5000000,8);
    tests.emplace_back("number_multsorted8_50000000", NUMBERMULTIPLESORTED, 50000000,8);
    tests.emplace_back("string_multsorted8_5000", STRINGMULTIPLESORTED, 5000,8);
    tests.emplace_back("string_multsorted8_50000", STRINGMULTIPLESORTED, 50000,8);
    tests.emplace_back("string_multsorted8_500000", STRINGMULTIPLESORTED, 500000,8);
    tests.emplace_back("string_multsorted8_5000000", STRINGMULTIPLESORTED, 5000000,8);

    tests.emplace_back("number_reverse_50000", NUMBERREVERSE, 50000);
    tests.emplace_back("number_reverse_500000", NUMBERREVERSE, 500000);
    tests.emplace_back("number_reverse_5000000", NUMBERREVERSE, 5000000);
    tests.emplace_back("number_reverse_50000000", NUMBERREVERSE, 50000000);
    tests.emplace_back("string_reverse_5000", STRINGREVERSE, 5000);
    tests.emplace_back("string_reverse_50000", STRINGREVERSE, 50000);
    tests.emplace_back("string_reverse_500000", STRINGREVERSE, 500000);
    tests.emplace_back("string_reverse_5000000", STRINGREVERSE, 5000000);

    tests.emplace_back("number_pyramid_50000", NUMBERPYRAMID, 50000);
    tests.emplace_back("number_pyramid_500000", NUMBERPYRAMID, 500000);
    tests.emplace_back("number_pyramid_5000000", NUMBERPYRAMID, 5000000);
    tests.emplace_back("number_pyramid_50000000", NUMBERPYRAMID, 50000000);
    tests.emplace_back("string_pyramid_5000", STRINGPYRAMID, 5000);
    tests.emplace_back("string_pyramid_50000", STRINGPYRAMID, 50000);
    tests.emplace_back("string_pyramid_500000", STRINGPYRAMID, 500000);
    tests.emplace_back("string_pyramid_5000000", STRINGPYRAMID, 5000000);

    tests.emplace_back("string_diffsize_5000", STRINGDIFFERENTSIZE, 5000);
    tests.emplace_back("string_diffsize_50000", STRINGDIFFERENTSIZE, 50000);
    tests.emplace_back("string_diffsize_500000", STRINGDIFFERENTSIZE, 500000);
    tests.emplace_back("string_diffsize_5000000", STRINGDIFFERENTSIZE, 5000000);


    tests.emplace_back("largeclass_rng_5000", LARGETESTCLASSRNG, 5000);
    tests.emplace_back("largeclass_rng_50000", LARGETESTCLASSRNG, 50000);
    tests.emplace_back("largeclass_rng_500000", LARGETESTCLASSRNG, 500000);
    tests.emplace_back("largeclass_rng_5000000", LARGETESTCLASSRNG, 5000000);

    tests.emplace_back("number_duplicate_50000", NUMBERDUPLICATE, 50000);
    tests.emplace_back("number_duplicate_500000", NUMBERDUPLICATE, 500000);
    tests.emplace_back("number_duplicate_5000000", NUMBERDUPLICATE, 5000000);
    tests.emplace_back("number_duplicate_50000000", NUMBERDUPLICATE, 50000000);
    tests.emplace_back("string_duplicate_5000", STRINGDUPLICATE, 5000);
    tests.emplace_back("string_duplicate_50000", STRINGDUPLICATE, 50000);
    tests.emplace_back("string_duplicate_500000", STRINGDUPLICATE, 500000);
    tests.emplace_back("string_duplicate_5000000", STRINGDUPLICATE, 5000000);

    tests.emplace_back("stable", TESTCLASSSTABLE, 50000);

    for (auto &test: tests) {
        test.run();
    }
}
