#include <cassert>
#include <climits>
#include <iostream>
#include <random>
#include <string>

#include "long_division.h"

static_assert(divide(9223372036854775807LL, 4402021551LL)==2095258264LL, "");

static_assert(divide(INT8_C(-22), INT8_C(3))==-7, "");
static_assert(divide(INT8_C(0), INT8_C(-102))==0, "");
static_assert(divide(INT8_C(16), INT8_C(-31))==0, "");
static_assert(divide(INT8_C(3), INT8_C(2))==1, "");
static_assert(divide(UINT8_C(14), UINT8_C(1))==14, "");
static_assert(divide(UINT8_C(90), UINT8_C(2))==45, "");
static_assert(divide(UINT8_C(29), UINT8_C(102))==0, "");
static_assert(divide(UINT8_C(0), UINT8_C(19))==0, "");
static_assert(divide(INT16_C(232), INT16_C(-1392))==0, "");
static_assert(divide(INT16_C(-4), INT16_C(2316))==0, "");
static_assert(divide(INT16_C(-377), INT16_C(384))==0, "");
static_assert(divide(INT16_C(14738), INT16_C(36))==409, "");
static_assert(divide(UINT16_C(19445), UINT16_C(10302))==1, "");
static_assert(divide(UINT16_C(3209), UINT16_C(2))==1604, "");
static_assert(divide(UINT16_C(26), UINT16_C(3184))==0, "");
static_assert(divide(UINT16_C(1), UINT16_C(6))==0, "");
static_assert(divide(INT32_C(112271), INT32_C(13355287))==0, "");
static_assert(divide(INT32_C(-9636934), INT32_C(-1728553491))==0, "");
static_assert(divide(INT32_C(1), INT32_C(115225709))==0, "");
static_assert(divide(INT32_C(-8079727), INT32_C(-240))==33665, "");
static_assert(divide(UINT32_C(656), UINT32_C(93286560))==0, "");
static_assert(divide(UINT32_C(257560), UINT32_C(930760))==0, "");
static_assert(divide(UINT32_C(250179), UINT32_C(25062))==9, "");
static_assert(divide(UINT32_C(8776), UINT32_C(62))==141, "");
static_assert(divide(INT64_C(-1311323196360972), INT64_C(2063185694))==-635581, "");
static_assert(divide(INT64_C(-10), INT64_C(14075170))==0, "");
static_assert(divide(INT64_C(4782793), INT64_C(-116989223491211349))==0, "");
static_assert(divide(UINT64_C(16748969), UINT64_C(11))==1522633, "");
static_assert(divide(UINT64_C(6), UINT64_C(1848998660))==0, "");
static_assert(divide(UINT64_C(8895), UINT64_C(2842315490253321074))==0, "");
static_assert(divide(UINT64_C(473301195), UINT64_C(4344847881292))==0, "");

template<typename Integer>
void test_divide(Integer const dividend, Integer const divisor)
{
    if (!divisor) {
        return;
    }

    if (std::is_signed<Integer>::value) {
        assert(dividend!=std::numeric_limits<Integer>::lowest());
        assert(divisor!=std::numeric_limits<Integer>::lowest());
    }

    auto const expected = dividend/divisor;
    auto const actual = divide(dividend, divisor);
    assert(expected==actual);
}

template<typename Integer>
void test_modulus(Integer const dividend, Integer const divisor)
{
    if (!divisor) {
        return;
    }

    if (std::is_signed<Integer>::value) {
        assert(dividend!=std::numeric_limits<Integer>::lowest());
        assert(divisor!=std::numeric_limits<Integer>::lowest());
    }

    auto const expected = dividend/divisor;
    auto const actual = modulus(dividend, divisor);
    assert(expected==actual);
}

template<typename Integer>
void test_divide(char const* macro)
{
    auto rd { std::random_device{} };
    auto e2 { std::mt19937_64(rd()) };

    // Most negative number (INT_MIN) is not supported but next lowest value (-INT_MAX) is.
    auto const lowest { Integer{std::numeric_limits<Integer>::lowest()+std::is_signed<Integer>::value} };
    auto const highest { std::numeric_limits<Integer>::max() };

    std::uniform_int_distribution<Integer> dist(lowest, highest);

    for (int i { 1000000 }; i; --i) {
        auto const r1 { dist(e2) };
        auto const r2 { dist(e2) & (sizeof(Integer)*CHAR_BIT-1) };
        auto const r3 { dist(e2) };
        auto const r4 { dist(e2) & (sizeof(Integer)*CHAR_BIT-1) };
        auto const dividend { static_cast<Integer>(static_cast<Integer>(r1) >> r2) };
        auto const divisor { static_cast<Integer>(static_cast<Integer>(r3) >> r4) };
        if (divisor==0) {
            continue;
        }

        test_divide(dividend, divisor);

        test_divide(lowest, divisor);
        test_divide(Integer{0}, divisor);
        test_divide(highest, divisor);

        test_divide(dividend, lowest);
        test_divide(dividend, highest);

        if (i<5) {
            std::cout << "static_assert(divide(" << macro << '(' << std::to_string(dividend) << "), " << macro << '('
                      << std::to_string(divisor) << "))=="
                      << (dividend/divisor) << ", \"\");\n";
        }
    }
}

int main()
{
    test_divide<std::int8_t>("INT8_C");
    test_divide<std::uint8_t>("UINT8_C");
    test_divide<std::int16_t>("INT16_C");
    test_divide<std::uint16_t>("UINT16_C");
    test_divide<std::int32_t>("INT32_C");
    test_divide<std::uint32_t>("UINT32_C");
    test_divide<std::int64_t>("INT64_C");
    test_divide<std::uint64_t>("UINT64_C");
}
