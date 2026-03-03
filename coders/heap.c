/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nait-sfi <nait-sfi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 15:52:27 by nait-sfi          #+#    #+#             */
/*   Updated: 2026/03/03 17:44:56 by nait-sfi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	heap_init(t_heap *h, int capacity)
{
	h->nodes = malloc(sizeof(t_heap_node) * capacity);
	if (!h->nodes)
	{
		fprintf(stderr, "Error: heap malloc failed\n");
		return (1);
	}
	h->size = 0;
	h->capacity = capacity;
	return (0);
}

void	heap_destroy(t_heap *h)
{
	free(h->nodes);
	h->nodes = NULL;
	h->size = 0;
	h->capacity = 0;
}

void	heap_push(t_heap *h, int coder_id, long long priority)
{
	if (h->size >= h->capacity)
	{
		fprintf(stderr, "Error: heap overflow\n");
		return ;
	}
	h->nodes[h->size].coder_id = coder_id;
	h->nodes[h->size].priority = priority;
	h->size++;
	bubble_up(h, h->size - 1);
}

t_heap_node	heap_pop(t_heap *h)
{
	t_heap_node	top;

	top = h->nodes[0];
	h->size--;
	if (h->size > 0)
	{
		h->nodes[0] = h->nodes[h->size];
		bubble_down(h, 0);
	}
	return (top);
}

void	heap_remove_coder(t_heap *h, int coder_id)
{
	int	i;

	i = 0;
	while (i < h->size)
	{
		if (h->nodes[i].coder_id == coder_id)
		{
			h->size--;
			if (i < h->size)
			{
				h->nodes[i] = h->nodes[h->size];
				bubble_up(h, i);
				bubble_down(h, i);
			}
			return ;
		}
		i++;
	}
}
