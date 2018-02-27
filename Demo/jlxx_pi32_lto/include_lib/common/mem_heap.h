#ifndef _MEM_HEAP_H_
#define _MEM_HEAP_H_

#ifdef __cplusplus
extern "C" {
#endif
//extern void *sbrk(void *addr, int incr);
extern void *malloc(unsigned int size);
extern void *calloc(unsigned int count, unsigned int size);
extern void *realloc(void *rmem, unsigned int newsize);
extern void  free(void *mem);
extern void malloc_stats(void);

#ifdef __cplusplus
}
#endif

#endif /* _MEM_HEAP_H_ */
