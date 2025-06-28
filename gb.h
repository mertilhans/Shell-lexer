#ifndef GB_H
# define GB_H

# include <stdlib.h>

typedef struct s_gc_node
{
        void    *ptr;
        struct s_gc_node *next;
} t_gb_node;

void    *gb_malloc(size_t size);
void    gb_free_one(void *ptr);
void    gb_free_all(void);

#endif