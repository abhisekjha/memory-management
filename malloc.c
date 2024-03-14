/*Abhisek Kumar Jha 1001859565*/

#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define ALIGN4(s)         (((((s) - 1) >> 2) << 2) + 4)
#define BLOCK_DATA(b)     ((b) + 1)
#define BLOCK_HEADER(ptr) ((struct _block *)(ptr) - 1)

static int atexit_registered = 0;
static int num_mallocs       = 0;
static int num_frees         = 0;
static int num_reuses        = 0;
static int num_grows         = 0;
static int num_splits        = 0;
static int num_coalesces     = 0;
static int num_blocks        = 0;
static int num_requested     = 0;
static int max_heap          = 0;

/*
 *  \brief printStatistics
 *
 *  \param none
 *
 *  Prints the heap statistics upon process exit.  Registered
 *  via atexit()
 *
 *  \return none
 */
void printStatistics( void )
{
  printf("\nheap management statistics\n");
  printf("mallocs:\t%d\n", num_mallocs );
  printf("frees:\t\t%d\n", num_frees );
  printf("reuses:\t\t%d\n", num_reuses );
  printf("grows:\t\t%d\n", num_grows );
  printf("splits:\t\t%d\n", num_splits );
  printf("coalesces:\t%d\n", num_coalesces );
  printf("blocks:\t\t%d\n", num_blocks );
  printf("requested:\t%d\n", num_requested );
  printf("max heap:\t%d\n", max_heap );
}

struct _block 
{
   size_t  size;         /* Size of the allocated _block of memory in bytes */
   struct _block *next;  /* Pointer to the next _block of allcated memory   */
   bool   free;          /* Is this _block free?                            */
   char   padding[3];    /* Padding: IENTRTMzMjAgU3ByaW5nIDIwMjM            */
};


struct _block *heapList = NULL; /* Free list to track the _blocks available */

struct _block *findFreeBlock(struct _block **last, size_t size) 
{
   struct _block *curr = heapList; 

#if defined FIT && FIT == 0
   /* First fit */
   while (curr && !(curr->free && curr->size >= size)) 
   {
      *last = curr;
      curr  = curr->next;
   }
#endif

//Best Fit code in this #ifdef block
#if defined BEST && BEST == 0
   /** \TODO Implement best fit here */
   /*Best Fit*/
   struct _block *best_fit = NULL;

   size_t min_difference = INT_MAX;

   while(curr != NULL)
   {
      *last = curr;

      if ((curr->free) && (curr->size >= size))
      {
         size_t difference = (int)(curr->size - size);
         if(difference < min_difference)
         {
            min_difference = difference;
            best_fit = curr;
         }
      }
      curr = curr-> next;
   }
   curr = best_fit;

#endif

//Worst Fit code in this #ifdef block
#if defined WORST && WORST == 0
   /** \TODO Implement worst fit here */

   /*Worst fit*/
   struct _block *worst_block = NULL;
   size_t max_diff =0;
   while(curr != NULL)
   {
      *last = curr;

      if((curr->free) &&(curr->size >= size))
      {
         size_t diff= (int)(curr->size - size);
         if(diff > max_diff)
         {
            max_diff = diff;
            worst_block = curr;
         }
      }
      
      curr = curr->next;
   }
   curr = worst_block;
#endif

// Next Fit code in this #ifdef block
#if defined NEXT && NEXT == 0
   /** \TODO Implement next fit here */

   /*Next Fit*/
   static struct _block *last_visited = NULL;

   if(last_visited ==NULL )
   {
      last_visited = heapList;
   }
   curr= last_visited;
   
   //search for a suitable block starting from the last visited block
   while(curr)
   {
      if(curr->free && curr->size >= size)
      {
         last_visited= curr-> next;
         break;
      }
      *last = curr;
      curr = curr ->next;
   }

   //if no suitabke block is found and start searching from the beginging of the list

   if(curr== NULL)
   {
      curr = heapList;
      while(curr && !(curr ->free && curr->size >= size))
      {
         *last = curr;
         curr = curr-> next;
      }
   }

#endif

   return curr;
}

/*
 * \brief growheap
 *
 * Given a requested size of memory, use sbrk() to dynamically 
 * increase the data segment of the calling process.  Updates
 * the free list with the newly allocated memory.
 *
 * \param last tail of the free _block list
 * \param size size in bytes to request from the OS
 *
 * \return returns the newly allocated _block of NULL if failed
 */
struct _block *growHeap(struct _block *last, size_t size) 
{
   /* Request more space from OS */
   struct _block *curr = (struct _block *)sbrk(0);
   struct _block *prev = (struct _block *)sbrk(sizeof(struct _block) + size);

   assert(curr == prev);

   /* OS allocation failed */
   if (curr == (struct _block *)-1) 
   {
      return NULL;
   }

   /* Update heapList if not set */
   if (heapList == NULL) 
   {
      heapList = curr;
   }

   /* Attach new _block to previous _block */
   if (last) 
   {
      last->next = curr;
   }

   /* Update _block metadata:
      Set the size of the new block and initialize the new block to "free".
      Set its next pointer to NULL since it's now the tail of the linked list.
   */
   curr->size = size;
   curr->next = NULL;
   curr->free = false;

   num_blocks++;
   max_heap += size + sizeof(struct _block);

   return curr;
}

/*
 * \brief malloc
 *
 * finds a free _block of heap memory for the calling process.
 * if there is no free _block that satisfies the request then grows the 
 * heap and returns a new _block
 *
 * \param size size of the requested memory in bytes
 *
 * \return returns the requested memory allocation to the calling process 
 * or NULL if failed
 */
void *malloc(size_t size) 
{

   if( atexit_registered == 0 )
   {
      atexit_registered = 1;
      atexit( printStatistics );
   }

   /* Align to multiple of 4 */
   size = ALIGN4(size);

   /* Handle 0 size */
   if (size == 0) 
   {
      return NULL;
   }

   /* Look for free _block.  If a free block isn't found then we need to grow our heap. */

   struct _block *last = heapList;
   struct _block *next = findFreeBlock(&last, size);

   /* TODO: If the block found by findFreeBlock is larger than we need then:
            If the leftover space in the new block is greater than the sizeof(_block)+4 then
            split the block.
            If the leftover space in the new block is less than the sizeof(_block)+4 then
            don't split the block.
   */

   /* Could not find free _block, so grow heap */
   if (next == NULL) 
   {
      next = growHeap(last, size);
   }

   /* Could not find free _block or grow heap, so just return NULL */
   if (next == NULL) 
   {
      return NULL;
   }
   
   /* Mark _block as in use */
   next->free = false;

   if (next->size >= size + sizeof(struct _block) + 4) 
   {
      struct _block *new_block = (struct _block *)((char *)BLOCK_DATA(next) + size);
      new_block->size = next->size - size - sizeof(struct _block);
      new_block->next = next->next;
      new_block->free = true;

      next->size = size;
      next->next = new_block;

      num_splits++;
   }

   num_mallocs++;
   num_requested += size;

   /* Return data address associated with _block to the user */
   return BLOCK_DATA(next);
}

/*
 * \brief free
 *
 * frees the memory _block pointed to by pointer. if the _block is adjacent
 * to another _block then coalesces (combines) them
 *
 * \param ptr the heap memory to free
 *
 * \return none
 */
void free(void *ptr) 
{
   if (ptr == NULL) 
   {
      return;
   }

   /* Make _block as free */
   struct _block *curr = BLOCK_HEADER(ptr);
   assert(curr->free == 0);
   curr->free = true;

   /* Coalesce with previous block if possible */
   struct _block *prev = heapList;
   if (prev && prev->next == curr && prev->free) 
   {
      prev->size += sizeof(struct _block) + curr->size;
      prev->next = curr->next;
      curr = prev;
      num_coalesces++;
   }
 
   /* Coalesce with next block if possible */
   if (curr->next && curr->next->free)
   {
      curr->size += sizeof(struct _block) + curr->next->size;
      curr->next = curr->next->next;
      num_coalesces++;
   }
 
   num_frees++;
}

void *calloc( size_t nmemb, size_t size )
{
   // \TODO Implement calloc
   size_t total_size = nmemb * size;
   void *ptr = malloc(total_size);
   if (ptr) 
   {
      memset(ptr, 0, total_size);
   }
   return ptr;
}

void *realloc( void *ptr, size_t size )
{
    if (ptr == NULL) 
    {
      return malloc(size);
    }
    if (size == 0) 
    {
        free(ptr);
        return NULL;
    }

    struct _block *old_block = BLOCK_HEADER(ptr);
    size_t old_size = old_block->size;
    size_t copy_size = old_size < size ? old_size : size;

    void *newPtr = malloc(size);
    if (newPtr) 
    {
        memcpy(newPtr, ptr, copy_size);
        free(ptr);
    }
    return newPtr; 

}



/* vim: IENTRTMzMjAgU3ByaW5nIDIwMjM= -----------------------------------------*/
/* vim: set expandtab sts=3 sw=3 ts=6 ft=cpp: --------------------------------*/
