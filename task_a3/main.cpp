#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace std;

mt19937 rng((uint32_t)chrono::steady_clock::now().time_since_epoch().count());

void insertion_sort(vector<int>& arr, int left, int right) {
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

void heapify(vector<int>& arr, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest]) {
        largest = left;
    }

    if (right < n && arr[right] > arr[largest]) {
        largest = right;
    }

    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heap_sort(vector<int>& arr, int left, int right) {
    int n = right - left + 1;
    if (n <= 1)
        return;

    vector<int> subarr(n);
    for (int i = 0; i < n; i++) {
        subarr[i] = arr[left + i];
    }

    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(subarr, n, i);
    }

    for (int i = n - 1; i > 0; i--) {
        swap(subarr[0], subarr[i]);
        heapify(subarr, i, 0);
    }

    for (int i = 0; i < n; i++) {
        arr[left + i] = subarr[i];
    }
}

int part_rand(vector<int>& arr, int left, int right) {
    uniform_int_distribution<int> dist(left, right);
    int pivot_index = dist(rng);
    swap(arr[pivot_index], arr[right]);

    int pivot = arr[right];
    int i = left - 1;

    for (int j = left; j < right; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }

    swap(arr[i + 1], arr[right]);
    return i + 1;
}

void qs_rec(vector<int>& arr, int left, int right) {
    if (left >= right)
        return;

    int pivot_index = part_rand(arr, left, right);
    qs_rec(arr, left, pivot_index - 1);
    qs_rec(arr, pivot_index + 1, right);
}

void quicksort(vector<int>& arr) {
    if (arr.empty())
        return;
    qs_rec(arr, 0, (int)arr.size() - 1);
}

void introsort_impl(vector<int>& arr, int left, int right, int depth_limit) {
    if (left >= right)
        return;
    int size = right - left + 1;

    if (size < 16) {
        insertion_sort(arr, left, right);
        return;
    }

    if (depth_limit == 0) {
        heap_sort(arr, left, right);
        return;
    }

    int pivot_index = part_rand(arr, left, right);

    introsort_impl(arr, left, pivot_index - 1, depth_limit - 1);
    introsort_impl(arr, pivot_index + 1, right, depth_limit - 1);
}

void introsort(vector<int>& arr) {
    int n = (int)arr.size();
    if (n <= 1)
        return;

    int depth_limit = 2 * (int)floor(log2(n));
    introsort_impl(arr, 0, n - 1, depth_limit);
}

vector<int> generate_random(int n) {
    vector<int> a(n);
    uniform_int_distribution<int> dist(0, 1'000'000'000);
    for (int i = 0; i < n; ++i) {
        a[i] = dist(rng);
    }
    return a;
}

vector<int> generate_sorted(int n) {
    vector<int> a = generate_random(n);
    sort(a.begin(), a.end());
    return a;
}

vector<int> generate_reverse_sorted(int n) {
    vector<int> a = generate_sorted(n);
    reverse(a.begin(), a.end());
    return a;
}

class SortTester {
public:
    using Clock = chrono::steady_clock;

    long long measure_quick_sort(vector<int> arr) const {
        auto start = Clock::now();
        quicksort(arr);
        auto finish = Clock::now();
        return chrono::duration_cast<chrono::nanoseconds>(finish - start)
            .count();
    }

    long long measure_introsort(vector<int> arr) const {
        auto start = Clock::now();
        introsort(arr);
        auto finish = Clock::now();
        return chrono::duration_cast<chrono::nanoseconds>(finish - start)
            .count();
    }

    template <typename Generator>
    void run(const string& category, Generator gen, const vector<int>& sizes,
             int repeats, ostream& out) const {

        for (int n : sizes) {
            for (int t = 0; t < repeats; ++t) {
                vector<int> base = gen(n);

                long long t_quick = measure_quick_sort(base);
                long long t_hybrid = measure_introsort(base);

                out << category << ";quick;" << n << ";" << t << ";" << t_quick
                    << "\n";
                out << category << ";hybrid;" << n << ";" << t << ";"
                    << t_hybrid << "\n";
            }
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    SortTester tester;

    vector<int> sizes = {1000, 5000, 10000, 20000, 50000};
    int repeats = 10;

    ofstream out("results.csv");
    if (!out) {
        cerr << "Can't open results.csv\n";
        return 1;
    }
    cout << "Saved to results.csv";

    out << "category;algo;n;trial;time_ns\n";

    tester.run("random", generate_random, sizes, repeats, out);

    tester.run("sorted", generate_sorted, sizes, repeats, out);

    tester.run("reverse", generate_reverse_sorted, sizes, repeats, out);
}
