#pragma once

#include "YSL_Conversion_Data_Homogeneous.h"
#include "YSL_Conversion_Data_Heterogeneous.h"

namespace ysl
{
	/* --- Final Interfaces --- */
	template<class T>
	yobject to(const T& t)
	{
		static constexpr size_t header_size = mark_size_f(TypeHolder<T>());
		size_t data_size;
		uchar* test = 0; to_raw_data<false>(test, t);
		data_size = (size_t)test;

		yobject result;
		result.x.resize(header_size + data_size, 0);
		bpos buf = &result.x[0];

		mark_write_f(buf, TypeHolder<T>());
		to_raw_data<true>(buf, t);

		if (buf - (&result.x[0] + header_size) != data_size) throw ESerialization<T>();
		return result;
	}
	template<class T>
	void from(const yobject& o, T& x)
	{
		hpos_safe all(o.x);
		try
		{
			auto hs = header_size(all);
			hpos_safe header(o.x, 0, hs);
			hpos_safe data(o.x, hs, o.x.size());

			from_raw_data<true, true>(header, data, &x);
		}
		catch (const hpos_safe::ill_formed& e) { e;  throw ESerialization<T>(); }
	}

	// for script programming
	template<class T>
	string header()
	{
		static constexpr size_t header_size = mark_size_f(TypeHolder<T>());
		bytes res(header_size);
		bpos buf = &res[0];
		mark_write_f(buf, TypeHolder<T>());

		string result;
		result.resize(header_size);
		for (int i = 0; i < header_size; i++)
			result[i] = res[i];
		return result;
	}
}