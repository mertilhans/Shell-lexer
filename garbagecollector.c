#include "gb.h"

static t_gb_node *gb_head = NULL;

void	*gb_malloc(size_t size)
{
	void *mem = malloc(size);
	if (!mem)
		return (NULL);

	t_gb_node *new_node = malloc(sizeof(t_gb_node));
	if (!new_node)
	{
		free(mem);
		return (NULL);
	}
	new_node->ptr = mem;
	new_node->next = gb_head;
	gb_head = new_node;
	return mem;
}

void	gb_free_one(void *ptr)
{
	t_gb_node *curr = gb_head;
	t_gb_node *prev = NULL;

	while (curr)
	{
		if (curr->ptr == ptr)
		{
			if (prev)
				prev->next = curr->next;
			else
				gb_head = curr->next;
			free(curr->ptr);
			free(curr);
			return;
		}
		prev = curr;
		curr = curr->next;
	}
}

void	gb_free_all(void)
{
	t_gb_node *tmp;

	while (gb_head)
	{
		tmp = gb_head;
		gb_head = gb_head->next;
		free(tmp->ptr);
		free(tmp);
	}
}
