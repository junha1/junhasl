#pragma once

#include "YSL_SerializationConfiguration.h"
#include "../YTL.h"
#include "YSL_Common.h"
#include "YSL_Types.h"

namespace ysl
{
	using std::enable_if;
	using YTL::TypeHolder;
	using std::size_t;

	using namespace YTL;

	template <class T>
	uint write_p(bpos& p, T t)
	{
		*(T*) p = t;
		p += sizeof(T);
		return sizeof(T);
	}
}