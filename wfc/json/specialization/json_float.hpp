#pragma once

#include <cstdio>
#include <cstdlib>

namespace wfc{ namespace json{

template<int N>
struct float_format;

template<>
struct float_format<10> {
  const char *operator()()
  {
    return "%.10f";
  }
};

template<int N>
class serializerFloat
{
public:
	// serialize
	template<typename P>
	P operator()(float v, P beg)
	{
		char buf[32] = { '\0' };
		const char* s = buf;
		snprintf(buf, sizeof(buf), float_format<N>()(), v);
		while(*s != '\0') {
			*(beg++) = *(s++);
		}
		return beg;
	}

	// unserialize
	template<typename P>
	P operator()(float& v, P beg, P end)
	{
		if( parser::is_null(beg, end) ) 
    {
			v = 0.0;
			return parser::parse_null(beg, end);
		}
		if(beg == end) {
			throw unexpected_end_fragment();
		}
		v = static_cast<float>( std::atof( &(*beg) ) );
		while(beg != end) {
			if((*beg < '0' || *beg > '9') && *beg != '.' && *beg != '-') {
				break;
			}
			++beg;
		}
		return beg;
	}
};

template<int N>
struct float_value
{
	typedef float target;
	typedef serializerFloat< N > serializer;
};

}}
