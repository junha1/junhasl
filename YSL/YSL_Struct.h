#pragma once

#include "YSL_SerializationConfiguration.h"
#include "../YTL/YTL.h"
#include "YSL_Common.h"
#include "YSL_Types.h"

namespace ysl
{
// This is supposed to be working properly in C++ but Visual fucking studio has serious bug.
#define YSL_PACK_FV(...) static constexpr auto ysl_mps = make_tuple(__VA_ARGS__);\
static constexpr bool ysl_struct_trival = true;

#define YSL_PACK(...) static constexpr auto ysl_mps() {return make_tuple(__VA_ARGS__);}\
static constexpr bool ysl_struct_trival = true;

	template <class T, typename... Args>
	static tuple<Args& ...> ysl_pack(T* t, const tuple<Args T::* ...>& mpt)
	{
		static constexpr int n = sizeof...(Args);
		if constexpr (n == 0)
		{
			return tuple{};
		}
		else
		{
			static constexpr auto range = YTL::GenAtoB<0, n - 1>::ints();
			return YTL::PM_Ints(range, [&](auto... S)
				{
					return std::tie(t->*get<S.value>(mpt)...);
				});
		}
	}
	// const
	template <class T, typename... Args>
	static tuple<const Args& ...> ysl_pack(const T* t, const tuple<Args T::* ...>& mpt)
	{
		static constexpr int n = sizeof...(Args);
		if constexpr (n == 0)
		{
			return tuple{};
		}
		else
		{
			static constexpr auto range = YTL::GenAtoB<0, n - 1>::ints();
			return YTL::PM_Ints(range, [&](auto... S)
				{
					return std::tie(t->*get<S.value>(mpt)...);
				});
		}
	}

	struct ysl_struct_base
	{
		// redeclare to true if your struct has only 1 member and you want to use it as itself (not in a tuple)
		using tuple_unpack_for_single_member = std::bool_constant<false>; 
		void ysl_loads_notify() {}
	};
	struct ysl_struct : public ysl_struct_base
	{
		virtual ysl_struct* ysl_factory() const = 0;
		virtual yobject ysl_dumps() const = 0;
		virtual void ysl_loads(const yobject& p) = 0;
		virtual void ysl_loads_notify() = 0;
	};

	template <class T>
	auto ysl_struct_tuple(T* t) // returning reference is tricky. always by value.
	{
		using B = typename T::ysl_parent;
		if constexpr (std::is_same_v<B, void> || std::is_same_v<B, ysl_struct_base>)
		{
			if constexpr (T::tuple_unpack_for_single_member::value == false)
				return ysl_pack<T>(t, T::ysl_mps()); // <T> needed when mps is empty
			else
			{
				static_assert(std::tuple_size_v<decltype(T::ysl_mps())> == 1);
				return ref(t->*get<0>(T::ysl_mps())); // no tuple
			}
		}
		else
		{
			static_assert(T::tuple_unpack_for_single_member::value == false);
			if constexpr (std::tuple_size<decltype(T::ysl_mps())>::value == 0)
				return ysl_struct_tuple<B>(t);
			else
				return make_tuple(ysl_struct_tuple<B>(t), ysl_pack<T>(t, T::ysl_mps()));
		}
	}
	template <class T>
	decltype(auto) ysl_struct_tuple(const T* t)
	{
		using B = typename T::ysl_parent;
		if constexpr (std::is_same_v<B, void> || std::is_same_v<B, ysl_struct_base>)
		{
			if constexpr (T::tuple_unpack_for_single_member::value == false)
				return ysl_pack<T>(t, T::ysl_mps()); // <T> needed when mps is empty
			else
			{
				static_assert(std::tuple_size_v<decltype(T::ysl_mps())> == 1);
				return t->*get<0>(T::ysl_mps()); // no tuple
			}
		}
		else
		{
			static_assert(T::tuple_unpack_for_single_member::value == false);
			if constexpr (std::tuple_size<decltype(T::ysl_mps())>::value == 0)
				return ysl_struct_tuple<B>(t);
			else
				return make_tuple(ysl_struct_tuple<B>(t), ysl_pack<T>(t, T::ysl_mps()));
		}
	}

	template <class Data>
	struct ysl_struct_final : public Data
	{
		using ysl_parent = Data;
		YSL_PACK();
		virtual ysl_struct* ysl_factory() const { return new ysl_struct_final(); }
		virtual yobject ysl_dumps() const
		{
			return to(ysl_struct_tuple((Data*)this));
		}
		virtual void ysl_loads(const yobject& p)
		{
			auto t = ysl_struct_tuple((Data*)this);
			from(p, t);
			ysl_loads_notify();
		}
		virtual void ysl_loads_notify() {};

		// constructor pass
		template <typename... Args>
		ysl_struct_final(Args&& ... args) : Data(args...)
		{}
	};
}





