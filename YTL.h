#pragma once

#include "Header_Basic.h"

namespace YTL
{
	// General
	template <class T> struct TypeHolder {};

	template <class C, class V>
	using PtM= V C::*;

	// Adder (with argument)
	template <class T, class F, T... Some>
	struct Adder;
	template <class T, class F, T A>
	struct Adder<T, F, A> { static constexpr T value = A; };
	template <class T, class F, T A, T... Rest>
	struct Adder<T, F, A, Rest...> { static constexpr T value = F()(A, Adder<T, F, Rest...>::value); };

	template <int... Args>
	static constexpr int Adder_Int = Adder<int, std::plus<int>, Args...>::value;
	template<>
	static constexpr int Adder_Int<> = 0;

	// Sequence Generation
	template<int ...>
	struct Ints { };

	template<int A, int B, int ...S>
	struct GenAtoB : GenAtoB<A, B - 1, B, S...> { };

	template<int A, int ...S>
	struct GenAtoB<A, A, S...> {
		typedef Ints<A, S...> ints;
	};

	template<> // this is useful for GenAtoB<0, n-1>
	struct GenAtoB<0, -1> { typedef Ints<> ints; };

	//Patter matchings
	template<int Value>
	struct ConstInt {
		constexpr static int value = Value;
	};
	template<int... S, class F>
	auto PM_Ints(Ints<S...>, F&& func) {
		return func(ConstInt<S>()...);
	}

	// Tuple split
	template <int Start, int End, typename... Args>
	auto Tuple_Split(const tuple<Args...>& t)
	{
		static constexpr int n = sizeof...(Args);
		static_assert(Start >= 0 && End < n);
		static constexpr auto range = GenAtoB<Start, End>::ints();

		auto res = PM_Ints(range, [&](auto... S)
			{
				return make_tuple(get<S.value>(t)...);
			});
		return res;
	}

	/* ----- TUPLE TYPE MODIFICATION ----- */
	template <class, class>
	struct Tuple_Cat_Type;
	template <class... First, class... Second>
	struct Tuple_Cat_Type<std::tuple<First...>, std::tuple<Second...>> {
		using type = std::tuple<First..., Second...>;
	};

	/*
	NOTE : the standard says
	"A partially specialized non-type argument expression shall not involve a template parameter of the partial specialization except when the argument expression is a simple identifier."
	so we need index wrapper
	*/
	template<size_t N>
	using Tuple_Index = std::integral_constant<size_t, N>;

	template <typename Start, typename End, class Tuple>
	struct Tuple_Split_Type_;

	template <size_t Start, size_t End, typename ArgFirst, typename... Args>
	struct Tuple_Split_Type_<Tuple_Index<Start>, Tuple_Index<End>, tuple<ArgFirst, Args...>>
	{
		using type = typename Tuple_Split_Type_<Tuple_Index<Start - 1>, Tuple_Index<End - 1>, tuple<Args...>>::type;
	};
	template <size_t End, typename ArgFirst, typename... Args>
	struct Tuple_Split_Type_<Tuple_Index<0>, Tuple_Index<End>, tuple<ArgFirst, Args...>>
	{
		using type = typename Tuple_Cat_Type<tuple<ArgFirst>,
			typename Tuple_Split_Type_<Tuple_Index<0>, Tuple_Index<End - 1>, tuple<Args...>>::type>::type;
	};
	template <typename ArgFirst, typename... Args>
	struct Tuple_Split_Type_<Tuple_Index<0>, Tuple_Index<0>, tuple<ArgFirst, Args...>>
	{
		using type = typename tuple<ArgFirst>;
	};
	template <> // empty case
	struct Tuple_Split_Type_<Tuple_Index<0>, Tuple_Index<0>, tuple<>>
	{
		using type = typename tuple<>;
	};
	template <size_t Start, size_t End, class Tuple>
	using Tuple_Split_Type = typename Tuple_Split_Type_<Tuple_Index<Start>, Tuple_Index<End>, Tuple>::type;

	// iterate N types to make tuple
	template <size_t I, typename T>
	struct tuple_n {
		template< typename...Args> using type = typename tuple_n<I - 1, T>::template type<T, Args...>;
	};

	template <typename T>
	struct tuple_n<0, T> {
		template<typename...Args> using type = std::tuple<Args...>;
	};
	template <typename T, size_t N>  using TupleNTimes = typename tuple_n<N, T>::template type<>;



	// Capture 'this'
	template <typename T, typename R, typename... args>
	function<R(args...)> CaptureMF(T* t, R(T::* f)(args...))
	{
		return [=](args... arg)->R {return (*t.*f)(arg...); };
	}

	// Tagging
	template<class T, auto tag_>
	struct Tag
	{
		typedef T xtype; typedef decltype(tag_) ttype;
		T x;
		static const decltype(tag_) tag = tag_;
	};

	// Retrieve type of pointee of "pointer to member"
	template<typename T, auto T::* p>
	struct RetrieveMP
	{
		typedef typename std::remove_reference<decltype(((T*)(nullptr))->*p)>::type type;
	};

	// Take function and make type of corresponding tuple
	template<typename Res, typename... Args>
	struct ParamToTuple;
	template<typename Res, typename... Args>
	struct ParamToTuple < Res(Args...) >
	{
		using type = tuple<Args...>;
	};
	template<typename C, typename Res, typename... Args>
	struct ParamToTuple < Res(C::*)(Args...) >
	{
		using type = tuple<Args...>;
	};
	template<typename Res, typename... Args>
	struct ParamToTuple < function<Res(Args...)> >
	{
		using type = tuple<Args...>;
	};

	// multiple is_same
	template<typename T, typename... Rest>
	struct is_any : std::false_type {};
	template<typename T, typename First>
	struct is_any<T, First> : std::is_same<T, First> {};
	template<typename T, typename First, typename... Rest>
	struct is_any<T, First, Rest...>
		: std::integral_constant<bool, std::is_same<T, First>::value || is_any<T, Rest...>::value>
	{};

	/* Variant Support */
	template<typename T>
	struct Variant_No_Overhead
	{
	};
	template<typename... Args>
	struct Variant_No_Overhead<std::variant<Args...>>
	{
		static constexpr bool value = ((!std::is_polymorphic_v<Args>) && ...);
	};



	/* UTILITY */
	class RSDT // 'Reserved Destruction'. useful with RAII
	{
		function<void(void)> V_Func;
	public:
		RSDT(function<void(void)> f) : V_Func(f)
		{
		}
		~RSDT()
		{
			V_Func();
		}
	};

	template <class T>
	inline void hash_combine(std::size_t& seed, T const& v)
	{
		seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}


}

#define PRINT_TYPE(x) typedef typename decltype(x)::TYPE_PRINT TYPE_PRINT##x;


namespace std
{
	namespace // hash of tuple
	{

		// Recursive template code derived from Matthieu M.
		template <class Tuple, size_t Index = std::tuple_size<Tuple>::value - 1>
		struct HashValueImpl
		{
			static void apply(size_t& seed, Tuple const& tuple)
			{
				HashValueImpl<Tuple, Index - 1>::apply(seed, tuple);
				YTL::hash_combine(seed, std::get<Index>(tuple));
			}
		};

		template <class Tuple>
		struct HashValueImpl<Tuple, 0>
		{
			static void apply(size_t& seed, Tuple const& tuple)
			{
				YTL::hash_combine(seed, std::get<0>(tuple));
			}
		};
	}

	template <typename ... TT>
	struct hash<std::tuple<TT...>>
	{
		size_t operator()(std::tuple<TT...> const& tt) const
		{
			size_t seed = 0;
			HashValueImpl<std::tuple<TT...> >::apply(seed, tt);
			return seed;
		}
	};

	template <class F, class S>
	struct hash<std::pair<F, S>>
	{
		size_t operator()(std::pair<F, S> const& x) const
		{
			auto seed = hash<F>()(x.first);
			YTL::hash_combine(seed, x.second);
			return seed;
		}
	};
}

#define MACRO_ARGS_NUM(...) (std::tuple_size<decltype(make_tuple(__VA_ARGS__))>::value)


