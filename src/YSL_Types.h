#pragma once

#include "YSL_Common.h"
#include "YSL_Struct.h"
namespace ysl
{
	// remember : no bool
	template <class T>
	constexpr static auto IsPrimitive = YTL::is_any<T, bool, char, uchar, short, ushort, int, uint, bint, ubint, float, double>::value;

	// for those complete aliases
	template <class T, class = std::void_t<>>
	struct TypeConfirm;
	template<class T>
	using TypeConfirm_v = typename TypeConfirm<T>::type;

	// itself
	template <class T, class = std::void_t<>>
	struct TypeConfirm
	{
		using type = T;
	};
#if (YSLSC_GLM_USE)
	// glm::vec
	template <class T, int D>
	struct TypeConfirm<glm::vec<D, T>>
	{
		using type = typename YTL::TupleNTimes<T, D>;
	};
#endif
	// ysl_struct
	template <class S>
	struct TypeConfirm<S, std::void_t<typename enable_if_t<
		std::is_base_of_v<ysl_struct_base, S> && S::ysl_struct_trival>>>
	{
		using type = TypeConfirm_v<typename decltype(ysl_struct_tuple<S>(nullptr))>;
	};
	template <class S>
	struct TypeConfirm<S, std::void_t<typename enable_if_t<
		std::is_base_of_v<ysl_struct_base, S> && !S::ysl_struct_trival>>>
	{
		// Note : only non-trivial can be derived from another non-trivial.
		using type = TypeConfirm_v<typename S::ysl_nontrivial_type>;
	};
	
	// enum
	template <class U>
	struct TypeConfirm<U, std::void_t<typename enable_if_t<std::is_enum_v<U>>>>
	{
		using type = int;
	};
	// reference
	template <class T>
	struct TypeConfirm<T&>
	{
		using type = typename TypeConfirm<T>::type;
	};
	template <class T>
	struct TypeConfirm<std::reference_wrapper<T>>
	{
		using type = typename TypeConfirm<T>::type;
	};
	// const
	template <class T>
	struct TypeConfirm<const T>
	{
		using type = typename TypeConfirm<T>::type;
	};

	template <size_t N>
	struct TypeConfirm<char[N]>
	{
		using type = string;
	};

	template <size_t N>
	struct TypeConfirm<const char[N]>
	{
		using type = string;
	};

	template <>
	struct TypeConfirm<const char*>
	{
		using type = string;
	};
	
	/*
	======================================
	========= Helper Templates ===========
	======================================
	*/
	// Array Size
	template<class T, size_t N>
	constexpr size_t Array_Size(TypeHolder<array<T, N>>)
	{
		return N;
	};

	// type 
	// very intersting SFINAE used here
	// void_t can detect ill-formed template parameter. (we can't directly use YTL::is_any here)
	template< class, class = std::void_t<> >
	struct ysl_is_vector : std::false_type {};
	template< class V >
	struct ysl_is_vector<V, std::void_t<typename V::value_type>>
	{
		static constexpr bool value = is_same_v<V, vector<typename V::value_type>>;
	};

	template< class, class = std::void_t<> >
	struct ysl_is_list : std::false_type {};
	template< class V >
	struct ysl_is_list<V, std::void_t<typename V::value_type>>
	{
		static constexpr bool value = is_same_v<V, list<typename V::value_type>>;
	};

	template< class, class = std::void_t<> >
	struct ysl_is_array : std::false_type {};
	template< class V >
	struct ysl_is_array<V, std::void_t<typename V::value_type, typename decltype(Array_Size(TypeHolder<V>()))>>
	{
		static constexpr bool value = is_same_v<V, array<typename V::value_type, Array_Size(TypeHolder<V>())>>;
	};

	template< class, class = std::void_t<> >
	struct ysl_is_uset : std::false_type {};
	template< class V >
	struct ysl_is_uset<V, std::void_t<typename V::value_type>>
	{
		static constexpr bool value = is_same_v<V, uset<typename V::value_type>>;
	};

	template< class, class = std::void_t<> >
	struct ysl_is_umap : std::false_type {};
	template< class V >
	struct ysl_is_umap<V, std::void_t<typename V::key_type, typename V::mapped_type>>
	{
		static constexpr bool value = is_same_v<V, umap<typename V::key_type, typename V::mapped_type>>;
	};


	//final interface
	template <class V>
	constexpr bool is_homogeneous_composite = ysl_is_vector<V>::value || ysl_is_list<V>::value || ysl_is_array<V>::value ||
		ysl_is_umap<V>::value || ysl_is_uset<V>::value;
	template <class V>
	constexpr bool is_list_convertible = ysl_is_vector<V>::value || ysl_is_list<V>::value || ysl_is_array<V>::value;
	template <class V>
	constexpr bool is_variable_sized_type = ysl_is_vector<V>::value || ysl_is_list<V>::value 
		|| ysl_is_umap<V>::value || ysl_is_uset<V>::value;


	/*
	======================================
	========= Size Variability ===========
	======================================
	*/
	template <class T>
	constexpr bool size_fixed_f(const TypeHolder<T>);

	// forward declaration
	template <class K, class V>
	constexpr bool size_fixed(const TypeHolder<pair<K, V>>);
	template <class T, int N>
	constexpr bool size_fixed(const TypeHolder<array<T, N>>);
	template <typename... Args>
	constexpr bool size_fixed(const TypeHolder<tuple<Args...>>);

	// actual definition
	template <class T, enable_if_t<IsPrimitive<T>>* = nullptr>
	constexpr bool size_fixed(const TypeHolder<T>) { return true; }
	static constexpr bool size_fixed(const TypeHolder<string>) { return false; }
	static constexpr bool size_fixed(const TypeHolder<yobject>) { return false; }

	template <class T>
	constexpr bool size_fixed(const TypeHolder<optional<T>>) { return false; }
	template <typename... Args>
	constexpr bool size_fixed(const TypeHolder<variant<Args...>>) { return false; }

	template <class V, enable_if_t<is_variable_sized_type<V>>* = nullptr>
	constexpr bool size_fixed(const TypeHolder<V>) { return false; }
	template <class K, class V>
	constexpr bool size_fixed(const TypeHolder<pair<K,V>>) 
 	{ return size_fixed_f(TypeHolder<std::remove_const_t<K>>()) && size_fixed_f(TypeHolder<V>()); }
	template <class T, int N>
	constexpr bool size_fixed(const TypeHolder<array<T, N>>)
	{
		return size_fixed_f(TypeHolder<T>());
	}
	template <typename... Args>
	constexpr bool size_fixed(const TypeHolder<tuple<Args...>>)
	{
		if constexpr (sizeof...(Args) == 0) return true;
		else return YTL::Adder<bool, std::logical_and<bool>, size_fixed_f(TypeHolder<Args>())...>::value;
	}

	// final interface
	template <class T>
	constexpr bool size_fixed_f(const TypeHolder<T>)
	{
		return size_fixed(TypeHolder<TypeConfirm_v<T>>());
	}

	/* for those size-variable objects, it should be discarded by 'if constexpr' and thus
	never should be instantiated. */
	template <class T>
	constexpr size_t estimate_data_size_f(const TypeHolder<T>);

	// forward-declaration
	template <class T, int N>
	constexpr size_t estimate_data_size(const TypeHolder<array<T, N>>);
	template <typename... Args>
	constexpr size_t estimate_data_size(const TypeHolder<tuple<Args...>>);
	template <class K, class V>
	constexpr size_t estimate_data_size(const TypeHolder<pair<K, V>>);

	// actual definition
	template <class T, typename = enable_if_t<IsPrimitive<T>>>
	constexpr size_t estimate_data_size(const TypeHolder<T>) { return sizeof(T); }
	template <class T, int N>
	constexpr size_t estimate_data_size(const TypeHolder<array<T, N>>)
	{
		return sizeof(size_t) + estimate_data_size_f(TypeHolder<T>()) * N;
	}
	template <typename... Args>
	constexpr size_t estimate_data_size(const TypeHolder<tuple<Args...>>)
	{
		return YTL::Adder_Int<estimate_data_size_f(TypeHolder<Args>())...>;
	}
	template <class K, class V>
	constexpr size_t estimate_data_size(const TypeHolder<pair<K, V>>)
	{
		return estimate_data_size_f(TypeHolder<std::remove_const_t<K>>()) + estimate_data_size_f(TypeHolder<V>());
	}

	// final interface
	template <class T>
	constexpr size_t estimate_data_size_f(const TypeHolder<T>)
	{
		return estimate_data_size(TypeHolder<TypeConfirm_v<T>>());
	}

	/*
	=====================================================
	========= Run Time Reading of Primitives  ===========
	=====================================================
	*/
	template <class T, class S>
	void read_primitive_(T& t, hpos_safe& buf)
	{
		/* check loss of information
		if (sizeof(T) < sizeof(char)) throw ESerialization<T>();
		if (!std::is_unsigned_v<T> && std::is_unsigned_v<char>) throw ESerialization<T>();
		*/
		t = (T) buf.read<S>();
	}

	template <bool R, bool W, class T>
	void read_primitive(T* t, FirstType type, hpos_safe& buf)
	{
		static array<std::function<void(T& t, hpos_safe& buf)>, 128> reader =
			[]() 
		{
			array<std::function<void(T& t, hpos_safe& buf)>, 128> reader;
			reader[YSLSC_BOOL] = &read_primitive_<T, bool>;
			reader[YSLSC_CHAR] = &read_primitive_<T, char>;
			reader[YSLSC_UCHAR] = &read_primitive_<T, uchar>;
			reader[YSLSC_SHORT] = &read_primitive_<T, short>;
			reader[YSLSC_USHORT] = &read_primitive_<T, ushort>;
			reader[YSLSC_INT] = &read_primitive_<T, int>;
			reader[YSLSC_UINT] = &read_primitive_<T, uint>;
			reader[YSLSC_LONG] = &read_primitive_<T, bint>;
			reader[YSLSC_ULONG] = &read_primitive_<T, ubint>;
			reader[YSLSC_FLOAT] = &read_primitive_<T, float>;
			reader[YSLSC_DOUBLE] = &read_primitive_<T, double>;
			return reader;
		}(); // this is ugly but thread-safe way of static variable initialization.

		if constexpr (R) if (!is_primitive(type)) throw ESerialization<T>();
		if constexpr (W) reader[type](*t, buf);
		else return;
	}
}