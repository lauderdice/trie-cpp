#include "trie.hpp"
#include "test-helpers.hpp"

#include "catch.hpp"

#include <chrono>
#include <random>
#include <iostream>
#include <algorithm>

using Clock = cpu_clock;


TEST_CASE("Erase actually deletes nodes", "[erase][.long]") {
    using namespace std::chrono_literals;
    double last_time = 0;
    auto add_prefix = [] (std::vector<std::string> const& strings, std::string const& prefix) {
        std::vector<std::string> results;
        for (auto const& str : strings) {
            results.push_back(prefix + str);
        }
        return results;
    };

    for (size_t i = 1'200; i < 64'000; i *= 2) {
        auto s = generate_string(i);
        trie lhs;
        lhs.insert(s);
        lhs.erase(s);
        auto start_time = Clock::now();
        bool contains = lhs.contains(s);
        auto end_time = Clock::now();
        // Add 0.5 ms to even out variability
        auto time_diff = end_time - start_time + 500us;
        auto total_time = static_cast<double>(time_diff.count());
        REQUIRE_FALSE(contains);

        if (last_time) {
            REQUIRE(total_time / last_time <= 1.25);
        }
        last_time = total_time;
        std::cout << "Trie find erased: i = " << i << " total time = " << (time_diff - 500us).count() << '\n';
    }
}

TEST_CASE("Trie intersection - complexity", "[intersection][.long]") {
    using namespace std::chrono_literals;
    double last_time = 0;
    auto add_prefix = [] (std::vector<std::string> const& strings, std::string const& prefix) {
        std::vector<std::string> results;
        for (auto const& str : strings) {
            results.push_back(prefix + str);
        }
        return results;
    };
    for (size_t i = 1'200; i < 64'000; i *= 2) {
        auto lhs_elems = add_prefix(generate_data(i), "aaa");
        trie lhs{ lhs_elems };
        auto rhs_elems = add_prefix(generate_data(i), "zzz");
        trie rhs{ rhs_elems };
        auto start_time = Clock::now();
        auto res = lhs & rhs;
        auto end_time = Clock::now();
        // Add 0.5 ms to even out variability
        auto time_diff = end_time - start_time + 500us;
        auto total_time = static_cast<double>(time_diff.count());
        REQUIRE(res == trie{});

        if (last_time) {
            REQUIRE(total_time / last_time <= 1.25);
        }
        last_time = total_time;
        std::cout << "Trie intersection: i = " << i << " total time = " << (time_diff - 500us).count() << '\n';
    }
}

TEST_CASE("member swap -- complexity", "[swap][.long]") {
    using namespace std::chrono_literals;
    using Clock = cpu_clock;
    double last_time = 0;
    auto remove_duplicates = [] (std::vector<std::string> data) {
        std::sort(begin(data), end(data));
        data.erase(std::unique(begin(data), end(data)),
                   end(data));
        return data;
    };
    for (size_t i = 1'200; i < 64'000; i *= 2) {
        auto lhs_elems = remove_duplicates(generate_data(i));
        trie lhs{ lhs_elems };
        auto rhs_elems = remove_duplicates(generate_data(i));
        trie rhs{ rhs_elems };
        auto start_time = Clock::now();
        lhs.swap(rhs);
        auto end_time = Clock::now();
        // Add 0.5 ms to even out variability
        auto time_diff = end_time - start_time + 500us;
        auto total_time = static_cast<double>(time_diff.count());
        auto silent_check = [] (auto const& lhs, auto const& rhs) {
            auto l = lhs;
            auto r = rhs;
            std::sort(begin(l), end(l));
            std::sort(begin(r), end(r));
            REQUIRE((l == r));
        };

        silent_check(extract_all(lhs), rhs_elems);
        silent_check(extract_all(rhs), lhs_elems);

        if (last_time) {
            REQUIRE(total_time / last_time <= 1.25);
        }
        last_time = total_time;
        std::cout << "member swap: i = " << i << " total time = " << (time_diff - 500us).count() << '\n';
    }
}
