#ifndef BOOST_INT128_INT128_HPP
#define BOOST_INT128_IBOOST_INT128T128_HPP

#include <boost/int128/detail/config.hpp>
#include <boost/int128/detail/fwd.hpp>
#include <cstdint>

namespace boost {
namespace int128 {

struct
    #ifdef BOOST_INT128_HAS_INT128
    alignas(alignof(__int128))
    #else
    alignas(16)
    #endif
int128_t
{
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::int64_t high {};
    #else
    std::int64_t high {};
    std::uint64_t low {};
    #endif

    // Defaulted Constructors
    constexpr int128_t() noexcept = default;
    constexpr int128_t(const int128_t& other) noexcept = default;
    constexpr int128_t(int128_t&& other) noexcept = default;
    constexpr int128_t& operator=(const int128_t& other) noexcept = default;
    constexpr int128_t& operator=(int128_t&& other) noexcept = default;
    constexpr ~int128_t() noexcept = default;

    #ifdef BOOST_INT128_ENDIAN_LITTLE_BYTE

    constexpr int128_t(std::int64_t high_, std::uint64_t low_) noexcept : low {low_}, high {high_} {}
    constexpr int128_t(const uint128_t& v) noexcept;
    explicit constexpr int128_t(std::uint64_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint32_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint16_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint8_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::int64_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int32_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int16_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int8_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}

    #else

    constexpr int128_t(std::int64_t high_, std::uint64_t low_) noexcept : high {high_}, low {low_} {}
    constexpr int128_t(const int128_t& v) noexcept;
    explicit constexpr int128_t(std::uint64_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint32_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint16_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint8_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::int64_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int32_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int16_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int8_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}

    #endif

};

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_INT128_HPP
#include <boost/int128/detail/fwd.hpp>
#include <cstdint>

namespace boost {
namespace int128 {

struct
    #ifdef BOOST_INT128_HAS_INT128
    alignas(alignof(__int128))
    #else
    alignas(16)
    #endif
int128_t
{
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::int64_t high {};
    #else
    std::int64_t high {};
    std::uint64_t low {};
    #endif

    // Defaulted Constructors
    constexpr int128_t() noexcept = default;
    constexpr int128_t(const int128_t& other) noexcept = default;
    constexpr int128_t(int128_t&& other) noexcept = default;
    constexpr int128_t& operator=(const int128_t& other) noexcept = default;
    constexpr int128_t& operator=(int128_t&& other) noexcept = default;
    constexpr ~int128_t() noexcept = default;

    #ifdef BOOST_INT128_ENDIAN_LITTLE_BYTE

    constexpr int128_t(std::int64_t high_, std::uint64_t low_) noexcept : low {low_}, high {high_} {}
    constexpr int128_t(const uint128_t& v) noexcept;
    explicit constexpr int128_t(std::uint64_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint32_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint16_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint8_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::int64_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int32_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int16_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int8_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}

    #else

    constexpr int128_t(std::int64_t high_, std::uint64_t low_) noexcept : high {high_}, low {low_} {}
    constexpr int128_t(const int128_t& v) noexcept;
    explicit constexpr int128_t(std::uint64_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint32_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint16_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint8_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::int64_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int32_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int16_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int8_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}

    #endif

};

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_INT128_HPP
#include <boost/int128/detail/fwd.hpp>
#include <cstdint>

namespace boost {
namespace int128 {

struct
    #ifdef BOOST_INT128_HAS_INT128
    alignas(alignof(__int128))
    #else
    alignas(16)
    #endif
int128_t
{
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::int64_t high {};
    #else
    std::int64_t high {};
    std::uint64_t low {};
    #endif

    // Defaulted Constructors
    constexpr int128_t() noexcept = default;
    constexpr int128_t(const int128_t& other) noexcept = default;
    constexpr int128_t(int128_t&& other) noexcept = default;
    constexpr int128_t& operator=(const int128_t& other) noexcept = default;
    constexpr int128_t& operator=(int128_t&& other) noexcept = default;
    constexpr ~int128_t() noexcept = default;

    #ifdef BOOST_INT128_ENDIAN_LITTLE_BYTE

    constexpr int128_t(std::int64_t high_, std::uint64_t low_) noexcept : low {low_}, high {high_} {}
    constexpr int128_t(const uint128_t& v) noexcept;
    explicit constexpr int128_t(std::uint64_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint32_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint16_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint8_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::int64_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int32_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int16_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int8_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}

    #else

    constexpr int128_t(std::int64_t high_, std::uint64_t low_) noexcept : high {high_}, low {low_} {}
    constexpr int128_t(const int128_t& v) noexcept;
    explicit constexpr int128_t(std::uint64_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint32_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint16_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint8_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::int64_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int32_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int16_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int8_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}

    #endif

};

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_INT128_HPP
#include <boost/int128/detail/fwd.hpp>
#include <cstdint>

namespace boost {
namespace int128 {

struct
    #ifdef BOOST_INT128_HAS_INT128
    alignas(alignof(__int128))
    #else
    alignas(16)
    #endif
int128_t
{
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::int64_t high {};
    #else
    std::int64_t high {};
    std::uint64_t low {};
    #endif

    // Defaulted Constructors
    constexpr int128_t() noexcept = default;
    constexpr int128_t(const int128_t& other) noexcept = default;
    constexpr int128_t(int128_t&& other) noexcept = default;
    constexpr int128_t& operator=(const int128_t& other) noexcept = default;
    constexpr int128_t& operator=(int128_t&& other) noexcept = default;
    constexpr ~int128_t() noexcept = default;

    #ifdef BOOST_INT128_ENDIAN_LITTLE_BYTE

    constexpr int128_t(std::int64_t high_, std::uint64_t low_) noexcept : low {low_}, high {high_} {}
    constexpr int128_t(const uint128_t& v) noexcept;
    explicit constexpr int128_t(std::uint64_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint32_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint16_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint8_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::int64_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int32_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int16_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int8_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}

    #else

    constexpr int128_t(std::int64_t high_, std::uint64_t low_) noexcept : high {high_}, low {low_} {}
    constexpr int128_t(const int128_t& v) noexcept;
    explicit constexpr int128_t(std::uint64_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint32_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint16_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint8_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::int64_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int32_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int16_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int8_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}

    #endif

};

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_INT128_HPP
#include <boost/int128/detail/fwd.hpp>
#include <cstdint>

namespace boost {
namespace int128 {

struct
    #ifdef BOOST_INT128_HAS_INT128
    alignas(alignof(__int128))
    #else
    alignas(16)
    #endif
int128_t
{
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::int64_t high {};
    #else
    std::int64_t high {};
    std::uint64_t low {};
    #endif

    // Defaulted Constructors
    constexpr int128_t() noexcept = default;
    constexpr int128_t(const int128_t& other) noexcept = default;
    constexpr int128_t(int128_t&& other) noexcept = default;
    constexpr int128_t& operator=(const int128_t& other) noexcept = default;
    constexpr int128_t& operator=(int128_t&& other) noexcept = default;
    constexpr ~int128_t() noexcept = default;

    #ifdef BOOST_INT128_ENDIAN_LITTLE_BYTE

    constexpr int128_t(std::int64_t high_, std::uint64_t low_) noexcept : low {low_}, high {high_} {}
    constexpr int128_t(const uint128_t& v) noexcept;
    explicit constexpr int128_t(std::uint64_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint32_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint16_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint8_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::int64_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int32_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int16_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int8_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}

    #else

    constexpr int128_t(std::int64_t high_, std::uint64_t low_) noexcept : high {high_}, low {low_} {}
    constexpr int128_t(const int128_t& v) noexcept;
    explicit constexpr int128_t(std::uint64_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint32_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint16_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint8_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::int64_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int32_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int16_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int8_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}

    #endif

};

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_INT128_HPPw.boost.org/LICENSE_1_0.txt

#ifndef BOOST_INT128_INT128_HPP
#define BOOST_INT128_INT128_HPP

#include <boost/int128/detail/config.hpp>
#include <boost/int128/detail/fwd.hpp>
#include <cstdint>

namespace boost {
namespace int128 {

struct
    #ifdef BOOST_INT128_HAS_INT128
    alignas(alignof(__int128))
    #else
    alignas(16)
    #endif
int128_t
{
    #if BOOST_INT128_ENDIAN_LITTLE_BYTE
    std::uint64_t low {};
    std::int64_t high {};
    #else
    std::int64_t high {};
    std::uint64_t low {};
    #endif

    // Defaulted Constructors
    constexpr int128_t() noexcept = default;
    constexpr int128_t(const int128_t& other) noexcept = default;
    constexpr int128_t(int128_t&& other) noexcept = default;
    constexpr int128_t& operator=(const int128_t& other) noexcept = default;
    constexpr int128_t& operator=(int128_t&& other) noexcept = default;
    constexpr ~int128_t() noexcept = default;

    #ifdef BOOST_INT128_ENDIAN_LITTLE_BYTE

    constexpr int128_t(std::int64_t high_, std::uint64_t low_) noexcept : low {low_}, high {high_} {}
    constexpr int128_t(const uint128_t& v) noexcept;
    explicit constexpr int128_t(std::uint64_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint32_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint16_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::uint8_t v) noexcept : low {v}, high {} {}
    explicit constexpr int128_t(std::int64_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int32_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int16_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}
    explicit constexpr int128_t(std::int8_t v) noexcept : low{static_cast<std::uint64_t>(v)}, high{v < 0 ? -1 : 0} {}

    #else

    constexpr int128_t(std::int64_t high_, std::uint64_t low_) noexcept : high {high_}, low {low_} {}
    constexpr int128_t(const int128_t& v) noexcept;
    explicit constexpr int128_t(std::uint64_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint32_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint16_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::uint8_t v) noexcept : high {}, low {v} {}
    explicit constexpr int128_t(std::int64_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int32_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int16_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}
    explicit constexpr int128_t(std::int8_t v) noexcept : high{v < 0 ? -1 : 0}, low{static_cast<std::uint64_t>(v)} {}

    #endif

};

} // namespace int128
} // namespace boost

#endif // BOOST_INT128_INT128_HPP
