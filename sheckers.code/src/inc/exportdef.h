// Copyright: Steven Toscano

#pragma once

// This macro should be defined in this dll project so
//	the external api's can be accessible outside the dll
// Clients that include the public API headers should
//	not define these macros

#ifdef EXPORT_PUBLIC_API
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __declspec(dllimport)
#endif

#ifdef IMPORT_PUBLIC_API
#define IMPORT_API __declspec(dllimport)
#else
#define IMPORT_API __declspec(dllexport)
#endif
