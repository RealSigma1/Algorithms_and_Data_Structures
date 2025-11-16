#include <iostream>
#include "array_generator.cpp"
#include "sort_tester.cpp"

int main() {
    std::cout << "SORTING ALGORITHM ANALYSIS" << std::endl;
    std::cout << std::endl;

    ArrayGenerator generator;

    SortTester tester(&generator);

    tester.run_tests();

    std::cout << "Analysis completed!" << std::endl;

}
