
CXX = g++
CXXFLAGS = -Wall -pedantic -std=c++20 -fopenmp -O3

.PHONY: all
all: compile generate_tests test

.PHONY: generate_tests
generate_tests: compile
	./generate_tests

.PHONY: test
test: compile
	./tester

.PHONY: compile
compile:
	$(CXX) $(CXXFLAGS) tester.cpp -o tester
	$(CXX) $(CXXFLAGS) generate_tests.cpp -o generate_tests


.PHONY: star
star: compile
	mkdir -p output
	mkdir -p sge
	qrun 20c 1 pdp_serial STAR/serial_job_gfx_timsort.sh
	qrun 20c 1 pdp_serial STAR/serial_job_it_merge_2_sort.sh
	qrun 20c 1 pdp_serial STAR/serial_job_it_merge_2_sort_rev.sh
	qrun 20c 1 pdp_serial STAR/serial_job_it_merge_3_sort.sh
	qrun 20c 1 pdp_serial STAR/serial_job_it_merge_4a_sort.sh
	qrun 20c 1 pdp_serial STAR/serial_job_it_merge_4b_sort.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_2_sort.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_3_sort.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_4a_sort.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_4b_sort.sh
	qrun 20c 1 pdp_serial STAR/serial_job_sort.sh
	qrun 20c 1 pdp_serial STAR/serial_job_stable_sort.sh
	qrun 20c 1 pdp_serial STAR/serial_job_timsort.sh
	qrun 20c 1 pdp_serial STAR/serial_job_timsort_rev.sh
# parallel 1 core
	qrun 20c 1 pdp_serial STAR/serial_job_timsort_parallel_a_1c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_timsort_parallel_b_1c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_sort_parallel_a_1c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_sort_parallel_b_1c.sh
# parallel 2 core
	qrun 20c 1 pdp_serial STAR/serial_job_timsort_parallel_a_2c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_timsort_parallel_b_2c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_sort_parallel_a_2c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_sort_parallel_b_2c.sh
# parallel 4 core
	qrun 20c 1 pdp_serial STAR/serial_job_timsort_parallel_a_4c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_timsort_parallel_b_4c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_sort_parallel_a_4c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_sort_parallel_b_4c.sh
# parallel 8 core
	qrun 20c 1 pdp_serial STAR/serial_job_timsort_parallel_a_8c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_timsort_parallel_b_8c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_sort_parallel_a_8c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_sort_parallel_b_8c.sh
# parallel 16 core
	qrun 20c 1 pdp_serial STAR/serial_job_timsort_parallel_a_16c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_timsort_parallel_b_16c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_sort_parallel_a_16c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_sort_parallel_b_16c.sh
# parallel 20 core
	qrun 20c 1 pdp_serial STAR/serial_job_timsort_parallel_a_20c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_timsort_parallel_b_20c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_sort_parallel_a_20c.sh
	qrun 20c 1 pdp_serial STAR/serial_job_merge_sort_parallel_b_20c.sh


clearStar:
	rm -rf output/ sge/

clear: clearStar
	rm -rf test/ tester generate_tests
