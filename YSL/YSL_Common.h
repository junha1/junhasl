#pragma once

// can be commonly used with both C++ / Python

#include "YSL_SerializationConfiguration.h"
#include "junhatl/YTL.h"

#if (YSLSC_GLM_USE)
#include "Headers/Header_GLM.h" // modify this if you want
#endif



namespace ysl
{
	class yobject
	{
	public:
		bytes x;
	};

	using namespace YTL;

	template <class T> // instantiate with relevant type
	class ESerialization {};

	using bpos = uchar *; //buffer position
	using cbpos = const uchar*;
	using FirstType = uchar;

	struct hpos_safe
	{
		struct ill_formed {}; // header is ill-formed
		const bytes& buffer;
		size_t index; size_t index_max;
		hpos_safe(const bytes& b, size_t i = 0, size_t im = 0) : buffer(b)
		{
			index = i;
			if (im == 0) index_max = b.size();
			else index_max = im;
		}

		template <class T>
		T read()
		{
			if (index + sizeof(T) > index_max) throw ill_formed();
			auto res = *(T*)(&buffer[index]);
			index += sizeof(T);
			return res;
		}
		template <class T>
		void read_pass()
		{
			index += sizeof(T);
		}
		hpos_safe operator+(bint n)
		{
			return hpos_safe(buffer, index + n, index_max);
		}
		void operator+=(bint n)
		{
			index += n;
		}

		//for tuple size (which is encoded as string)
		pair<size_t, uchar> read_size()
		{
			char buf[17];
			int count = 0;

			if (buffer[index] <'0' || buffer[index] >'9' || index >= index_max) throw ill_formed();
			if (buffer[index] == '0')
			{
				index++; return make_pair(0, 1); // special case;
			}
			while (true)
			{
				if (index >= buffer.size() || count >= 16) throw ill_formed();
				if (buffer[index] <'0' || buffer[index] >'9') break;
				buf[count] = buffer[index];
				index++;
				count++;
			}
			buf[count] = 0;
			size_t result = 0;
			sscanf_s(buf, "%zu", &result);
			return make_pair(result, count);
		}
		// estimate size only (for non-typechecking, ok to be dangerous)
		void read_size_pass()
		{
			uchar count = 0;
			while (true)
			{
				if (buffer[index + count] <'0' || buffer[index + count] >'9') break;
				count++;
			}
			index += count;
		}

	};

	static bool is_primitive(FirstType t)
	{
		static std::unordered_set<uchar> prims = []()
		{
			std::unordered_set<uchar> prims;
			prims.insert(YSLSC_BOOL);
			prims.insert(YSLSC_CHAR);
			prims.insert(YSLSC_UCHAR);
			prims.insert(YSLSC_SHORT);
			prims.insert(YSLSC_USHORT);
			prims.insert(YSLSC_INT);
			prims.insert(YSLSC_UINT);
			prims.insert(YSLSC_LONG);
			prims.insert(YSLSC_ULONG);
			prims.insert(YSLSC_FLOAT);
			prims.insert(YSLSC_DOUBLE);
			return prims;
		}();
		return prims.count(t);
	}
	static bool is_special(FirstType t)
	{
		if (t == YSLSC_BYTES) return true;
		if (t == YSLSC_STRING) return true;
		return false;
	}
	static bool is_homogeneous(FirstType t)
	{
		if (t == YSLSC_LIST) return true;
		if (t == YSLSC_DICT) return true;
		if (t == YSLSC_SET) return true;
		if (t == YSLSC_OPT) return true;
		return false;
	}

	uint header_size(hpos_safe p, uint step = 0);
	static constexpr uchar size_encoding_size(const size_t s)
	{
		if (s < 10) return 1;
		else if (s < 100) return 2;
		else if (s < 1000) return 3;
		else if (s < 10000) return 4;
		else throw ESerialization<void>();
	}

	/* --- Mark for first types ---- */
	template <class T>
	struct mark;
	template<> struct mark<bool> { static const uchar value = YSLSC_BOOL; };
	template<> struct mark<char> { static const uchar value = YSLSC_CHAR; };
	template<> struct mark<uchar> { static const uchar value = YSLSC_UCHAR; };
	template<> struct mark<short> { static const uchar value = YSLSC_SHORT; };
	template<> struct mark<ushort> { static const uchar value = YSLSC_USHORT; };
	template<> struct mark<int> { static const uchar value = YSLSC_INT; };
	template<> struct mark<uint> { static const uchar value = YSLSC_UINT; };
	template<> struct mark<bint> { static const uchar value = YSLSC_LONG; };
	template<> struct mark<ubint> { static const uchar value = YSLSC_ULONG; };
	template<> struct mark<float> { static const uchar value = YSLSC_FLOAT; };
	template<> struct mark<double> { static const uchar value = YSLSC_DOUBLE; };

	template<class T> struct mark<vector<T>> { static const uchar value = YSLSC_LIST; };
	template<class T> struct mark<uset<T>> { static const uchar value = YSLSC_SET; };
	template<class K, class V> struct mark<umap<K, V>> { static const uchar value = YSLSC_DICT; };
	template<class T> struct mark<optional<T>> { static const uchar value = YSLSC_OPT; };

	template<typename... Args> struct mark<tuple<Args...>> { static const uchar value = YSLSC_TUPLE; };
	template<typename... Args> struct mark<variant<Args...>> { static const uchar value = YSLSC_VARIANT; };
}

