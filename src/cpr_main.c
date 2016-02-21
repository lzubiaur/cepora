/*
 * cpr_main.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include "cpr_config.h"
#include "cpr_cepora.h"

#if defined(CPR_BUILD_WINDOWS)
/* Faster builds with smaller header files */
#define VC_EXTRALEAN 1
#define WIN32_LEAN_AND_MEAN 1
/* TODO investigte the "NOapi" symbols */
/* (https://msdn.microsoft.com/en-us/library/windows/desktop/aa383745(v=vs.85).aspx) */
#include <windows.h> /* WindMain */
#endif

#if defined(CPR_BUILD_WINDOWS)
int CALLBACK WinMain(
  HINSTANCE hInstance,     /* handle to the current instance of the application. */
  HINSTANCE hPrevInstance, /* handle to the previous instance of the application */
  LPSTR     lpCmdLine,     /* command line for the application */
  int       nCmdShow) {    /* controls how the window is to be shown */
  /* Get command line count and string from global variable __argc and __argv */
  return cpr_start(__argc, __argv);
}
#else
/* Linux and OSX use standard entry point */
int main(int argc, char *argv[]) {
  return cpr_start(argc, argv);
}
#endif
