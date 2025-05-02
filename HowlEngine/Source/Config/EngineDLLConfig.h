#pragma once

#ifdef HOWLENGINE_EXPORTS
#define HE_API __declspec(dllexport)

#else
#define HE_API __declspec(dllimport)

#endif
