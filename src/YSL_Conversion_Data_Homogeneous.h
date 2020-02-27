#pragma once

#pragma warning( push )
#pragma warning( disable: 4348 )

#include "YSL_Conversion_Data.h"
namespace ysl
{
	// PAIR (ONLY FOR DICT : never use std::pair to serialize)
	template <bool W, class K, class V>
	void to_raw_data(bpos& p, const pair<K, V>& x)
	{
		to_raw_data<W>(p, x.first);
		to_raw_data<W>(p, x.second);
	}
	template <bool R, bool W, class K, class V>
	void from_raw_data(hpos_safe& t, hpos_safe& p, pair<K, V>* x)
	{
		if constexpr (W)
		{
			from_raw_data<R, true>(t, p, &x->first);
			from_raw_data<R, true>(t, p, &x->second);
		}
		else
		{
			from_raw_data<true, false>(t, p, static_cast<K*>(nullptr));
			from_raw_data<true, false>(t, p, static_cast<V*>(nullptr));
		}
	}

	// List
	template <bool W, class V, enable_if_t<is_homogeneous_composite<V>, int> = 0>
	void to_raw_data(bpos & p, const V & x)
	{
		if constexpr (W || !size_fixed_f(TypeHolder<V::value_type>()))
		{
			auto old = p;
			to_raw_data<W>(p, (size_t)x.size());
			for (const auto& y : x)
				to_raw_data<W>(p, y);
		}
		else // optimization : doesn't iterate for fixed-size type.
		{
			auto size = sizeof(size_t);
			size += estimate_data_size_f(TypeHolder<V::value_type>()) * x.size();
			p += size;
		}
	}

	template <class V, typename sfinae = void>
	struct ValueType
	{
		using type = typename V::value_type;
	};

	template <class V>
	struct ValueType<V, typename enable_if_t<ysl_is_umap<V>::value>>
	{
		//to remove const of key_type
		using type = typename pair<typename V::key_type, typename V::mapped_type>;
	};

	template <bool R, bool W, class V, enable_if_t<is_homogeneous_composite<V>, int> = 0>
	void from_raw_data(hpos_safe & t, hpos_safe & p, V* x)
	{
		using T = typename ValueType<V>::type;

		if constexpr (R)
		{
			if constexpr (is_list_convertible<V>)
			{
				if (t.read<FirstType>() != YSLSC_LIST) throw ESerialization<V>();
			}
			else if constexpr (ysl_is_umap<V>::value)
			{
				if (t.read<FirstType>() != YSLSC_DICT) throw ESerialization<V>();
			}
			else if constexpr (ysl_is_uset<V>::value)
			{
				if (t.read<FirstType>() != YSLSC_SET) throw ESerialization<V>();
			}
			else static_assert(false);
		}
		else t.read_pass<FirstType>();
		// t will be pointing just after '(' or '{' or '<'
		auto oldt = t;
		from_raw_data<R, false>(t, p, static_cast<T*>(nullptr));

		if constexpr (W)
		{
			size_t n = p.read<size_t>();
			if constexpr (ysl_is_array<V>::value) //allocated case
			{
				if (n != x->size()) throw ESerialization<V>();
			}
			else x->clear();
			if constexpr (is_list_convertible<V>)
			{
				if constexpr (ysl_is_array<V>::value)
				{
					for (int i = 0; i < n; i++)
					{
						hpos_safe temp = oldt; // reset
						from_raw_data<false, true>(temp, p, &x->at(i));
					}
				}
				else
				{
					for (int i = 0; i < n; i++)
					{
						hpos_safe temp = oldt; // reset
						from_raw_data<false, true>(temp, p, &x->emplace_back());
					}
				}
			}
			else
			{
				for (int i = 0; i < n; i++)
				{
					hpos_safe temp = oldt; // reset
					T elem; from_raw_data<false, true>(temp, p, &elem);
					x->emplace(elem);
				}
			}
		}
	}
}

#pragma warning( pop )