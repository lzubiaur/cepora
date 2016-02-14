/*
 * cpr_sys_tools.h
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#ifndef CPR_SYS_TOOLS_H
#define CPR_SYS_TOOLS_H

#include "cpr_config.h"

#ifdef __cplusplus
extern "C" {
#endif

CPR_API_EXTERN int cpr_file_is_absolute(const char *path);

/* TODO check GLFW cmake options (GLFW_USE_CHDIR) or directive (_GLFW_USE_CHDIR)
 * when building OSX release because glfwInit will change current directory
 * to the bundle `resource` folder automaticaly.
 */

CPR_API_EXTERN int cpr_file_exists(const char *path);

/* Get the directory absolute path of the executable.
 * The path string must be freed by the caller.
 */
CPR_API_EXTERN char *cpr_get_exec_dir();
/* Get the executable absolute path (no symbolic link, /./ or /../ components are resolved).
 * The string is dynamically allocated and must be freed by the caller.
 */
CPR_API_EXTERN char *cpr_get_exec_path();

#ifdef __cplusplus
}
#endif

#endif /* CPR_SYS_TOOLS_H */
