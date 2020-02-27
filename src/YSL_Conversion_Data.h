#pragma once
#include "YSL_Conversion_Mark.h"
#include "YSL_Types.h"

namespace ysl
{
	/*
	=====================================
	========= Data Conversion ===========
	=====================================
	*/
	/* --- Forward declaration for composite types --- */
	/*
	- hpos_safe is not a reference : header could be read multiple times (not just-one-read)
	- W : actual writing / R : type checking

	for 'from_raw_':
	1. (None) : invalid
	2. R : type check
	3. W : iterating homogeneous type
	4. RW : Common case

	- note : typepos should always move even when R is false. (primitive casting)
	- note : datapos should not move in none-W mode
	FAQ
	- why 'x' is pointer, not reference, in from_raw_data?
	=> for W = false, 'x' will not be used. (nullptr)
	*/

	template <bool R, class T>
	void type_test(hpos_safe& t, FirstType type, const TypeHolder<T>)
	{
		if constexpr (R)
		{
			if (t.read<FirstType>() != type) throw ESerialization<T>();
		}
		else
		{
			t.read_pass<FirstType>();
		}
	}

	// PAIR (ONLY FOR DICT IMPLEMENTATION, NEVER USE DIRECTLY)
	template <bool W, class K, class V>
	void to_raw_data(bpos& p, const std::pair<K, V>& x);
	template <bool R, bool W, class K, class V>
	void from_raw_data(hpos_safe& t, hpos_safe& p, std::pair<K, V>* x);

	// HOMOGENEOUS
	template <bool W, class V, enable_if_t<is_homogeneous_composite<V>, int> = 0>
	void to_raw_data(bpos& p, const V & x);
	template <bool R, bool W, class V, enable_if_t<is_homogeneous_composite<V>, int> = 0>
	void from_raw_data(hpos_safe& t, hpos_safe & p, V* x);

	// OPT
	template <bool W, class T>
	void to_raw_data(bpos& p, const optional<T>& x);
	template <bool R, bool W, class T>
	void from_raw_data(hpos_safe& t, hpos_safe& p, optional<T>* x);

	// TUPLE
	template <bool W, typename... Args>
	void to_raw_data(bpos& p, const tuple<Args...>& x);
	template <bool R, bool W, typename... Args>
	void from_raw_data(hpos_safe& t, hpos_safe& p, tuple<Args...>* x);

	// VARIANT
	template <bool W, typename... Args>
	void to_raw_data(bpos& p, const variant<Args...>& x);
	template <bool R, bool W, typename... Args>
	void from_raw_data(hpos_safe& t, hpos_safe& p, variant<Args...>* x);

	
	// STRUCT (Trivial)
	template <bool W, class S, enable_if_t<std::is_base_of_v<ysl_struct_base, S> && S::ysl_struct_trival, int> = 0>
	void to_raw_data(bpos & p, const S & x);
	template <bool R, bool W, class S, enable_if_t<std::is_base_of_v<ysl_struct_base, S> && S::ysl_struct_trival, int> = 0>
	void from_raw_data(hpos_safe & t, hpos_safe & p, S* x);

	// STRUCT (Non-Trivial)
	template <bool W, class S, enable_if_t<std::is_base_of_v<ysl_struct_base, S> && !S::ysl_struct_trival, int> = 0>
	void to_raw_data(bpos & p, const S & x);
	template <bool R, bool W, class S, enable_if_t<std::is_base_of_v<ysl_struct_base, S> && !S::ysl_struct_trival, int> = 0>
	void from_raw_data(hpos_safe & t, hpos_safe & p, S* x);

	// Reference
	template <bool W, class T>
	void to_raw_data(bpos& p, const std::reference_wrapper<T>& x);
	template <bool R, bool W, class T>
	void from_raw_data(hpos_safe& t, hpos_safe& p, std::reference_wrapper<T>* x);


	/* --- Actual Definitions --- */
	// Primitive
	template <bool W, class T, enable_if_t<IsPrimitive<T>, int> = 0>
	void to_raw_data(bpos& p, T x)
	{
		if constexpr (W) *(T*)p = x;
		p += sizeof(T);
	}
	// Primitives are special : they are all castable with each other.
	template <bool R, bool W, class T, enable_if_t<IsPrimitive<T>, int> = 0>
	void from_raw_data(hpos_safe& t, hpos_safe& p, T* x)
	{
		FirstType type = t.read<FirstType>();
		read_primitive<R, W, T>(x, type, p); // implicit conversion may occur here
	}

	// Enums
	template <bool W, class T, enable_if_t<std::is_enum_v<T>, int> = 0>
	void to_raw_data(bpos & p, T x)
	{
		to_raw_data<W>(p, (int)x);
	}
	template <bool R, bool W, class T, enable_if_t<std::is_enum_v<T>, int> = 0>
	void from_raw_data(hpos_safe & t, hpos_safe & p, T* x)
	{
		if constexpr (W)
		{
			int e;
			from_raw_data<R, W>(t, p, &e);
			*x = (T)e;
		}
		else
			from_raw_data<R, false>(t, p, static_cast<int*>(nullptr));		
	}

#if (YSLSC_GLM_USE)
	// glm::vec
	template <bool W, class T, int D>
	void to_raw_data(bpos& p, const glm::vec<D, T>& x)
	{
		constexpr static auto range = YTL::GenAtoB<0, D - 1>::ints();
		auto vec = YTL::PM_Ints(range, [&](auto...S)
			{
				return std::tie(x[S.value]...);
			});
		to_raw_data<W>(p, vec);
	}
	template <bool R, bool W, class T, int D>
	void from_raw_data(hpos_safe& t, hpos_safe& p, glm::vec<D, T>* x)
	{
		using corresponding_tuple = YTL::TupleNTimes<T, D>;
		if constexpr (W)
		{
			constexpr static auto range = YTL::GenAtoB<0, D - 1>::ints();
			auto vec = YTL::PM_Ints(range, [&](auto...S)
				{
					return std::tie((*x)[S.value]...);
				});
			from_raw_data<R, true>(t, p, &vec);
		}
		else
			from_raw_data<R, false>(t, p, static_cast<corresponding_tuple*>(nullptr));
	}
#endif

	// Special
	template <bool W>
	void to_raw_data(bpos& p, const yobject& x)
	{
		to_raw_data<W>(p, x.x.size());
		if constexpr (W)
			memcpy(p, &x.x[0], x.x.size());
		p += x.x.size();
	}
	template <bool R, bool W>
	void from_raw_data(hpos_safe& t, hpos_safe& p, yobject* x)
	{
		type_test<R>(t, YSLSC_BYTES, TypeHolder< yobject>());
		if constexpr (W)
		{
			size_t n = p.read<size_t>();
			x->x.resize(n);
			if (n > 0) memcpy(&x->x.at(0), &p.buffer[p.index], n);
			p += n;
		}
	}
	template <bool W>
	void to_raw_data(bpos& p, const char* x)
	{
		size_t size = strlen(x);
		to_raw_data<W>(p, size);
		if constexpr (W)
			memcpy(p, &x[0], size);
		p += size;
	}
	template <bool W>
	void to_raw_data(bpos& p, const string& x)
	{
		to_raw_data<W>(p, x.size());
		if constexpr (W)
			memcpy(p, &x[0], x.size());
		p += x.size();
	}
	template <bool R, bool W>
	void from_raw_data(hpos_safe& t, hpos_safe& p, string* x)
	{
		type_test<R>(t, YSLSC_STRING, TypeHolder< yobject>());
		if constexpr (W)
		{
			size_t n = p.read<size_t>();
			x->resize(n);
			if (n > 0) memcpy(&x->at(0), &p.buffer[p.index], n);
			p += n;
		}
	}

	// Optional
	template <bool W, class T>
	void to_raw_data(bpos& p, const optional<T>& x)
	{
		if (x.has_value())
		{
			to_raw_data<W>(p, (uchar)true);
			to_raw_data<W>(p, x.value());
		}
		else
			to_raw_data<W>(p, (uchar)false);
	}
	template <bool R, bool W,  class T>
	void from_raw_data(hpos_safe& t, hpos_safe& p, optional<T>* x)
	{
		if constexpr (R)
		{
			if (t.read<FirstType>() != YSLSC_OPT) throw ESerialization<decltype(x)>();
		}
		else
			t.read<FirstType>();

		if constexpr (W)
		{
			uchar has;
			has = p.read<uchar>();
			if (has)
			{
				*x = T(); // allocate
				from_raw_data<R, true>(t, p, &x->value());
				return;
			}
		}
		from_raw_data<R, false>(t, p, static_cast<T*>(nullptr));
	}

	// Struct
	template <bool W, class S, enable_if_t<std::is_base_of_v<ysl_struct_base, S> && S::ysl_struct_trival, int> = 0>
	void to_raw_data(bpos & p, const S & x)
	{
		to_raw_data<W>(p, ysl_struct_tuple<S>(&x));
	}
	template <bool R, bool W, class S, enable_if_t<std::is_base_of_v<ysl_struct_base, S> && S::ysl_struct_trival, int> = 0>
	void from_raw_data(hpos_safe & t, hpos_safe & p, S* x)
	{
		if constexpr (W)
		{
			auto tp = ysl_struct_tuple<S>(x);
			from_raw_data<R, W>(t, p, &tp);
			x->ysl_loads_notify();
		}
		else
			from_raw_data<R, false>(t, p, static_cast<
				std::remove_reference_t<decltype(ysl_struct_tuple<S>(x))>*>(nullptr));
	}
	template <bool W, class S, enable_if_t<std::is_base_of_v<ysl_struct_base, S> && !S::ysl_struct_trival, int> = 0>
	void to_raw_data(bpos & p, const S & x)
	{
		to_raw_data<W>(p, x.ysl_nontrivial_pack());
	}
	template <bool R, bool W, class S, enable_if_t<std::is_base_of_v<ysl_struct_base, S> && !S::ysl_struct_trival, int> = 0>
	void from_raw_data(hpos_safe & t, hpos_safe & p, S* x)
	{
		if constexpr (W)
		{
			from_raw_data<R, W>(t, p, &x->ysl_nontrivial_pack());
			x->ysl_loads_notify();
		}
		else 
			from_raw_data<R, false>(t, p, static_cast<decltype(&x->ysl_nontrivial_pack())>(nullptr));
	}

	template <bool W, class T>
	void to_raw_data(bpos& p, const std::reference_wrapper<T>& x)
	{
		to_raw_data<W>(p, static_cast<T&>(x));
	}
	template <bool R, bool W, class T>
	void from_raw_data(hpos_safe& t, hpos_safe& p, std::reference_wrapper<T>* x)
	{
		if constexpr(W)
			from_raw_data<R,W>(t, p, &static_cast<T&>(*x));
		else
			from_raw_data<R, W>(t, p, static_cast<T*>(nullptr));
	}
}