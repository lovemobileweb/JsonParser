
#ifndef PLATFORM_SELECTOR_H
#define PLATFORM_SELECTOR_H

#include <math.h>
#include <cmath>
#include <limits>
#include <float.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#if defined(_MSC_VER)

#include "Convert.h"
#include "JsonException.h"

//
typedef int64_t int_size;

//-------------------------------------------------------------------------
// compatibility
namespace std
{

	//-------------------------------------------------------------------------
	//
	inline int isnan(double x)
	{
		return _isnan(x);
	}
	inline int isinf(double x)
	{
		return _finite(x) == 0;
	}

// apologies for the macros once more ...?
#define _IS_NAN(arg) std::isnan(arg)
#define _IS_INF(arg) std::isinf(arg)
#define __PLATFORM_THROW(arg) throw (arg)
	
}

#elif defined(__MACH__)

#include <stdlib.h>

#include <mach/clock.h>
#include <mach/mach.h>
#include <mach/mach_time.h>  // for mach_absolute_time() and friends
#include "osx/convert.h"
#include "osx/json_ex.h"

//
typedef int64_t int_size;

// apologies for the macros once more ...?
#define _IS_NAN(arg) isnan(arg)
#define _IS_INF(arg) isinf(arg)
#define __PLATFORM_THROW(arg) throw (arg)

// g++
#elif defined(__GNUG__)

#include "osx/convert.h"
#include "osx/json_ex.h"

// apologies for the macros once more ...?
#define _IS_NAN(arg) std::isnan(arg)
#define _IS_INF(arg) std::isinf(arg)
#define __PLATFORM_THROW(arg) throw (arg)
//
typedef int64_t int_size;

#elif defined(__arm__)

#include "arm/json_ex.h"
#include "arm/convert.h"

// apologies for the macros once more ...?
#define _IS_NAN(arg) isnan(arg)
#define _IS_INF(arg) isinf(arg)
#define __PLATFORM_THROW(arg) throw (arg)
//
typedef int32_t int_size;

#else

#error No support for current platform

#endif

#if defined(__MACH__) || defined(__arm__)

#include <stdarg.h>

#endif


#endif	// platform_selector

