#include <cstdint>
#include <limits>
#include <tuple>
#include <type_traits>

// lifted from: https://github.com/torvalds/linux/blob/master/lib/div64.c#L142
template<typename Integer>
constexpr auto divide(Integer dividend, Integer divisor)
-> std::enable_if_t<std::is_unsigned<Integer>::value, Integer>
{
    auto half_digits = std::numeric_limits<Integer>::digits / 2;

    auto div64_32 = [half_digits](Integer dividend, std::uint32_t divisor) {
        Integer rem = dividend;
        Integer b = divisor;
        Integer d = 1;
        std::uint32_t high = rem >> half_digits;

        Integer quot = 0;
        if (high>=divisor) {
            high /= divisor;
            quot = (Integer) high << half_digits;
            rem -= (Integer) (high*divisor) << half_digits;
        }

        while ((int64_t) b>0 && b<rem) {
            b = b+b;
            d = d+d;
        }

        do {
            if (rem>=b) {
                rem -= b;
                quot += d;
            }
            b >>= 1;
            d >>= 1;
        }
        while (d);

        return std::make_tuple(quot, rem);
    };

    std::uint32_t high = divisor >> half_digits;
    if (high==0) {
        auto[quot, rem] = div64_32(dividend, divisor);
        return quot;
    }

    auto fls = [half_digits](std::uint32_t n) {
        if (n==0) {
            return 0;
        }
        for (int r = half_digits;; n <<= 1, r--) {
            if (n & (1ULL << (half_digits-1))) {
                return r;
            }
        }
    };

    int n = fls(high);
    auto[quot, rem] = div64_32(dividend >> n, divisor >> n);

    if (quot!=0) {
        quot--;
    }
    if ((dividend-quot*divisor)>=divisor) {
        quot++;
    }

    return quot;
}

// requires: divisor is non-zero; operands are not most negative number
template<typename Integer>
constexpr auto divide(Integer dividend, Integer divisor)
-> std::enable_if_t<std::is_signed<Integer>::value, Integer>
{
    using unsigned_integer = std::make_unsigned_t<Integer>;
    return (dividend>=0)
           ? (divisor>=0)
             ? divide<unsigned_integer>(dividend, divisor)
             : -divide<unsigned_integer>(dividend, -divisor)
           : (divisor>=0)
             ? -divide<unsigned_integer>(-dividend, divisor)
             : divide<unsigned_integer>(-dividend, -divisor);
}
