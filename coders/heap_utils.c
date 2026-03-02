/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nait-sfi <nait-sfi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 17:49:54 by nait-sfi          #+#    #+#             */
/*   Updated: 2026/03/02 12:22:24 by nait-sfi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	swap(t_heap_node *a, t_heap_node *b)
{
	t_heap_node	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

void	bubble_up(t_heap *h, int i)
{
	int	parent;

	while (i > 0)
	{
		parent = (i - 1) / 2;
		if (h->nodes[i].priority < h->nodes[parent].priority)
		{
			swap(&h->nodes[i], &h->nodes[parent]);
			i = parent;
		}
		else
			break ;
	}
}

void	bubble_down(t_heap *h, int i)
{
	int	smallest;
	int	left;
	int	right;

	while (1)
	{
		smallest = i;
		left = 2 * i + 1;
		right = 2 * i + 2;
		if (left < h->size
			&& h->nodes[left].priority < h->nodes[smallest].priority)
			smallest = left;
		if (right < h->size
			&& h->nodes[right].priority < h->nodes[smallest].priority)
			smallest = right;
		if (smallest == i)
			break ;
		swap(&h->nodes[i], &h->nodes[smallest]);
		i = smallest;
	}
}

int	heap_peek_id(t_heap *h)
{
	return (h->nodes[0].coder_id);
}

int	heap_is_empty(t_heap *h)
{
	return (h->size == 0);
}
