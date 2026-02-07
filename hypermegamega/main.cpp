#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include "HyperLogLog.hpp"
#include "RandomStreamGen.hpp"

int main() {
    const int B = 12;
    const std::size_t stream_size = 10000;
    const std::size_t num_streams = 100;
    const std::vector<double> prefixes = {
        0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5,
        0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.0};

    RandomStreamGen gen(42);
    HyperLogLog hll(B);

    std::vector<std::vector<double>> estimates(prefixes.size());
    std::vector<std::vector<std::size_t>> exact_counts(prefixes.size());

    std::cout << "Running experiments (b=" << B << ", streams=" << num_streams
              << ")...\n";

    for (std::size_t stream_idx = 0; stream_idx < num_streams; ++stream_idx) {
        auto stream = gen.generateStream(stream_size);
        hll.reset();

        std::size_t prev_size = 0;
        for (std::size_t p_idx = 0; p_idx < prefixes.size(); ++p_idx) {
            std::size_t prefix_size =
                static_cast<std::size_t>(stream_size * prefixes[p_idx]);
            for (std::size_t i = prev_size; i < prefix_size; ++i) {
                hll.add(stream[i]);
            }
            prev_size = prefix_size;

            estimates[p_idx].push_back(hll.estimate());
            exact_counts[p_idx].push_back(hll.exactCount());
        }

        if ((stream_idx + 1) % 10 == 0) {
            std::cout << "  Processed " << (stream_idx + 1) << " streams\n";
        }
    }

    std::vector<double> mean_estimates;
    std::vector<double> std_estimates;
    std::vector<double> mean_exact;

    mean_estimates.reserve(prefixes.size());
    std_estimates.reserve(prefixes.size());
    mean_exact.reserve(prefixes.size());

    for (std::size_t p_idx = 0; p_idx < prefixes.size(); ++p_idx) {
        double sum_est = 0.0;
        double sum_sq_est = 0.0;
        double sum_exact = 0.0;

        for (std::size_t s = 0; s < num_streams; ++s) {
            const double v = estimates[p_idx][s];
            sum_est += v;
            sum_sq_est += v * v;
            sum_exact += static_cast<double>(exact_counts[p_idx][s]);
        }

        const double mean_est = sum_est / static_cast<double>(num_streams);
        const double mean_ex = sum_exact / static_cast<double>(num_streams);

        mean_estimates.push_back(mean_est);
        mean_exact.push_back(mean_ex);

        const double variance =
            (sum_sq_est / static_cast<double>(num_streams)) -
            (mean_est * mean_est);
        std_estimates.push_back(std::sqrt(std::max(0.0, variance)));
    }

    {
        auto stream = gen.generateStream(stream_size);
        hll.reset();

        std::ofstream out1("single_stream.csv");
        out1 << "prefix_percent,F0,N\n";

        std::size_t prev_size = 0;
        for (double p : prefixes) {
            std::size_t prefix_size = static_cast<std::size_t>(stream_size * p);
            for (std::size_t i = prev_size; i < prefix_size; ++i) {
                hll.add(stream[i]);
            }
            prev_size = prefix_size;

            out1 << std::fixed << std::setprecision(2) << (p * 100.0) << ","
                 << hll.exactCount() << "," << hll.estimate() << "\n";
        }

        std::cout << "Exported: single_stream.csv\n";
    }

    {
        std::ofstream out2("stats.csv");
        if (!out2.is_open()) {
            std::cerr << "Error: Cannot open stats.csv for writing!\n";
            return 1;
        }

        out2 << "prefix_percent,F0_mean,N_mean,N_std\n";
        for (std::size_t i = 0; i < prefixes.size(); ++i) {
            out2 << std::fixed << std::setprecision(2) << (prefixes[i] * 100.0)
                 << "," << mean_exact[i] << "," << mean_estimates[i] << ","
                 << std_estimates[i] << "\n";
        }

        std::cout << "Exported: stats.csv\n";
    }

    const double m = static_cast<double>(1ULL << B);
    const double theoretical_rse = 1.04 / std::sqrt(m);
    const double theoretical_bound = 1.3 / std::sqrt(m);

    const double rse_100 = std_estimates.back() / mean_exact.back();
    const double bias_100 =
        std::abs(mean_estimates.back() - mean_exact.back()) / mean_exact.back();

    std::cout << "\n=== Results ===\n";
    std::cout << "Theoretical RSE: " << std::fixed << std::setprecision(2)
              << theoretical_rse * 100.0 << "%\n";
    std::cout << "Theoretical bound: " << std::fixed << std::setprecision(2)
              << theoretical_bound * 100.0 << "%\n";
    std::cout << "Observed RSE: " << std::fixed << std::setprecision(2)
              << rse_100 * 100.0 << "%\n";
    std::cout << "Bias: " << std::fixed << std::setprecision(6)
              << bias_100 * 100.0 << "%\n";

    std::cout << "\nTo generate plots:\n";
    std::cout << "  python plot.py\n";
}
