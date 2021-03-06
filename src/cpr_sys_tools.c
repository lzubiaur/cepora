/*
 * cpr_sys_tools.c
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#include "cpr_config.h"
#include "cpr_sys_tools.h"

#include <stdio.h>
#include <string.h>     /* strdup, _strdup */
#include <stdlib.h>     /* malloc, free, realpath, _splitpath_s */

#if defined(__APPLE__)
#include <mach-o/dyld.h> /* For _NSGetExecutablePath */
#include <sys/stat.h>
#endif

#if defined(__linux__)
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h> /* realpath */
#endif

/* GetModuleFileName will link the executable against KERNEL32.DLL */
#if defined(_WIN32)
#include <windows.h>
#include <Shlwapi.h> /* PathIsRelative, PathFileExists */
#else
#include <libgen.h> /* dirname */
#endif

#if defined(_WIN32)
#define chdir(p) (_chdir(p))
#endif

CPR_API_EXTERN int cpr_file_is_absolute(const char *path) {
#if defined(__linux__) || defined(__APPLE__)
  return (path[0] == '/');
#else
  return PathIsRelative(path) == 0;
#endif
}

CPR_API_EXTERN int cpr_file_exists(const char *path) {
#if defined(__linux__) || defined(__APPLE__)
  struct stat st;
  return stat(path, &st) == 0 ? 1 : 0;
#elif defined(_WIN32)
  return PathFileExists(path);
#endif
}

CPR_API_EXTERN char *cpr_get_exec_dir() {
    char *path = NULL, *dir = NULL;
#if defined(_WIN32)
    char drive_buf[_MAX_DRIVE], dir_buf[_MAX_DIR];
    if ((path = cpr_get_exec_path()) == NULL) goto end;
    if(_splitpath_s(path, drive_buf, _MAX_DRIVE, dir_buf, _MAX_DIR, NULL, 0, NULL, 0) != 0) {
        perror("get_app_dir");
        goto end;
    }
    if ((dir = _strdup(dir_buf)) == NULL) {
        perror("get_app_dir");
        goto end;
    }
#else
    char *buf = NULL;
    if ((path = cpr_get_exec_path()) == NULL) goto end;
    /* dirname returns a pointer to internal storage space allocated on the first call */
    if ((buf = dirname(path)) == NULL) {
        perror("get_app_dir");
        goto end;
    }
    if ((dir = strdup(buf)) == NULL) {
        perror("get_app_dir");
        goto end;
    }
#endif

end:
    free(path);
    return dir;
}

/*
 * http://stackoverflow.com/questions/1023306/finding-current-executables-path-without-proc-self-exe
 */
CPR_API_EXTERN char *cpr_get_exec_path() {
    char *full_path = NULL, *buf = NULL;
#if defined(__APPLE__)
    uint32_t size = 0;
    /* First call to get the buffer required size */
    _NSGetExecutablePath(NULL, &size);
    if ((buf = (char *)malloc(size)) == NULL) {
        fprintf(stderr, "insufficient memory\n");
        goto end;
    }
    /* Second call to actually get the exec path */
    if (_NSGetExecutablePath(buf, &size) != 0) {
        fprintf(stderr, "Can't get executable path\n");
        goto end;
    }
    /* Get the real absolute path. */
    if ((full_path = realpath(buf, NULL)) == NULL) {
        perror("get_exec_path");
        goto end;
    }
#elif defined(__linux__)
    size_t r;
    if ((buf = (char *)malloc(PATH_MAX)) == NULL) {
        fprintf(stderr, "insufficient memory\n");
        goto end;
    }
    if ((r = readlink("/proc/self/exe",buf,PATH_MAX)) < 0 ) {
        perror("get_exec_path");
        goto end;
    }
    buf[r] = '\0';
    /* Get the real absolute path. */
    if ((full_path = realpath(buf, NULL)) == NULL) {
        perror("get_exec_path");
        goto end;
    }
#elif defined(_WIN32)
    int len = 0;
    if ((buf = (char *)malloc(MAX_PATH + 1)) == NULL) {
        fprintf(stderr, "insufficient memory\n");
        goto end;
    }
    /* Call GetModuleFileName with hModule (first parameter) = NULL to get the path of
    the executable file of the current process */
    if ((len = GetModuleFileName(NULL, buf, MAX_PATH)) == 0) {
        fprintf(stderr, "Can't get executable file path [%d]",GetLastError());
        goto end;
    }
    /*  Windows XP: If path is too long the string is truncated to MAX_PATH characters
        and is not null-terminated so we alloc MAX_PATH + 1 and set `len +1` to null */
    buf[len + 1] = '\0';
    /* Call GetFullPathName once to get the required buffer length (including the null character) */
    if((len = GetFullPathName(buf, 0, NULL, NULL)) == 0) {
        fprintf(stderr, "GetFullPathName failed (%d)\n", GetLastError());
        goto end;
    }
    if ((full_path = (char *)malloc(len)) == NULL) {
        fprintf(stderr, "insufficient memory\n");
        goto end;
    }
    if(GetFullPathName(buf, len, full_path, NULL) == 0) {
        fprintf(stderr, "GetFullPathName failed (%d)\n", GetLastError());
        goto end;
    }
#endif

end:
    free(buf);
    return full_path;
}
