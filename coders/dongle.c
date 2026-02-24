/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nait-sfi <nait-sfi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 15:02:38 by nait-sfi          #+#    #+#             */
/*   Updated: 2026/02/24 13:48:40 by nait-sfi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	try_acquire_loop(t_dongle *dongle, t_coder *coder)
{
	long long		timeout;
	struct timespec	ts;

	if (!is_running(coder->sim))
	{
		heap_remove_coder(&dongle->queue, coder->id);
		pthread_mutex_unlock(&dongle->mutex);
		return (-1);
	}
	if (can_take_dongle(dongle, coder))
	{
		heap_pop(&dongle->queue);
		dongle->available = 0;
		pthread_mutex_unlock(&dongle->mutex);
		return (1);
	}
	timeout = compute_wait_timeout(dongle, coder->sim);
	ts = make_timespec(timeout);
	pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
	return (0);
}

int	acquire_dongle(t_coder *coder, int dongle_idx)
{
	t_dongle	*dongle;
	long long	priority;
	int			result;

	dongle = &coder->sim->dongles[dongle_idx];
	priority = get_priority(coder);
	pthread_mutex_lock(&dongle->mutex);
	heap_push(&dongle->queue, coder->id, priority);
	while (1)
	{
		result = try_acquire_loop(dongle, coder);
		if (result != 0)
			return (result == 1);
	}
}

void	release_dongle(t_coder *coder, int dongle_idx)
{
	t_dongle	*dongle;

	dongle = &coder->sim->dongles[dongle_idx];
	pthread_mutex_lock(&dongle->mutex);
	dongle->available = 1;
	dongle->release_time = get_time_ms();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

void	wake_all_dongles(t_sim *sim)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		pthread_mutex_lock(&sim->dongles[i].mutex);
		pthread_cond_broadcast(&sim->dongles[i].cond);
		pthread_mutex_unlock(&sim->dongles[i].mutex);
		i++;
	}
}
