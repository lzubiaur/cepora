#ifndef CONFIG_H
#define CONFIG_H

#define VC_EXTRALEAN 1
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

typedef void (* open_mod_fun_t)(void);

#define FOO_DLL_NAME "foo.dll"
#define BAR_DLL_NAME "bar.dll"
#define OPEN_MOD_SYM "open_foo"

#if defined(BUILD_DLL)
#define API_DLL __declspec(dllexport)
#else
#define API_DLL __declspec(dllimport)
#endif

#endif
