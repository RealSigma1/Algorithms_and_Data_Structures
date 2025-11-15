#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

class ArrayGenerator;

class SortTester {
private:
    ArrayGenerator* generator;

    void standard_merge(std::vector<int>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        std::vector<int> left_arr(n1);
        std::vector<int> right_arr(n2);

        for (int i = 0; i < n1; i++)
            left_arr[i] = arr[left + i];
        for (int i = 0; i < n2; i++)
            right_arr[i] = arr[mid + 1 + i];

        int i = 0, j = 0, k = left;

        while (i < n1 && j < n2) {
            if (left_arr[i] <= right_arr[j]) {
                arr[k] = left_arr[i];
                i++;
            } else {
                arr[k] = right_arr[j];
                j++;
            }
            k++;
        }

        while (i < n1) {
            arr[k] = left_arr[i];
            i++;
            k++;
        }

        while (j < n2) {
            arr[k] = right_arr[j];
            j++;
            k++;
        }
    }

    void standard_merge_sort(std::vector<int>& arr, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            standard_merge_sort(arr, left, mid);
            standard_merge_sort(arr, mid + 1, right);
            standard_merge(arr, left, mid, right);
        }
    }

    void insertion_sort(std::vector<int>& arr, int left, int right) {
        for (int i = left + 1; i <= right; i++) {
            int key = arr[i];
            int j = i - 1;

            while (j >= left && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

    void hybrid_merge(std::vector<int>& arr, int left, int mid, int right) {
        standard_merge(arr, left, mid, right);
    }

    void hybrid_merge_sort(std::vector<int>& arr, int left, int right,
                           int threshold) {
        if (left < right) {
            if (right - left + 1 <= threshold) {
                insertion_sort(arr, left, right);
            } else {
                int mid = left + (right - left) / 2;
                hybrid_merge_sort(arr, left, mid, threshold);
                hybrid_merge_sort(arr, mid + 1, right, threshold);
                hybrid_merge(arr, left, mid, right);
            }
        }
    }

    void test_on_array_type(const std::string& type,
                            const std::vector<int>& sizes,
                            const std::vector<int>& thresholds) {
        std::cout << "Testing on " << type << " arrays..." << std::endl;

        std::ofstream file(type + "_results.csv");
        file << "Size,Standard";
        for (int threshold : thresholds) {
            file << ",Hybrid_" << threshold;
        }
        file << std::endl;

        for (size_t i = 0; i < sizes.size(); i++) {
            int size = sizes[i];

            if (i % 50 == 0) {
                std::cout << "Progress: " << (i * 100 / sizes.size()) << "% ("
                          << size << " elements)" << std::endl;
            }

            std::vector<int> arr;
            if (type == "Random") {
                arr = generator->get_random_array(size);
            } else if (type == "Reverse_Sorted") {
                arr = generator->get_reverse_sorted_array(size);
            } else {
                arr = generator->get_almost_sorted_array(size);
            }

            long long standard_time = 0;
            std::vector<long long> hybrid_times(thresholds.size(), 0);

            const int RUNS = 3;
            for (int run = 0; run < RUNS; run++) {
                std::vector<int> test_arr = arr;
                standard_time += test_standard_merge_sort(test_arr);

                for (size_t j = 0; j < thresholds.size(); j++) {
                    test_arr = arr;
                    hybrid_times[j] +=
                        test_hybrid_merge_sort(test_arr, thresholds[j]);
                }
            }

            standard_time /= RUNS;
            file << size << "," << standard_time;

            for (size_t j = 0; j < thresholds.size(); j++) {
                hybrid_times[j] /= RUNS;
                file << "," << hybrid_times[j];
            }
            file << std::endl;
        }

        file.close();
        std::cout << "Results saved to " << type << "_results.csv" << std::endl
                  << std::endl;
    }

public:
    SortTester(ArrayGenerator* gen)
        : generator(gen) {
    }

    long long test_standard_merge_sort(std::vector<int> arr) {
        auto start = std::chrono::high_resolution_clock::now();
        standard_merge_sort(arr, 0, arr.size() - 1);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::microseconds>(elapsed)
            .count();
    }

    long long test_hybrid_merge_sort(std::vector<int> arr, int threshold) {
        auto start = std::chrono::high_resolution_clock::now();
        hybrid_merge_sort(arr, 0, arr.size() - 1, threshold);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::microseconds>(elapsed)
            .count();
    }

    void run_tests() {
        std::vector<int> sizes = generator->get_sizes();
        std::vector<int> thresholds = {5, 10, 15, 20, 30, 50};

        std::cout << "Starting tests..." << std::endl;
        std::cout << "Thresholds: ";
        for (int th : thresholds)
            std::cout << th << " ";
        std::cout << std::endl << std::endl;

        test_on_array_type("Random", sizes, thresholds);
        test_on_array_type("Reverse_Sorted", sizes, thresholds);
        test_on_array_type("Almost_Sorted", sizes, thresholds);

        std::cout << "All tests completed! Results saved to CSV files."
                  << std::endl;
    }

};
