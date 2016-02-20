#include "config.h"

int main(int argc, char **argv) { 
  HMODULE handle = NULL;
  open_mod_fun_t open_dll = NULL;
  
  if ((handle = LoadLibraryExA(FOO_DLL_NAME, NULL, 0)) == NULL) {
    printf("Cannot open shared library '%s'\n", FOO_DLL_NAME);
    exit(EXIT_FAILURE);
  }

  if ((open_dll = (open_mod_fun_t)GetProcAddress((HMODULE)handle, OPEN_MOD_SYM)) == NULL)  {
    printf("Cannot find symbol '%s'", OPEN_MOD_SYM);
    exit(EXIT_FAILURE);
  }

  open_dll();

  FreeLibrary((HMODULE)handle);

  bar("called from exe");

  return EXIT_SUCCESS;
}
