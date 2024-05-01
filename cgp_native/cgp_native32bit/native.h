#ifdef __unix__
#include <sys/mman.h>  /* mprotect */
#include <limits.h>    /* PAGESIZE */
#endif

#ifndef PAGESIZE
 #define PAGESIZE 4096
#endif 

//------------------------------------------------------------------
// MALLOC
//  aligned malloc with code execution flag
//------------------------------------------------------------------
unsigned char * malloc_aligned(size_t size, size_t alignment = PAGESIZE)
{

  unsigned char *p = (unsigned char *)malloc(size + alignment-1);
  if (!p) 
  {
    perror("malloc failed");
    exit(1);
  }
  //printf("malloc len %d\n", size + alignment-1);
  unsigned char *pp = (unsigned char *)(((__PTRDIFF_TYPE__) p + alignment - 1) & ~(alignment - 1));

  #ifndef __WIN32__
  //printf("malloc addr %d, %X, (%d)\n", pp - p, pp, size + alignment-1 - (pp-p));
  if (mprotect(pp, size + alignment-1 - (pp-p), PROT_READ | PROT_WRITE | PROT_EXEC)) 
  {
    perror("Couldn't mprotect");
    exit(2);
  };
  #endif

  return pp;
}
