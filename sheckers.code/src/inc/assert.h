// Copyright: Steven Toscano

#ifdef DEBUG
	#include <iostream>
#endif

template <typename TResult>
bool Failed(TResult tr)
{
	if (1 == !tr)
	{
		return true;
	}
	else
	{
		return false;
	}
}

#ifdef ASSERT
	#undef ASSERT
#endif

#define ASSERT(x) ASSERTMSG(x, "")

#ifdef DEBUG
	#define ASSERTMSG(x, msg) \
	do { \
		if (!(x)) \
		{ \
			std::cout << msg << endl; \
			std::cout << "Failed (" << #x; \
			std::cout << ") at line " << __LINE__; \
			std::cout << " in file " << __FILE__ << endl; \
			__debugbreak(); \
		} \
	} while (false)
#else
	#define ASSERTMSG(x, msg) do {} while(false)
#endif

#define FAILED_RETURN(return_expression, expression) \
\
do \
{ \
	if (Failed(expression)) \
	{ \
		return return_expression; \
	} \
} while (false)

#define FAILED_ASSERT_RETURN(return_expression, expression) \
\
do \
{ \
	if (Failed(expression)) \
	{ \
		ASSERTMSG(0, #expression); \
		return return_expression; \
	} \
} while (false)
