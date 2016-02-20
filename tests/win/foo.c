#include "foo.h"
#include "bar.h"

API_DLL void open_foo(void) {
  bar("called from foo");
}
