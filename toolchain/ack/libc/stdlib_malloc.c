#include "types.h"

extern char *sbrk(isize increment);

struct free_block {
	u32 size;
	struct free_block *next;
};

static struct free_block *free_list;

void *
malloc(usize n)
{
	u32 need;
	u32 total;
	u32 remainder;
	u32 *p;
	struct free_block *block;
	struct free_block *previous;
	struct free_block *next;

	if (n == 0)
		n = 1;
	need = ((u32)n + 3) & ~3L;
	total = need + sizeof(u32);
	previous = (struct free_block *)0;
	for (block = free_list; block; block = block->next) {
		if (block->size >= total) {
			remainder = block->size - total;
			if (remainder >= sizeof(struct free_block)) {
				next = (struct free_block *)((char *)block + total);
				next->size = remainder;
				next->next = block->next;
				if (previous)
					previous->next = next;
				else
					free_list = next;
				block->size = need;
			} else {
				if (previous)
					previous->next = block->next;
				else
					free_list = block->next;
				block->size -= sizeof(u32);
			}
			return (void *)((u32 *)block + 1);
		}
		previous = block;
	}
	p = (u32 *)sbrk((isize)total);
	if ((char *)p == (char *)-1)
		return (void *)0;
	*p = need;
	return (void *)(p + 1);
}

void
free(void *p)
{
	struct free_block *block;
	struct free_block *current;
	struct free_block *previous;

	if (p == (void *)0)
		return;
	block = (struct free_block *)((u32 *)p - 1);
	block->size += sizeof(u32);
	previous = (struct free_block *)0;
	current = free_list;
	while (current && current < block) {
		previous = current;
		current = current->next;
	}
	block->next = current;
	if (previous)
		previous->next = block;
	else
		free_list = block;
	if (current && (char *)block + block->size == (char *)current) {
		block->size += current->size;
		block->next = current->next;
	}
	if (previous && (char *)previous + previous->size == (char *)block) {
		previous->size += block->size;
		previous->next = block->next;
	}
}

void *
calloc(usize count, usize size)
{
	u32 wide;
	usize n;
	char *p;
	usize i;

	wide = (u32)count * (u32)size;
	if (wide > 65535L)
		return (void *)0;
	n = (usize)wide;
	p = (char *)malloc(n);
	if (p == (char *)0)
		return (void *)0;
	for (i = 0; i < n; i++)
		p[i] = '\0';
	return p;
}

void *
realloc(void *old, usize n)
{
	char *p;
	char *q;
	usize oldn;
	usize i;

	if (old == (void *)0)
		return malloc(n);
	if (n == 0)
		n = 1;
	oldn = *((u32 *)old - 1);
	if (oldn >= (u32)n)
		return old;
	p = (char *)malloc(n);
	if (p == (char *)0)
		return (void *)0;
	q = (char *)old;
	if (oldn > n)
		oldn = n;
	for (i = 0; i < oldn; i++)
		p[i] = q[i];
	free(old);
	return p;
}
