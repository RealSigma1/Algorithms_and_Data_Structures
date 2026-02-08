#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include "HyperLogLog.hpp"
#include "HyperLogLogAvg.hpp"
#include "RandomStreamGen.hpp"

int main() {
    const int B = 12;
    const std::size_t K = 7;
    const std::size_t stream_size = 10000;
    const std::size_t num_streams = 100;
    const std::vector<double> prefixes = {
        0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5,
        0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.0};

    RandomStreamGen gen(42);

    HyperLogLog hll(B);
    HyperLogLogAvg hll_avg(B, K);

    std::vector<std::vector<double>> est_base(prefixes.size());
    std::vector<std::vector<double>> est_avg(prefixes.size());
    std::vector<std::vector<std::size_t>> exact_counts(prefixes.size());

    std::cout << "Running experiments: B=" << B
              << ", k=" << K
              << ", streams=" << num_streams
              << ", stream_size=" << stream_size << std::endl;

    for (std::size_t stream_idx = 0; stream_idx < num_streams; ++stream_idx) {
        auto stream = gen.generateStream(stream_size);
        hll.reset();
        hll_avg.reset();

        std::size_t prev_size = 0;
        for (std::size_t p_idx = 0; p_idx < prefixes.size(); ++p_idx) {
            std::size_t prefix_size =
                static_cast<std::size_t>(stream_size * prefixes[p_idx]);

            for (std::size_t i = prev_size; i < prefix_size; ++i) {
                hll.add(stream[i]);
                hll_avg.add(stream[i]);
            }
            prev_size = prefix_size;

            est_base[p_idx].push_back(hll.estimate());
            est_avg[p_idx].push_back(hll_avg.estimateMean());
            exact_counts[p_idx].push_back(hll.exactCount());
        }

        if ((stream_idx + 1) % 10 == 0) {
            std::cout << "  Processed " << (stream_idx + 1) << " / " << num_streams << " streams" << std::endl;
        }
    }

    std::vector<double> mean_base, std_base;
    std::vector<double> mean_avg, std_avg;
    std::vector<double> mean_exact;

    mean_base.reserve(prefixes.size());
    std_base.reserve(prefixes.size());
    mean_avg.reserve(prefixes.size());
    std_avg.reserve(prefixes.size());
    mean_exact.reserve(prefixes.size());

    for (std::size_t p_idx = 0; p_idx < prefixes.size(); ++p_idx) {
        double sb = 0.0, ssb = 0.0;
        double sa = 0.0, ssa = 0.0;
        double sx = 0.0;

        for (std::size_t s = 0; s < num_streams; ++s) {
            double vb = est_base[p_idx][s];
            double va = est_avg[p_idx][s];
            sb += vb;
            ssb += vb * vb;
            sa += va;
            ssa += va * va;
            sx += static_cast<double>(exact_counts[p_idx][s]);
        }

        double mb = sb / static_cast<double>(num_streams);
        double ma = sa / static_cast<double>(num_streams);
        double mx = sx / static_cast<double>(num_streams);

        double varb = (ssb / static_cast<double>(num_streams)) - (mb * mb);
        double vara = (ssa / static_cast<double>(num_streams)) - (ma * ma);

        mean_base.push_back(mb);
        std_base.push_back(std::sqrt(std::max(0.0, varb)));

        mean_avg.push_back(ma);
        std_avg.push_back(std::sqrt(std::max(0.0, vara)));

        mean_exact.push_back(mx);
    }

    {
        auto stream = gen.generateStream(stream_size);
        hll.reset();
        hll_avg.reset();

        std::ofstream out1("single_stream.csv");
        out1 << "prefix_percent,F0,N_base,N_avg\n";

        std::size_t prev_size = 0;
        for (double p : prefixes) {
            std::size_t prefix_size = static_cast<std::size_t>(stream_size * p);
            for (std::size_t i = prev_size; i < prefix_size; ++i) {
                hll.add(stream[i]);
                hll_avg.add(stream[i]);
            }
            prev_size = prefix_size;

            out1 << std::fixed << std::setprecision(2) << (p * 100.0) << ","
                 << hll.exactCount() << "," << hll.estimate() << ","
                 << hll_avg.estimateMean() << "\n";
        }

        std::cout << "Exported: single_stream.csv" << std::endl;
    }

    {
        std::ofstream out2("stats.csv");
        if (!out2.is_open()) {
            std::cerr << "Error: cannot open stats.csv for writing." << std::endl;
            return 1;
        }

        out2 << "prefix_percent,F0_mean,N_mean_base,N_std_base,N_mean_avg,N_std_avg\n";
        for (std::size_t i = 0; i < prefixes.size(); ++i) {
            out2 << std::fixed << std::setprecision(2) << (prefixes[i] * 100.0)
                 << "," << mean_exact[i]
                 << "," << mean_base[i] << "," << std_base[i]
                 << "," << mean_avg[i] << "," << std_avg[i] << "\n";
        }

        std::cout << "Exported: stats.csv" << std::endl;
    }

    double m = static_cast<double>(1ULL << B);
    double theoretical_rse = 1.04 / std::sqrt(m);
    double theoretical_bound = 1.3 / std::sqrt(m);

    double rse_base_100 = std_base.back() / mean_exact.back();
    double rse_avg_100 = std_avg.back() / mean_exact.back();

    double bias_base_100 =
        std::abs(mean_base.back() - mean_exact.back()) / mean_exact.back();
    double bias_avg_100 =
        std::abs(mean_avg.back() - mean_exact.back()) / mean_exact.back();

    std::cout << std::endl;
    std::cout << "Summary (at 100% prefix)" << std::endl;
    std::cout << "Theoretical HLL RSE: "
              << std::fixed << std::setprecision(2) << theoretical_rse * 100.0 << "%" << std::endl;
    std::cout << "Theoretical bound: "
              << std::fixed << std::setprecision(2) << theoretical_bound * 100.0 << "%" << std::endl;

    std::cout << "RSE (base HLL): "
              << std::fixed << std::setprecision(2) << rse_base_100 * 100.0 << "%" << std::endl;
    std::cout << "RSE (averaged HLL): "
              << std::fixed << std::setprecision(2) << rse_avg_100 * 100.0 << "%" << std::endl;

    std::cout << "Bias (base HLL): "
              << std::fixed << std::setprecision(6) << bias_base_100 * 100.0 << "%" << std::endl;
    std::cout << "Bias (averaged HLL): "
              << std::fixed << std::setprecision(6) << bias_avg_100 * 100.0 << "%" << std::endl;

    std::cout << std::endl;
    std::cout << "To generate plots: python plot.py" << std::endl;
}
