// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>

#ifndef BOOST_INT128_BUILD_MODULE

#include <boost/int128/int128.hpp>
#include <boost/int128/hash.hpp>

#else

import boost.int128;

#endif

#include <cstdint>
#include <functional>
#include <random>
#include <unordered_map>
#include <unordered_set>

void test_uint128_equivalent_hashes()
{
    using boost::int128::uint128;

    std::hash<uint128> hasher {};

    // Same value built different ways must hash identically
    const uint128 a {UINT64_C(0xDEADBEEF), UINT64_C(0xCAFEBABE)};
    const uint128 b {UINT64_C(0xDEADBEEF), UINT64_C(0xCAFEBABE)};
    BOOST_TEST_EQ(hasher(a), hasher(b));

    // Equality across assignment
    uint128 c {};
    c = a;
    BOOST_TEST_EQ(hasher(a), hasher(c));

    // Zero hashes to a stable value
    const uint128 zero1 {};
    const uint128 zero2 {0};
    BOOST_TEST_EQ(hasher(zero1), hasher(zero2));

    // Values constructed from a small integer match equivalent two-word form
    const uint128 small_a {42};
    const uint128 small_b {0, 42};
    BOOST_TEST_EQ(hasher(small_a), hasher(small_b));
}

void test_int128_equivalent_hashes()
{
    using boost::int128::int128;

    std::hash<int128> hasher {};

    const int128 a {INT64_C(-1), UINT64_C(0xCAFEBABE)};
    const int128 b {INT64_C(-1), UINT64_C(0xCAFEBABE)};
    BOOST_TEST_EQ(hasher(a), hasher(b));

    int128 c {};
    c = a;
    BOOST_TEST_EQ(hasher(a), hasher(c));

    const int128 zero1 {};
    const int128 zero2 {0};
    BOOST_TEST_EQ(hasher(zero1), hasher(zero2));

    const int128 pos_a {42};
    const int128 pos_b {0, 42};
    BOOST_TEST_EQ(hasher(pos_a), hasher(pos_b));
}

void test_int128_negative_no_collision_with_absolute()
{
    using boost::int128::int128;

    std::hash<int128> hasher {};

    // hash(-x) must not equal hash(x) for non-zero x
    for (std::int64_t i {1}; i <= 1024; ++i)
    {
        const int128 pos {i};
        const int128 neg {-i};
        BOOST_TEST_NE(hasher(pos), hasher(neg));
    }

    // Larger magnitudes including values that span beyond 64 bits
    const int128 big_pos {INT64_C(0x0000000100000000), UINT64_C(0)};
    const int128 big_neg {-big_pos};
    BOOST_TEST_NE(hasher(big_pos), hasher(big_neg));

    // Values whose low word matches their negation's low word arithmetic (mod 2^64) are still distinct
    const int128 one {1};
    const int128 minus_one {-1};
    BOOST_TEST_NE(hasher(one), hasher(minus_one));

    // INT128_MIN and a value with the same low word but opposite high word
    const int128 edge_pos {1};
    const int128 edge_neg {-1};
    BOOST_TEST_NE(hasher(edge_pos), hasher(edge_neg));
}

void test_hash_distribution_uint128()
{
    using boost::int128::uint128;

    std::hash<uint128> hasher {};
    std::unordered_set<std::size_t> seen {};

    // A few thousand random values should produce nearly distinct hashes
    std::mt19937_64 rng {42};
    constexpr int num_samples {4096};
    for (int i {0}; i < num_samples; ++i)
    {
        const uint128 v {rng(), rng()};
        seen.insert(hasher(v));
    }

    // Allow some collisions (size_t may be 32-bit on some platforms) but require high uniqueness
    BOOST_TEST_GT(seen.size(), static_cast<std::size_t>(num_samples - 16));
}

void test_hash_distribution_int128()
{
    using boost::int128::int128;

    std::hash<int128> hasher {};
    std::unordered_set<std::size_t> seen {};

    std::mt19937_64 rng {123};
    constexpr int num_samples {4096};
    for (int i {0}; i < num_samples; ++i)
    {
        const int128 v {static_cast<std::int64_t>(rng()), rng()};
        seen.insert(hasher(v));
    }

    BOOST_TEST_GT(seen.size(), static_cast<std::size_t>(num_samples - 16));
}

void test_use_in_unordered_map_uint128()
{
    using boost::int128::uint128;

    std::unordered_map<uint128, int> map {};
    map[uint128{0, 1}] = 1;
    map[uint128{0, 2}] = 2;
    map[uint128{1, 0}] = 3;
    map[uint128{UINT64_MAX, UINT64_MAX}] = 4;

    BOOST_TEST_EQ((map[uint128{0, 1}]), 1);
    BOOST_TEST_EQ((map[uint128{0, 2}]), 2);
    BOOST_TEST_EQ((map[uint128{1, 0}]), 3);
    BOOST_TEST_EQ((map[uint128{UINT64_MAX, UINT64_MAX}]), 4);
    BOOST_TEST_EQ(map.size(), (std::size_t {4}));
}

void test_use_in_unordered_map_int128()
{
    using boost::int128::int128;

    std::unordered_map<int128, int> map {};
    map[int128{0, 1}] = 1;
    map[int128{-1, UINT64_MAX}] = 2;
    map[int128{1, 0}] = 3;
    map[int128{-1, 0}] = 4;

    BOOST_TEST_EQ((map[int128{0, 1}]), 1);
    BOOST_TEST_EQ((map[int128{-1, UINT64_MAX}]), 2);
    BOOST_TEST_EQ((map[int128{1, 0}]), 3);
    BOOST_TEST_EQ((map[int128{-1, 0}]), 4);
    BOOST_TEST_EQ(map.size(), (std::size_t {4}));
}

void test_high_low_swap_not_colliding()
{
    using boost::int128::uint128;

    std::hash<uint128> hasher {};

    // hash({a, b}) should not equal hash({b, a}) in general
    const uint128 a {UINT64_C(0x1111), UINT64_C(0x2222)};
    const uint128 b {UINT64_C(0x2222), UINT64_C(0x1111)};
    BOOST_TEST_NE(hasher(a), hasher(b));
}

int main()
{
    test_uint128_equivalent_hashes();
    test_int128_equivalent_hashes();
    test_int128_negative_no_collision_with_absolute();
    test_hash_distribution_uint128();
    test_hash_distribution_int128();
    test_use_in_unordered_map_uint128();
    test_use_in_unordered_map_int128();
    test_high_low_swap_not_colliding();

    return boost::report_errors();
}
