#include <iomanip>
#include <iostream>

#include "onedal/homogen_table.hpp"
#include "onedal/decomposition/pca.hpp"

std::ostream &operator <<(std::ostream& stream, const dal::table& table) {
    const auto x = dal::flatten<float, dal::access_mode::read>(table);
    for (std::int64_t i = 0; i < table.get_row_count(); i++) {
        for (std::int64_t j = 0; j < table.get_column_count(); j++) {
            std::cout << std::setw(10)
                      << std::setiosflags(std::ios::fixed)
                      << std::setprecision(3)
                      << x[i * table.get_column_count() + j]
                      << " ";
        }
        std::cout << std::endl;
    }
    return stream;
}

int main(int argc, char const *argv[]) {
    using namespace dal::decomposition;

    const std::int64_t row_count = 5;
    const std::int64_t column_count = 3;

    float data[] = {
        1.f,  2.f,  3.f,
        1.f,  -1.f, 0.f,
        4.f,  5.f,  6.f,
        1.f,  2.f,  5.f,
        -4.f, 3.f,  0.f
    };

    auto table = dal::create_table(data, row_count, column_count);

    const auto params = pca::params()
        .set_components_count(2)
        .set_is_deterministic(true);

    const auto model = dal::train(params, table)
        .get_model();

    std::cout << "Eigenvectors:" << std::endl
              << model.get_eigenvectors() << std::endl;

    return 0;
}
