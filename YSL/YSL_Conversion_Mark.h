#pragma once
#include "YSL_Core.h"


namespace ysl
{
	/*
	=================================
	========= TYPE MARKING =========
	================================
	*/
	template <class T>
	constexpr size_t mark_size_f(const TypeHolder<T>);
	template <class T>
	void mark_write_f(bpos& p, const TypeHolder<T>);

	/* --- Forward declaration for composite types --- */
	template <class V, typename = enable_if_t<is_list_convertible<V>>>
	constexpr size_t mark_size(const TypeHolder<V>);
	template <class V, typename = enable_if_t<is_list_convertible<V>>>
	void mark_write(bpos& p, const TypeHolder<V>);

	template <typename... Args>
	constexpr size_t mark_size(const TypeHolder<tuple<Args...>>);
	template <typename... Args>
	void mark_write(bpos& p, const TypeHolder<tuple<Args...>>);

	template <class T>
	constexpr size_t mark_size(const TypeHolder<optional<T>>);
	template <class T>
	void mark_write(bpos& p, const TypeHolder<optional<T>>);


	// Primitive
	template <class T, typename enable_if<IsPrimitive<T>>::type* = nullptr>
	constexpr size_t mark_size(const TypeHolder<T>) { return 1; }
	template <class T, typename enable_if<IsPrimitive<T>>::type* = nullptr>
	void mark_write(bpos& p, const TypeHolder<T>)
	{
		write_p<FirstType>(p, mark<T>::value);
	}

	// Special
	static constexpr size_t mark_size(const TypeHolder<yobject>) { return 1; }
	static constexpr size_t mark_size(const TypeHolder<string>) { return 1; }
	static void mark_write(bpos& p, const TypeHolder<yobject>) { write_p<FirstType>(p, YSLSC_BYTES); }
	static void mark_write(bpos& p, const TypeHolder<string>) { write_p<FirstType>(p, YSLSC_STRING); }

	// List
	template <class V, typename>
	constexpr size_t mark_size(const TypeHolder<V>)
	{
		return 1 + mark_size_f(TypeHolder<typename V::value_type>());
	}
	template <class V, typename>
	void mark_write(bpos& p, const TypeHolder<V>)
	{
		write_p<FirstType>(p, YSLSC_LIST);
		mark_write_f(p, TypeHolder<typename V::value_type>());
	}

	// Set
	template <class T>
	constexpr size_t mark_size(const TypeHolder<uset<T>>)
	{
		return 1 + mark_size_f(TypeHolder<T>());
	}
	template <class T>
	void mark_write(bpos& p, const TypeHolder<uset<T>>)
	{
		write_p<FirstType>(p, YSLSC_SET);
		mark_write_f(p, TypeHolder<T>());
	}

	// Dict
	template <class K, class V>
	constexpr size_t mark_size(const TypeHolder<umap<K, V>>)
	{
		return 1 + mark_size_f(TypeHolder<K>()) + mark_size_f(TypeHolder<V>());
	}
	template <class K, class V>
	void mark_write(bpos& p, const TypeHolder<umap<K, V>>)
	{
		write_p<FirstType>(p, YSLSC_DICT);
		mark_write_f(p, TypeHolder<K>());
		mark_write_f(p, TypeHolder<V>());
	}

	// Optional
	template <class T>
	constexpr size_t mark_size(const TypeHolder<optional<T>>)
	{
		return 1 + mark_size_f(TypeHolder<T>());
	}
	template <class T>
	void mark_write(bpos& p, const TypeHolder<optional<T>>)
	{
		write_p<FirstType>(p, YSLSC_OPT);
		mark_write_f(p, TypeHolder<T>());
	}


	/* Heterogenous*/
	template <typename T>
	int mark_write_(bpos& p, const TypeHolder<T>)
	{
		mark_write_f(p, TypeHolder<T>());
		return 0;
	}
	// Tuple
	template <typename... Args>
	constexpr size_t mark_size(const TypeHolder<tuple<Args...>>)
	{
		return 1 + size_encoding_size(sizeof...(Args)) + YTL::Adder_Int<mark_size_f(TypeHolder<Args>())...>;
	}
	template <typename... Args>
	void mark_write_helper(bpos& p, const TypeHolder<tuple<Args...>>)
	{
		constexpr static int n = sizeof...(Args);
		constexpr static auto range = YTL::GenAtoB<0, n - 1>::ints();
		using t = tuple<Args...>;
		YTL::PM_Ints(range, [&](auto...S)
			{
				return vector<int>{mark_write_(p,
					TypeHolder<std::tuple_element_t<S.value, t>>())...}; // {} guarantees sequential order
			});
	}
	template <typename... Args>
	void mark_write(bpos& p, const TypeHolder<tuple<Args...>>)
	{
		write_p<FirstType>(p, YSLSC_TUPLE);

		char buf[17];
		int n = snprintf(buf, 16, "%d", (int)sizeof...(Args));
		memcpy(p, buf, n);
		p += n;
		mark_write_helper(p, TypeHolder<tuple<Args...>>());
	}

	// Variant
	template <typename... Args>
	constexpr size_t mark_size(const TypeHolder<variant<Args...>>)
	{
		return 1 + size_encoding_size(sizeof...(Args)) + YTL::Adder_Int<mark_size_f(TypeHolder<Args>())...>;
	}
	template <typename... Args>
	void mark_write_helper(bpos& p, const TypeHolder<variant<Args...>>)
	{
		constexpr static int n = sizeof...(Args);
		constexpr static auto range = YTL::GenAtoB<0, n - 1>::ints();
		using t = variant<Args...>;
		YTL::PM_Ints(range, [&](auto...S)
			{
				return vector<int>{mark_write_(p,
					TypeHolder<std::variant_alternative_t<S.value, t>>())...}; // {} guarantees sequential order
			});
	}
	template <typename... Args>
	void mark_write(bpos& p, const TypeHolder<variant<Args...>>)
	{
		write_p<FirstType>(p, YSLSC_VARIANT);

		char buf[17];
		int n = snprintf(buf, 16, "%d", (int)sizeof...(Args));
		memcpy(p, buf, n);
		p += n;
		mark_write_helper(p, TypeHolder<variant<Args...>>());
	}

	// Final interface
	template <class T>
	constexpr size_t mark_size_f(const TypeHolder<T>)
	{
		return mark_size(TypeHolder<TypeConfirm_v<T>>());
	}
	template <class T>
	void mark_write_f(bpos& p, const TypeHolder<T>)
	{
		mark_write(p, TypeHolder<TypeConfirm_v<T>>());
	}
}