#include "misc/memory.h"
#include <unistd.h>


namespace memory {
    
int free_memory() {
char stack_bottom;
  const char* heap_top = reinterpret_cast<char*>(sbrk(0));
  return &stack_bottom - heap_top;
}


}  // namespace memory