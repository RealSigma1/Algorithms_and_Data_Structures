#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>

using namespace std;

struct Circle {
    double x, y, r;
};

bool in_circle(double x, double y, const Circle& c) {
    double dx = x - c.x;
    double dy = y - c.y;
    return dx * dx + dy * dy <= c.r * c.r;
}

bool in_intersec(double x, double y, const Circle& c1, const Circle& c2,
                 const Circle& c3) {
    return in_circle(x, y, c1) && in_circle(x, y, c2) && in_circle(x, y, c3);
}

double exact_area() {
    return 0.25 * M_PI + 1.25 * asin(0.8) - 1.0;
}

double monte_carlo_area(const Circle& c1, const Circle& c2, const Circle& c3,
                        double min_x, double max_x, double min_y, double max_y,
                        int total_points, mt19937& gen) {
    double rect_area = (max_x - min_x) * (max_y - min_y);

    uniform_real_distribution<double> dist_x(min_x, max_x);
    uniform_real_distribution<double> dist_y(min_y, max_y);

    int inside = 0;
    for (int i = 0; i < total_points; ++i) {
        double x = dist_x(gen);
        double y = dist_y(gen);
        if (in_intersec(x, y, c1, c2, c3)) {
            ++inside;
        }
    }

    return rect_area * static_cast<double>(inside) / total_points;
}

int main() {
    Circle c1{1.0, 1.0, 1.0};
    Circle c2{1.5, 2.0, sqrt(5.0) / 2.0};
    Circle c3{2.0, 1.5, sqrt(5.0) / 2.0};

    double S_exact = exact_area();

    cout << fixed << setprecision(15);
    cout << "Exact area = " << S_exact << "\n";

    double wide_min_x = min({c1.x - c1.r, c2.x - c2.r, c3.x - c3.r});
    double wide_max_x = max({c1.x + c1.r, c2.x + c2.r, c3.x + c3.r});
    double wide_min_y = min({c1.y - c1.r, c2.y - c2.r, c3.y - c3.r});
    double wide_max_y = max({c1.y + c1.r, c2.y + c2.r, c3.y + c3.r});

    double narrow_min_x = 0.88;
    double narrow_max_x = 2.0;
    double narrow_min_y = 0.88;
    double narrow_max_y = 2.0;

    ofstream areas_file("areas_results.csv");
    ofstream errors_file("errors_results.csv");

    areas_file << "N,Wide_Area,Narrow_Area,Exact_Area\n";
    errors_file << "N,Wide_Relative_Error,Narrow_Relative_Error\n";

    random_device rd;
    mt19937 gen(rd());

    for (int N = 100; N <= 100000; N += 500) {
        double S_wide = monte_carlo_area(c1, c2, c3, wide_min_x, wide_max_x,
                                         wide_min_y, wide_max_y, N, gen);

        double S_narrow =
            monte_carlo_area(c1, c2, c3, narrow_min_x, narrow_max_x,
                             narrow_min_y, narrow_max_y, N, gen);

        double err_wide = fabs(S_wide - S_exact) / S_exact;
        double err_narrow = fabs(S_narrow - S_exact) / S_exact;

        areas_file << N << "," << S_wide << "," << S_narrow << "," << S_exact
                   << "\n";
        errors_file << N << "," << err_wide << "," << err_narrow << "\n";

        if (N % 10000 == 100) {
            cout << "N = " << N << "  wide = " << S_wide
                 << "  narrow = " << S_narrow
                 << "  err_wide = " << err_wide * 100 << "% "
                 << "  err_narrow = " << err_narrow * 100 << "%\n";
        }
    }

    areas_file.close();
    errors_file.close();

    int bigN = 1000000;
    double S_wide_big = monte_carlo_area(c1, c2, c3, wide_min_x, wide_max_x,
                                         wide_min_y, wide_max_y, bigN, gen);
    double S_narrow_big =
        monte_carlo_area(c1, c2, c3, narrow_min_x, narrow_max_x, narrow_min_y,
                         narrow_max_y, bigN, gen);

    cout << "\nFor N = " << bigN << ":\n";
    cout << "Wide:   " << S_wide_big
         << "  (rel.error = " << fabs(S_wide_big - S_exact) / S_exact * 100
         << "%)\n";
    cout << "Narrow: " << S_narrow_big
         << "  (rel.error = " << fabs(S_narrow_big - S_exact) / S_exact * 100
         << "%)\n";

    cout << "\nResults saved to areas_results.csv and errors_results.csv\n";
}
