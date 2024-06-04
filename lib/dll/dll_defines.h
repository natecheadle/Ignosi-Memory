#pragma once

#ifdef EXPORT
#ifdef _WIN32
#define EXPORT_SPEC __declspec(dllexport)
#else
#define EXPORT_SPEC
#endif  // _WIN32
#else
#ifdef _WIN32
#define EXPORT_SPEC __declspec(dllimport)
#else
#define EXPORT_SPEC
#endif  // _WIN32
#endif  // !EXPORT
