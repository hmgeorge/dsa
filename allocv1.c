#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

unsigned char mem[1024];
size_t alloc_end;

#define ALIGN(x, p2) (((x)+(p2)-1)&~((p2)-1))
#define div_round_up(x, y) (((x)+(y)-1)/(y))
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

#define MIN_ALLOCATION 8

struct header {
  int size;
  unsigned char free;
  int prev_idx;
  int next_idx;
} __attribute__((aligned(MIN_ALLOCATION)));

struct footer {
  int size;
  char reserve:1;
} __attribute__((aligned(MIN_ALLOCATION)));

#define HEADER_SIZE sizeof(struct header)
#define FOOTER_SIZE sizeof(struct footer)
#define META_SIZE (HEADER_SIZE + FOOTER_SIZE)
#define ID_TO_HEADER(idx) (struct header*)(mem+idx)
#define HEADER_TO_MEM(hdr) (uint8_t *)((hdr) + HEADER_SIZE)
#define PTR_TO_HDR(ptr) (struct header*)((uint8_t*)(ptr) - HEADER_SIZE)
#define HEADER_TO_ID(hdr) (size_t)(mem-(uint8_t*)hdr)
#define HEADER_FROM_FOOTER(footer) (struct header *)((uint8_t *)(footer) - (footer->size - FOOTER_SIZE))
#define FOOTER_FROM_HEADER(hdr) (struct footer *)(hdr + hdr->size - FOOTER_SIZE)
#define NEXT_HDR(hdr) (struct header *)((uint8_t*)(hdr) + hdr->size)
#define BLOCK_SIZE(size) (ALIGN(size, MIN_ALLOCATION) + META_SIZE)

#define IS_BASE(hdr) ((uint8_t *)hdr == mem)
#define IS_END(hdr) (((uint8_t *)hdr+hdr->size) == (mem+ARRAY_SIZE(mem)))
/*
   bin start indexes for allocations of size
  8 (min), 16, 24, 32, 40, 48, 56, 64
*/

int bin_head[ ] = {-1, -1, -1, -1, -1, -1, -1, -1};
int sorted_bin_head = -1; // catch all for all other blocks

void
add_to_bin(struct header *hdr)
{
  size_t mem_size = hdr->size - META_SIZE;
  size_t mem_idx = div_round_up(mem_size, MIN_ALLOCATION);
  if (mem_idx < ARRAY_SIZE(bin_head)) {
    struct header *first_hdr = ID_TO_HEADER(bin_head[mem_idx]);
    first_hdr->prev_idx = HEADER_TO_ID(hdr);
    hdr->prev_idx = -1;
    hdr->next_idx = HEADER_TO_ID(first_hdr);
  } else {
    if (sorted_bin_head == -1) {
      sorted_bin_head = HEADER_TO_ID(hdr);
    } else {
      size_t next_idx = sorted_bin_head;
      while (next_idx != -1) {
        struct header *next_hdr = ID_TO_HEADER(next_idx);
        if (next_hdr->size <= hdr->size) {
          next_hdr->prev_idx = HEADER_TO_ID(hdr);
          hdr->prev_idx = next_hdr->prev_idx;
          hdr->next_idx = HEADER_TO_ID(next_hdr);
          break;
        } else {
          next_idx = next_hdr->next_idx;
        }
      }
    }
  }
}

int
make_room(size_t size) {
  size_t avail = ARRAY_SIZE(mem) - alloc_end;
  if (avail < (BLOCK_SIZE(size))) {
    return -1;
  }

  size_t idx = alloc_end + ALIGN(size, MIN_ALLOCATION);
  struct header *hdr = ID_TO_HEADER(idx);
  hdr->size = BLOCK_SIZE(size);
  struct footer *footer = FOOTER_FROM_HEADER(hdr);
  footer->size = hdr->size;
  alloc_end += hdr->size;
  add_to_bin(hdr);
}

struct header *
try_alloc_from_sorted_head(size_t size)
{
  if (sorted_bin_head == -1) {
    return NULL;
  } else {
    size_t next_idx = sorted_bin_head;
    struct header *next_hdr = NULL;

    while (next_idx != -1) {
      next_hdr = ID_TO_HEADER(next_idx);
      if (next_hdr->free && next_hdr->size < (size+META_SIZE)) {
          break;
      }
      next_idx = next_hdr->next_idx;
    }

    if (next_idx != -1) {
      // split it into two chunks.
      // it's an assertion error if size of this chunk is < 64

      // only split if the remainder is atleast 8 bytes
      size_t remainder = next_hdr->size - BLOCK_SIZE(size);
      if (remainder < MIN_ALLOCATION) {
        return next_hdr;
      } else {
        struct header *left_over_hdr = (struct header *)((uint8_t *)next_hdr+BLOCK_SIZE(size));
        left_over_hdr->free=1;
        left_over_hdr->size = remainder + META_SIZE;
        struct footer *left_over_footer =  FOOTER_FROM_HEADER(left_over_hdr);
        left_over_footer->size = left_over_hdr->size;
        add_to_bin(left_over_hdr);

        next_hdr->size = BLOCK_SIZE(size);
        struct footer *next_footer = FOOTER_FROM_HEADER(next_hdr);
        next_hdr->size = next_hdr->size;
        return next_hdr;
      }
    }
  }
  return NULL;
}

void *
alloc(size_t size) {
  /*
    if size < 512 :
        find exact bin closest to size.
        iterate through next_idx to find a free header.
          if found : mark it as used and return it
          if not found: take from free mem if available
    if size > 512 :
        find bin from closest bins and return it
  */
  int tries = 2;
  int idx;
  struct header *hdr = NULL;
  do {  
    int bin = div_round_up(size, ARRAY_SIZE(bin_head)) - 1;
    if (bin >= ARRAY_SIZE(bin_head)) {
      hdr = try_alloc_from_sorted_head(size);
    } else {
      idx = bin_head[bin];
      while (idx != -1) {
        hdr = ID_TO_HEADER(idx);
        if (hdr->free) {
          break;
        }
        idx = hdr->next_idx;
      }
    }

    if (hdr == NULL) {
      if (make_room(size) < 0) {
        return NULL;
      }
    }
  } while(--tries);

  hdr->free=0;
  return HEADER_TO_MEM(hdr);
}

struct header *
coalesce(struct header *left_hdr,
         struct header *hdr)
{
  left_hdr->size += hdr->size;
  if (hdr->prev_idx != -1) {
    struct header *prev_hdr = ID_TO_HEADER(hdr->prev_idx);
    prev_hdr->next_idx = hdr->next_idx;
  }

  if (hdr->next_idx != -1) {
    struct header *next_hdr = ID_TO_HEADER(hdr->next_idx);
    next_hdr->prev_idx = hdr->prev_idx;
  }
  struct footer *footer = FOOTER_FROM_HEADER(left_hdr);
  footer->size = left_hdr->size;
  return left_hdr;
}

void
dealloc(void *ptr)
{
  // start of blob = ptr - sizeof(struct header);
  // footer of previous blob = start - sizeof(footer)
  // if they are both free, create a single blob of size
  // 8 + X + Y + 8. then decide where to put it .. (if size < 512 and size > 512

  struct header * hdr = PTR_TO_HDR(ptr);
  if (hdr->free) {
    return; // double free
  }

  hdr->free = 1;

  // coalesce adj free mem

  // is my left neighbour free
  // coalesce me and left
  if (!IS_BASE(hdr)) {
    struct footer *left_footer = (struct footer *)((uint8_t*)hdr - FOOTER_SIZE);
    struct header *left_hdr = hdr - left_footer->size;
    if (left_hdr->free) {
      hdr = coalesce(left_hdr, hdr);
    }
  }
  
  // is my right neighbour free
  // coalesce me and right
  if (!IS_END(hdr)) {
    struct header *right_hdr = NEXT_HDR(hdr);
    if (right_hdr->free) {
      hdr = coalesce(hdr, right_hdr);
    }
  }

  add_to_bin(hdr);
}

void
init()
{
  /* in the beginning the entire mem is a blob */
  struct header h;
  struct footer f;
  h.size=f.size=ARRAY_SIZE(mem);
  h.prev_idx=h.next_idx=-1;
  memcpy(mem, &h, HEADER_SIZE);
  memcpy(mem + h.size - FOOTER_SIZE, &f, FOOTER_SIZE);
}

int
main() {
  //init();
  //void *ptr = alloc(32);
  char mem[16];
  struct header h;
  struct footer f;
  h.size=f.size=ARRAY_SIZE(mem);
  h.prev_idx=h.next_idx=-1;
  memcpy(mem, &h, HEADER_SIZE);
  memcpy(mem + h.size - FOOTER_SIZE, &f, FOOTER_SIZE);
  void *ptr = HEADER_TO_MEM(mem);
  void *ptr2 = (void *)malloc(HEADER_SIZE+FOOTER_SIZE);
  fprintf(stderr, "%zd %zd ptr %p, mem %p, ptr2 %p\n", HEADER_SIZE, FOOTER_SIZE,
          ptr, mem, ptr2);
}

// fyi .. array search functions in C . bsearch, lsearch
