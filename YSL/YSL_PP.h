#pragma once

#include "YSL_Struct.h"

// https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
#define YSL_EXPAND(x) x

#define YSL_PACK_S0(T) YSL_PACK()
#define YSL_PACK_S1(T, A0) YSL_PACK(&T::A0)
#define YSL_PACK_S2(T, A0, A1) YSL_PACK(&T::A0, &T::A1)
#define YSL_PACK_S3(T, A0, A1, A2) YSL_PACK(&T::A0, &T::A1, &T::A2)
#define YSL_PACK_S4(T, A0, A1, A2, A3) YSL_PACK(&T::A0, &T::A1, &T::A2, &T::A3)
#define YSL_PACK_S5(T, A0, A1, A2, A3, A4) YSL_PACK(&T::A0, &T::A1, &T::A2, &T::A3, &T::A4)
#define YSL_PACK_S6(T, A0, A1, A2, A3, A4, A5) YSL_PACK(&T::A0, &T::A1, &T::A2, &T::A3, &T::A4, &T::A5)
#define YSL_PACK_S7(T, A0, A1, A2, A3, A4, A5, A6) YSL_PACK(&T::A0, &T::A1, &T::A2, &T::A3, &T::A4, &T::A5, &T::A6)
#define YSL_PACK_S8(T, A0, A1, A2, A3, A4, A5, A6, A7) YSL_PACK(&T::A0, &T::A1, &T::A2, &T::A3, &T::A4, &T::A5, &T::A6, &T::A7)
#define YSL_PACK_S9(T, A0, A1, A2, A3, A4, A5, A6, A7, A8) YSL_PACK(&T::A0, &T::A1, &T::A2, &T::A3, &T::A4, &T::A5, &T::A6, &T::A7, &T::A8)
#define YSL_PACK_S10(T, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) YSL_PACK(&T::A0, &T::A1, &T::A2, &T::A3, &T::A4, &T::A5, &T::A6, &T::A7, &T::A8, &T::A9)
#define YSL_PACK_S11(T, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) YSL_PACK(&T::A0, &T::A1, &T::A2, &T::A3, &T::A4, &T::A5, &T::A6, &T::A7, &T::A8, &T::A9, &T::A10)
#define YSL_PACK_S12(T, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11) YSL_PACK(&T::A0, &T::A1, &T::A2, &T::A3, &T::A4, &T::A5, &T::A6, &T::A7, &T::A8, &T::A9, &T::A10, &T::A11)
#define YSL_PACK_S13(T, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12) YSL_PACK(&T::A0, &T::A1, &T::A2, &T::A3, &T::A4, &T::A5, &T::A6, &T::A7, &T::A8, &T::A9, &T::A10, &T::A11, &T::A12)
#define YSL_PACK_S14(T, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13) YSL_PACK(&T::A0, &T::A1, &T::A2, &T::A3, &T::A4, &T::A5, &T::A6, &T::A7, &T::A8, &T::A9, &T::A10, &T::A11, &T::A12, &T::A13)
#define YSL_PACK_S15(T, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14) YSL_PACK(&T::A0, &T::A1, &T::A2, &T::A3, &T::A4, &T::A5, &T::A6, &T::A7, &T::A8, &T::A9, &T::A10, &T::A11, &T::A12, &T::A13, &T::A14)
#define YSL_GET_VARIADIC_MACRO( _0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, NAME, ...) NAME
#define YSL_PACK_S(T, ...) YSL_EXPAND(YSL_GET_VARIADIC_MACRO(_0, ##__VA_ARGS__, YSL_PACK_S15, YSL_PACK_S14, YSL_PACK_S13, YSL_PACK_S12, YSL_PACK_S11, YSL_PACK_S10, YSL_PACK_S9, YSL_PACK_S8, YSL_PACK_S7, YSL_PACK_S6, YSL_PACK_S5, YSL_PACK_S4, YSL_PACK_S3, YSL_PACK_S2, YSL_PACK_S1, YSL_PACK_S0) (T, __VA_ARGS__))
