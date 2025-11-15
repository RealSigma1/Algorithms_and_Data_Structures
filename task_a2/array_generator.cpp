#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

class ArrayGenerator {
private:
    std::vector<int> random_array;
    std::vector<int> reverse_sorted_array;
    std::vector<int> almost_sorted_array;

    const int MAX_SIZE = 100000;
    const int VALUE_RANGE_MIN = 0;
    const int VALUE_RANGE_MAX = 6000;

    void generate_arrays() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(VALUE_RANGE_MIN,
                                                VALUE_RANGE_MAX);

        std::cout << "Generating test arrays..." << std::endl;

        random_array.resize(MAX_SIZE);
        for (int i = 0; i < MAX_SIZE; i++) {
            random_array[i] = dist(gen);
        }

        reverse_sorted_array = random_array;
        std::sort(reverse_sorted_array.begin(), reverse_sorted_array.end(),
                  std::greater<int>());

        almost_sorted_array = reverse_sorted_array;
        std::sort(almost_sorted_array.begin(), almost_sorted_array.end());

        int swap_count = MAX_SIZE / 100;
        for (int i = 0; i < swap_count; i++) {
            int idx1 = rand() % MAX_SIZE;
            int idx2 = rand() % MAX_SIZE;
            std::swap(almost_sorted_array[idx1], almost_sorted_array[idx2]);
        }

        std::cout << "Test arrays generated successfully!" << std::endl;
    }

public:
    ArrayGenerator() {
        generate_arrays();
    }

    std::vector<int> get_random_array(int size) {
        return std::vector<int>(random_array.begin(),
                                random_array.begin() + size);
    }

    std::vector<int> get_reverse_sorted_array(int size) {
        return std::vector<int>(reverse_sorted_array.begin(),
                                reverse_sorted_array.begin() + size);
    }

    std::vector<int> get_almost_sorted_array(int size) {
        return std::vector<int>(almost_sorted_array.begin(),
                                almost_sorted_array.begin() + size);
    }

    std::vector<int> get_sizes() {
        std::vector<int> sizes;
        for (int size = 500; size <= 100000; size += 100) {
            sizes.push_back(size);
        }
        return sizes;
    }
};