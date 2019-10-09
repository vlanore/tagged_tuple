#include <tuple>

int main() {
    std::tuple<int, std::string, double> my_tuple{1, "a", 3.2};
    int first = std::get<0>(my_tuple);
    std::string second = std::get<1>(my_tuple);

    // better syntax for declaration (auto-deduce template params)
    auto my_tuple2 = std::make_tuple(1, "a", 3.2);
    auto first2 = std::get<0>(my_tuple2);
}