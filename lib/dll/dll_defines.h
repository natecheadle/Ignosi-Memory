#pragma once

#ifndef DLL_FUNC
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

#ifndef CALL

#ifdef _WIN32
#define CALL __stdcall
#else
#define CALL
#endif  // _WIN32
#endif  // !CALL

#define DLL_FUNC(RETURN_T) extern "C" EXPORT_SPEC RETURN_T CALL
#endif  // !DLL_FUNC
