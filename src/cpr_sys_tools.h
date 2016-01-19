/*
 * cpr_sys_tools.h
 * Copyright (c) 2015 Laurent Zubiaur
 * MIT License (http://opensource.org/licenses/MIT)
 */

#ifndef CPR_SYS_TOOLS_H
#define CPR_SYS_TOOLS_H

#ifdef __cplusplus
extern "C" {
#endif

/* TODO check GLFW cmake options (GLFW_USE_CHDIR) or directive (_GLFW_USE_CHDIR)
 * when building OSX release because glfwInit will change current directory
 * to the bundle `resource` folder automaticaly.
 */

int cpr_file_exists(const char *path);

/* Get the directory absolute path of the executable.
 * The path string must be freed by the caller.
 */
char *cpr_get_exec_dir();
/* Get the executable absolute path (no symbolic link, /./ or /../ components are resolved).
 * The string is dynamically allocated and must be freed by the caller.
 */
char *cpr_get_exec_path();

#ifdef __cplusplus
}
#endif

#endif /* CPR_SYS_TOOLS_H */
