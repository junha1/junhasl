#pragma once

#include "YSL_Conversion_Data.h"
namespace ysl
{
	// Tuple
	template <bool W, typename... Args>
	void to_raw_data(bpos& p, const tuple<Args...>& x)
	{
		constexpr const static int n = sizeof...(Args);
		constexpr const static auto range = YTL::GenAtoB<0, n - 1>::ints();
		constexpr const static char dummy = 0;

		YTL::PM_Ints(range, [&](auto...S)
			{
				vector<char>{(to_raw_data<W>(p, get<S.value>(x)), dummy)...}; // {} guarantees sequential order
			});
	}

	template <bool R, bool W, typename... Args>
	void from_raw_data(hpos_safe& t, hpos_safe& p, tuple<Args...>* x)
	{
		constexpr const static int n = sizeof...(Args);
		constexpr const static auto range = YTL::GenAtoB<0, n - 1>::ints();
		constexpr const static char dummy = 0;

		if constexpr (R)
		{
			if (t.read<FirstType>() != YSLSC_TUPLE) throw ESerialization<decltype(*x)>();
			if (t.read_size().first != n) throw ESerialization<decltype(*x)>();
		}
		else // move only
		{
			t.read<FirstType>();
			t.read_size_pass();
		}

		if constexpr (W)
		{
			YTL::PM_Ints(range, [&](auto...S)
				{
					return vector<char>{(from_raw_data<R, true>(t, p, &get<S.value>(*x)), dummy)...};
				});
		}
		else
		{
			YTL::PM_Ints(range, [&](auto...S)
				{
					return vector<char>{(from_raw_data<true, false>(t, p,
						static_cast<std::remove_reference_t<std::tuple_element_t<S.value, tuple<Args...>>>*>(nullptr)), dummy)...};
				});
		}
	}

	// Variant
	template <bool W, typename... Args>
	void to_raw_data(bpos& p, const variant<Args...>& x)
	{
		constexpr const static int n = sizeof...(Args);
		constexpr const static auto range = YTL::GenAtoB<0, n - 1>::ints();
		constexpr const static char dummy = 0;

		uchar index = x.index();
		to_raw_data<W>(p, (uchar)index);

		YTL::PM_Ints(range, [&](auto...S)
			{
				vector<char>{(S.value == index ? (to_raw_data<W>(p, get<S.value>(x)), dummy) : dummy, dummy)...}; // {} guarantees sequential order
			});
	}

	template <bool R, bool W, typename... Args>
	void from_raw_data(hpos_safe& t, hpos_safe& p, variant<Args...>* x)
	{
		constexpr const static int n = sizeof...(Args);
		constexpr const static auto range = YTL::GenAtoB<0, n - 1>::ints();
		constexpr const static char dummy = 0;

		if constexpr (R)
		{
			if (t.read<FirstType>() != YSLSC_VARIANT) throw ESerialization<decltype(*x)>();
			if (t.read_size().first != n) throw ESerialization<decltype(*x)>();
		}
		else // move only
		{
			t.read<FirstType>();
			t.read_size_pass();
		}

		if constexpr (W)
		{
			uchar index = p.read<uchar>();

			YTL::PM_Ints(range, [&](auto...S)
				{
					return vector<char>{(S.value == index ? (
						[&]()
						{
							x->emplace<S.value>();
							from_raw_data<R, true>(t, p, &get<S.value>(*x));
						}()
						, dummy) : (from_raw_data<R, false>(t, p, static_cast<std::remove_reference_t<std::variant_alternative_t<S.value, variant<Args...>>>*>(nullptr)), dummy), dummy)...};
				});
		}
		else
		{
			// we don't need index checking here! (this is for type pos mocing)
			YTL::PM_Ints(range, [&](auto...S)
				{
					return vector<char>{(from_raw_data<true, false>(t, p,
						static_cast<std::remove_reference_t<std::variant_alternative_t<S.value, variant<Args...>>>*>(nullptr)), dummy)...};
				});
		}
	}
}