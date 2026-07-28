// Copyright 2026 Matt Borland
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_TEST_BENCHMARK_RESULTS_HPP
#define BOOST_INT128_TEST_BENCHMARK_RESULTS_HPP

// Command line handling and result collection shared by benchmark_u128.cpp and
// benchmark_i128.cpp. Every timing is still printed to stderr as it is measured
// and is additionally recorded here. With --json <path> the whole run is written
// out as the data set that doc/render_benchmarks.py turns into the documentation
// tables and plots, so refreshing the documentation never needs numbers to be
// transcribed by hand.

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace bench {

// One timing: the operand width group it ran in, the operation, the
// implementation under test, and the elapsed time in microseconds.
struct sample
{
    std::string group {};
    std::string operation {};
    std::string implementation {};
    std::int64_t microseconds {};
};

// Identifies the data set being written. The labels name the types the way a
// user would write them because they appear verbatim in the documentation
// tables and in the plot legends.
struct metadata
{
    std::string type {};
    std::string sign {};
    std::string baseline {};
};

// Run configuration. --elements and --repetitions exist so that emulated
// platforms can measure a smaller data set in a reasonable amount of time.
struct options
{
    std::string json_path {};
    std::size_t elements {20'000'000};
    unsigned repetitions {5};
};

inline options& config()
{
    static options instance {};
    return instance;
}

inline std::vector<sample>& samples()
{
    static std::vector<sample> instance {};
    return instance;
}

inline std::string& current_group()
{
    static std::string instance {"unspecified"};
    return instance;
}

// Names the operand width group that subsequent timings belong to.
inline void set_group(const char* group)
{
    current_group() = group;
}

// Adds one timing to the data set under the current group.
inline void record(const char* operation, const char* implementation, const std::int64_t microseconds)
{
    samples().push_back(sample{current_group(), operation, implementation, microseconds});
}

// Microseconds between two clock readings, truncated like the stderr log.
template <typename TimePoint>
std::int64_t elapsed_us(const TimePoint start, const TimePoint stop)
{
    return static_cast<std::int64_t>(std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count());
}

// Operating system the data set was measured on; also selects the sub-folder of
// the documentation images tree.
inline const char* os_name() noexcept
{
    #if defined(_WIN32)
    return "windows";
    #elif defined(__APPLE__)
    return "macos";
    #elif defined(__linux__)
    return "linux";
    #elif defined(__FreeBSD__)
    return "freebsd";
    #elif defined(__CYGWIN__)
    return "cygwin";
    #else
    return "unknown";
    #endif
}

// Architecture the data set was measured on; also the file stem of the plots.
inline const char* arch_name() noexcept
{
    // ARM64EC also defines _M_AMD64, so the AArch64 checks come first.
    #if defined(__aarch64__) || defined(_M_ARM64) || defined(_M_ARM64EC)
    return "ARM64";
    #elif defined(__arm__) || defined(_M_ARM)
    return "ARM32";
    #elif defined(__x86_64__) || defined(__amd64__) || defined(_M_AMD64)
    #  if defined(__ILP32__)
    return "x32";
    #  else
    return "x64";
    #  endif
    #elif defined(__i386__) || defined(_M_IX86)
    return "x86";
    #elif defined(__s390x__)
    return "s390x";
    #elif defined(__powerpc64__) || defined(__PPC64__)
    #  if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    return "ppc64le";
    #  else
    return "ppc64";
    #  endif
    #elif defined(__powerpc__) || defined(__PPC__)
    return "ppc";
    #elif defined(__riscv)
    #  if defined(__riscv_xlen) && __riscv_xlen == 64
    return "riscv64";
    #  else
    return "riscv32";
    #  endif
    #elif defined(__loongarch64)
    return "loongarch64";
    #else
    return "unknown";
    #endif
}

// Compiler and version, e.g. "GCC 14.2", "Clang 20.1", "MSVC 14.45".
inline std::string compiler_name()
{
    char buffer[64] {};

    #if defined(__INTEL_LLVM_COMPILER)
    std::snprintf(buffer, sizeof(buffer), "Intel oneAPI %d", __INTEL_LLVM_COMPILER);
    #elif defined(__apple_build_version__)
    std::snprintf(buffer, sizeof(buffer), "Apple Clang %d.%d", __clang_major__, __clang_minor__);
    #elif defined(__clang__)
    std::snprintf(buffer, sizeof(buffer), "Clang %d.%d", __clang_major__, __clang_minor__);
    #elif defined(_MSC_VER)
    // Toolset spelling: _MSC_VER 1945 is MSVC 14.45.
    std::snprintf(buffer, sizeof(buffer), "MSVC %d.%d", _MSC_VER / 100 - 5, _MSC_VER % 100);
    #elif defined(__GNUC__)
    std::snprintf(buffer, sizeof(buffer), "GCC %d.%d", __GNUC__, __GNUC_MINOR__);
    #else
    std::snprintf(buffer, sizeof(buffer), "Unknown compiler");
    #endif

    return std::string{buffer};
}

// The language standard in use. MSVC only reports it through _MSVC_LANG unless
// /Zc:__cplusplus is given.
inline long cxxstd() noexcept
{
    #if defined(_MSVC_LANG)
    return static_cast<long>(_MSVC_LANG);
    #else
    return static_cast<long>(__cplusplus);
    #endif
}

// Escapes the two characters that cannot appear raw in a JSON string. The labels
// we emit are plain ASCII type names, so this is belt and braces.
inline std::string escape(const std::string& value)
{
    std::string result {};
    result.reserve(value.size());

    for (const auto character : value)
    {
        if (character == '"' || character == '\\')
        {
            result.push_back('\\');
        }

        result.push_back(character);
    }

    return result;
}

// The implementations in the order they were first measured, which is the column
// order used by the generated documentation tables.
inline std::vector<std::string> implementations()
{
    std::vector<std::string> result {};

    for (const auto& entry : samples())
    {
        auto seen = false;
        for (const auto& name : result)
        {
            if (name == entry.implementation)
            {
                seen = true;
                break;
            }
        }

        if (!seen)
        {
            result.push_back(entry.implementation);
        }
    }

    return result;
}

// Writes the collected samples to config().json_path; does nothing when --json
// was not given. Returns false only if the file could not be written.
inline bool write_json(const metadata& meta)
{
    const auto& path = config().json_path;
    if (path.empty())
    {
        return true;
    }

    std::ofstream out {path.c_str()};
    if (!out)
    {
        std::cerr << "error: cannot open " << path << " for writing" << std::endl;
        return false;
    }

    out << "{\n";
    out << "  \"schema\": \"boost.int128.benchmarks/1\",\n";
    out << "  \"type\": \"" << escape(meta.type) << "\",\n";
    out << "  \"sign\": \"" << escape(meta.sign) << "\",\n";
    out << "  \"os\": \"" << os_name() << "\",\n";
    out << "  \"arch\": \"" << arch_name() << "\",\n";
    out << "  \"compiler\": \"" << escape(compiler_name()) << "\",\n";
    out << "  \"cxxstd\": " << cxxstd() << ",\n";
    out << "  \"elements\": " << config().elements << ",\n";
    out << "  \"repetitions\": " << config().repetitions << ",\n";
    out << "  \"baseline\": \"" << escape(meta.baseline) << "\",\n";

    out << "  \"implementations\": [";
    const auto impls = implementations();
    for (std::size_t i {}; i < impls.size(); ++i)
    {
        out << (i == 0 ? "" : ", ") << '"' << escape(impls[i]) << '"';
    }
    out << "],\n";

    out << "  \"results\": [\n";
    const auto& all = samples();
    for (std::size_t i {}; i < all.size(); ++i)
    {
        out << "    {\"group\": \"" << escape(all[i].group)
            << "\", \"operation\": \"" << escape(all[i].operation)
            << "\", \"implementation\": \"" << escape(all[i].implementation)
            << "\", \"microseconds\": " << all[i].microseconds << "}"
            << (i + 1 == all.size() ? "\n" : ",\n");
    }
    out << "  ]\n";
    out << "}\n";

    if (!out)
    {
        std::cerr << "error: failed while writing " << path << std::endl;
        return false;
    }

    std::cerr << "wrote " << all.size() << " timings to " << path << std::endl;
    return true;
}

inline void usage(const char* program)
{
    std::cerr << "usage: " << program << " [--json <path>] [--elements <count>] [--repetitions <count>]\n"
              << "  --json         write the run out as a documentation data set\n"
              << "  --elements     values per vector (default 20000000)\n"
              << "  --repetitions  passes over each vector (default 5)" << std::endl;
}

// Parses the command line, exiting on anything unrecognized so that a typo in a
// CI script cannot silently produce a data set with the wrong element count.
inline void parse_options(const int argc, char* argv[])
{
    for (auto i = 1; i < argc; ++i)
    {
        const auto value = [argc, argv, &i]() -> const char*
        {
            if (i + 1 >= argc)
            {
                std::cerr << "error: " << argv[i] << " requires a value" << std::endl;
                std::exit(2);
            }

            return argv[++i];
        };

        if (std::strcmp(argv[i], "--json") == 0)
        {
            config().json_path = value();
        }
        else if (std::strcmp(argv[i], "--elements") == 0)
        {
            config().elements = static_cast<std::size_t>(std::strtoull(value(), nullptr, 10));
        }
        else if (std::strcmp(argv[i], "--repetitions") == 0)
        {
            config().repetitions = static_cast<unsigned>(std::strtoul(value(), nullptr, 10));
        }
        else
        {
            usage(argv[0]);
            std::exit(std::strcmp(argv[i], "--help") == 0 ? 0 : 2);
        }
    }

    if (config().elements < 2U || config().repetitions == 0U)
    {
        std::cerr << "error: --elements must be at least 2 and --repetitions at least 1" << std::endl;
        std::exit(2);
    }
}

} // namespace bench

#endif // BOOST_INT128_TEST_BENCHMARK_RESULTS_HPP
