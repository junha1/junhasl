#pragma once
#include "YSL_Conversion.h"
#include "YSL_PP.h"

using ysl::ysl_struct_base;
using ysl::ysl_struct;
using ysl::ysl_struct_final;

namespace ysl
{
	using none = optional<uchar>;
	using ybool = uchar;


	/*
	ysl identifer provides...
	1. hash
	2. comparator
	3. re-load lookup
	*/
	template <class T, int DefaultValue = 0, class Reloader = void>
	class identifer : public ysl_struct_base
	{
	public:
		int id;

		using tuple_unpack_for_single_member = std::bool_constant<true>;
		using ysl_parent = void;
		YSL_PACK(&identifer::id);

		identifer() { id = DefaultValue; }
		explicit identifer(int i) { id = i; }
		bool operator<(const identifer& x) const { return id < x.id; }
		bool operator==(const identifer& x) const { return id == x.id; }
		bool operator!=(const identifer& x) const { return id != x.id; }

		explicit operator bool() const { return id != DefaultValue; }

		void ysl_loads_notify()
		{
			if constexpr (!std::is_same_v<Reloader, void>) id = Reloader{}(id);
		}
	};
}

namespace std
{
	template<class T, int D, class R> struct hash<ysl::identifer<T, D, R>>
	{
		typedef ysl::identifer<T, D, R> argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const noexcept
		{
			return std::hash<int>()(s.id);
		}
	};
}

using ynone = ysl::none;
using ysl::ybool;