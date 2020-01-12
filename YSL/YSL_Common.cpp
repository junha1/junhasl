#include "YSL_Common.h"

uint ysl::header_size(hpos_safe p, uint step)
{
	if(step > 10000) hpos_safe::ill_formed();
	FirstType type = p.read<FirstType>();
	if (is_primitive(type)) return 1;
	if (type == YSLSC_STRING || type == YSLSC_BYTES) return 1;
	if (type == YSLSC_LIST) return 1 + header_size(p);
	if (type == YSLSC_SET) return 1 + header_size(p);
	if (type == YSLSC_DICT)
	{
		auto keysize = header_size(p);
		p += keysize;
		return 1 + keysize + header_size(p);
	}
	if (type == YSLSC_OPT) return 1 + header_size(p);
	if (type == YSLSC_TUPLE)
	{
		auto n = p.read_size();
		size_t size = 0;
		for (size_t i = 0; i < n.first; i++)
		{
			size += header_size(p + size);
		}
		return 1 + n.second + size;
	}
	if (type == YSLSC_VARIANT)
	{
		auto n = p.read_size();
		size_t size = 0;
		for (size_t i = 0; i < n.first; i++)
		{
			size += header_size(p + size);
		}
		return 1 + n.second + size;
	}
	else throw hpos_safe::ill_formed();
}

