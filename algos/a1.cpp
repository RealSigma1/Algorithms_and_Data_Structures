#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <limits>
#include <stdexcept>

using namespace std;

struct Metrics {
    long long symbolOps = 0;
};

class StringGenerator {
private:
    string alphabet;
    mt19937 gen;

public:
    StringGenerator() : alphabet("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#%:;^&*()-"),
                        gen(42) {}

    string randomString() {
        uniform_int_distribution<int> lenDist(10, 200);
        uniform_int_distribution<int> charDist(0, (int)alphabet.size() - 1);

        int len = lenDist(gen);
        string s;

        for (int i = 0; i < len; i++) {
            s += alphabet[charDist(gen)];
        }

        return s;
    }

    vector<string> randomArray(int n) {
        vector<string> a(n);

        for (int i = 0; i < n; i++) {
            a[i] = randomString();
        }

        return a;
    }

    vector<string> reversedArray(vector<string> a) {
        sort(a.begin(), a.end());
        reverse(a.begin(), a.end());
        return a;
    }

    vector<string> nearlySortedArray(vector<string> a) {
        sort(a.begin(), a.end());

        uniform_int_distribution<int> dist(0, (int)a.size() - 1);
        int swapsCount = max(1, (int)a.size() / 30);

        for (int i = 0; i < swapsCount; i++) {
            int x = dist(gen);
            int y = dist(gen);
            swap(a[x], a[y]);
        }

        return a;
    }
};

class StringSortTester {
private:
    static const int R = 256;
    static const int SWITCH_SIZE = 74;

    static int charAt(const string& s, int d) {
        if (d >= (int)s.size()) return -1;
        return (unsigned char)s[d];
    }

    static int charAtRadix(const string& s, int d, Metrics& m) {
        if (d < (int)s.size()) m.symbolOps++;
        if (d >= (int)s.size()) return -1;
        return (unsigned char)s[d];
    }

    static int compareChars(int a, int b, Metrics& m) {
        if (a >= 0 || b >= 0) m.symbolOps++;

        if (a < b) return -1;
        if (a > b) return 1;
        return 0;
    }

    static int compareStrings(const string& a, const string& b, Metrics& m) {
        int i = 0;
        int len = min(a.size(), b.size());

        while (i < len) {
            m.symbolOps++;

            if (a[i] < b[i]) return -1;
            if (a[i] > b[i]) return 1;

            i++;
        }

        if (a.size() < b.size()) return -1;
        if (a.size() > b.size()) return 1;
        return 0;
    }

    static pair<int, int> lcpCompare(const string& a, const string& b, int start, Metrics& m) {
        int i = start;
        int len = min(a.size(), b.size());

        while (i < len) {
            m.symbolOps++;

            if (a[i] < b[i]) return {-1, i};
            if (a[i] > b[i]) return {1, i};

            i++;
        }

        if (a.size() < b.size()) return {-1, i};
        if (a.size() > b.size()) return {1, i};
        return {0, i};
    }

    static void quickSort(vector<string>& a, int l, int r, Metrics& m) {
        if (l >= r) return;

        int p = (l + r) / 2;
        swap(a[l], a[p]);

        string pivot = a[l];
        int lt = l;
        int gt = r;
        int i = l + 1;

        while (i <= gt) {
            int cmp = compareStrings(a[i], pivot, m);

            if (cmp < 0) {
                swap(a[lt++], a[i++]);
            } else if (cmp > 0) {
                swap(a[i], a[gt--]);
            } else {
                i++;
            }
        }

        quickSort(a, l, lt - 1, m);
        quickSort(a, gt + 1, r, m);
    }

    static void mergeSort(vector<string>& a, vector<string>& tmp, int l, int r, Metrics& m) {
        if (r - l <= 1) return;

        int mid = (l + r) / 2;

        mergeSort(a, tmp, l, mid, m);
        mergeSort(a, tmp, mid, r, m);

        int i = l;
        int j = mid;
        int k = l;

        while (i < mid && j < r) {
            if (compareStrings(a[i], a[j], m) <= 0) {
                tmp[k++] = a[i++];
            } else {
                tmp[k++] = a[j++];
            }
        }

        while (i < mid) tmp[k++] = a[i++];
        while (j < r) tmp[k++] = a[j++];

        for (int x = l; x < r; x++) {
            a[x] = tmp[x];
        }
    }

    static void stringQuickSort(vector<string>& a, int l, int r, int d, Metrics& m) {
        if (l >= r) return;

        int p = (l + r) / 2;
        swap(a[l], a[p]);

        int lt = l;
        int gt = r;
        int v = charAt(a[l], d);
        int i = l + 1;

        while (i <= gt) {
            int t = charAt(a[i], d);
            int cmp = compareChars(t, v, m);

            if (cmp < 0) {
                swap(a[lt++], a[i++]);
            } else if (cmp > 0) {
                swap(a[i], a[gt--]);
            } else {
                i++;
            }
        }

        stringQuickSort(a, l, lt - 1, d, m);

        if (v >= 0) {
            stringQuickSort(a, lt, gt, d + 1, m);
        }

        stringQuickSort(a, gt + 1, r, d, m);
    }

    struct LcpNode {
        string s;
        int lcp = 0;
    };

    static vector<LcpNode> lcpMerge(const vector<LcpNode>& left,
                                    const vector<LcpNode>& right,
                                    Metrics& m) {
        vector<LcpNode> res;
        int i = 0;
        int j = 0;
        int lastSide = -1;
        int crossLcp = 0;

        while (i < (int)left.size() && j < (int)right.size()) {
            int cmp;
            int pairLcp;
            int outputLcp;

            if (lastSide == -1) {
                auto cur = lcpCompare(left[i].s, right[j].s, 0, m);
                cmp = cur.first;
                pairLcp = cur.second;
                outputLcp = 0;
            } else if (lastSide == 0) {
                int h = left[i].lcp;

                if (h < crossLcp) {
                    cmp = 1;
                    pairLcp = h;
                } else if (h > crossLcp) {
                    cmp = -1;
                    pairLcp = crossLcp;
                } else {
                    auto cur = lcpCompare(left[i].s, right[j].s, h, m);
                    cmp = cur.first;
                    pairLcp = cur.second;
                }

                outputLcp = cmp <= 0 ? h : crossLcp;
            } else {
                int h = right[j].lcp;

                if (h < crossLcp) {
                    cmp = -1;
                    pairLcp = h;
                } else if (h > crossLcp) {
                    cmp = 1;
                    pairLcp = crossLcp;
                } else {
                    auto cur = lcpCompare(left[i].s, right[j].s, h, m);
                    cmp = cur.first;
                    pairLcp = cur.second;
                }

                outputLcp = cmp <= 0 ? crossLcp : h;
            }

            if (cmp <= 0) {
                res.push_back({left[i].s, res.empty() ? 0 : outputLcp});
                i++;
                lastSide = 0;
            } else {
                res.push_back({right[j].s, res.empty() ? 0 : outputLcp});
                j++;
                lastSide = 1;
            }

            crossLcp = pairLcp;
        }

        while (i < (int)left.size()) {
            int curLcp = 0;

            if (!res.empty()) {
                curLcp = lastSide == 0 ? left[i].lcp : crossLcp;
            }

            res.push_back({left[i].s, curLcp});
            i++;
            lastSide = 0;
        }

        while (j < (int)right.size()) {
            int curLcp = 0;

            if (!res.empty()) {
                curLcp = lastSide == 1 ? right[j].lcp : crossLcp;
            }

            res.push_back({right[j].s, curLcp});
            j++;
            lastSide = 1;
        }

        return res;
    }

    static vector<LcpNode> stringMergeSortLcp(vector<LcpNode> a, Metrics& m) {
        if (a.size() <= 1) return a;

        int mid = (int)a.size() / 2;

        vector<LcpNode> left(a.begin(), a.begin() + mid);
        vector<LcpNode> right(a.begin() + mid, a.end());

        left = stringMergeSortLcp(left, m);
        right = stringMergeSortLcp(right, m);

        return lcpMerge(left, right, m);
    }

    static void msdRadixSort(vector<string>& a,
                             vector<string>& aux,
                             int l,
                             int r,
                             int d,
                             Metrics& m,
                             bool useSwitch) {
        if (r <= l) return;

        if (useSwitch && r - l + 1 < SWITCH_SIZE) {
            stringQuickSort(a, l, r, d, m);
            return;
        }

        vector<int> cnt(R + 2, 0);

        for (int i = l; i <= r; i++) {
            cnt[charAtRadix(a[i], d, m) + 2]++;
        }

        for (int i = 0; i < R + 1; i++) {
            cnt[i + 1] += cnt[i];
        }

        for (int i = l; i <= r; i++) {
            int c = charAtRadix(a[i], d, m);
            aux[cnt[c + 1]++] = a[i];
        }

        for (int i = l; i <= r; i++) {
            a[i] = aux[i - l];
        }

        for (int c = 0; c < R; c++) {
            msdRadixSort(a, aux, l + cnt[c], l + cnt[c + 1] - 1, d + 1, m, useSwitch);
        }
    }

    static bool isSorted(const vector<string>& a) {
        for (int i = 1; i < (int)a.size(); i++) {
            if (a[i] < a[i - 1]) return false;
        }

        return true;
    }

public:
    struct Result {
        double timeUs = 0;
        double symbolOps = 0;
    };

    static Result test(const vector<string>& source, const string& algorithm, int repeats) {
        double totalTime = 0;
        double totalOps = 0;

        for (int run = 0; run < repeats; run++) {
            vector<string> a = source;
            Metrics m;

            auto start = chrono::high_resolution_clock::now();

            if (algorithm == "quick_sort") {
                quickSort(a, 0, (int)a.size() - 1, m);
            } else if (algorithm == "merge_sort") {
                vector<string> tmp(a.size());
                mergeSort(a, tmp, 0, (int)a.size(), m);
            } else if (algorithm == "string_quick_sort") {
                stringQuickSort(a, 0, (int)a.size() - 1, 0, m);
            } else if (algorithm == "string_merge_sort_lcp") {
                vector<LcpNode> nodes;

                for (const string& s : a) {
                    nodes.push_back({s, 0});
                }

                nodes = stringMergeSortLcp(nodes, m);

                for (int i = 0; i < (int)a.size(); i++) {
                    a[i] = nodes[i].s;
                }
            } else if (algorithm == "msd_radix_sort") {
                vector<string> aux(a.size());
                msdRadixSort(a, aux, 0, (int)a.size() - 1, 0, m, false);
            } else if (algorithm == "msd_radix_sort_switch") {
                vector<string> aux(a.size());
                msdRadixSort(a, aux, 0, (int)a.size() - 1, 0, m, true);
            } else {
                throw runtime_error("Unknown algorithm");
            }

            auto finish = chrono::high_resolution_clock::now();

            if (!isSorted(a)) {
                throw runtime_error("Array is not sorted");
            }

            totalTime += chrono::duration<double, micro>(finish - start).count();
            totalOps += m.symbolOps;
        }

        return {totalTime / repeats, totalOps / repeats};
    }
};

int main() {
    const int maxN = 3000;
    const int step = 100;
    const int repeats = 7;

    StringGenerator generator;

    vector<string> randomMax = generator.randomArray(maxN);
    vector<string> reversedMax = generator.reversedArray(randomMax);
    vector<string> nearlyMax = generator.nearlySortedArray(randomMax);

    vector<pair<string, vector<string>>> datasets = {
        {"random", randomMax},
        {"reversed", reversedMax},
        {"nearly_sorted", nearlyMax}
    };

    vector<string> algorithms = {
        "quick_sort",
        "merge_sort",
        "string_quick_sort",
        "string_merge_sort_lcp",
        "msd_radix_sort",
        "msd_radix_sort_switch"
    };

    ofstream out("results.csv");

    out << "dataset,n,algorithm,time_us,symbol_ops\n";
    cout << fixed << setprecision(3);

    for (const auto& dataset : datasets) {
        for (int n = step; n <= maxN; n += step) {
            vector<string> cur(dataset.second.begin(), dataset.second.begin() + n);

            for (const string& algorithm : algorithms) {
                auto result = StringSortTester::test(cur, algorithm, repeats);

                out << dataset.first << ','
                    << n << ','
                    << algorithm << ','
                    << result.timeUs << ','
                    << result.symbolOps << '\n';

                cout << dataset.first << ' '
                     << n << ' '
                     << algorithm << ' '
                     << result.timeUs << " us "
                     << result.symbolOps << '\n';
            }
        }
    }

}