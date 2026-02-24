/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nait-sfi <nait-sfi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 15:02:38 by nai-sfi           #+#    #+#             */
/*   Updated: 2026/02/23 23:36:13 by nait-sfi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static long long	get_priority(t_coder *coder)
{
	long long	priority;

	if (coder->sim->scheduler == COD_SCHED_FIFO)
	{
		pthread_mutex_lock(&coder->sim->counter_mutex);
		priority = coder->sim->fifo_counter;
		coder->sim->fifo_counter++;
		pthread_mutex_unlock(&coder->sim->counter_mutex);
	}
	else
	{
		pthread_mutex_lock(&coder->state_mutex);
		priority = coder->last_compile_start + coder->sim->time_to_burnout;
		pthread_mutex_unlock(&coder->state_mutex);
	}
	return (priority);
}
static struct timespec	make_timespec(long long abs_time_ms)
{
	struct timespec	ts;

	ts.tv_sec = abs_time_ms / 1000;
	ts.tv_nsec = (abs_time_ms % 1000) * 1000000L;
	return (ts);
}
static int	can_take_dongle(t_dongle *dongle, t_coder *coder)
{
	long long	cooldown_end;

	if (!dongle->available)
		return (0);
	if (heap_is_empty(&dongle->queue))
		return (0);
	if (heap_peek_id(&dongle->queue) != coder->id)
		return (0);
	cooldown_end = dongle->release_time + coder->sim->dongle_cooldown;
	if (get_time_ms() < cooldown_end)
		return (0);
	return (1);
}

static long long	compute_wait_timeout(t_dongle *dongle, t_sim *sim)
{
	long long	now;
	long long	cooldown_end;

	now = get_time_ms();
	cooldown_end = dongle->release_time + sim->dongle_cooldown;
	if (dongle->available && now < cooldown_end)
		return (cooldown_end);
	return (now + 5);
}

int	acquire_dongle(t_coder *coder, int dongle_idx)
{
	t_dongle		*dongle;
	long long		priority;
	struct timespec	ts;
	long long		timeout;

	dongle = &coder->sim->dongles[dongle_idx];
	priority = get_priority(coder);
	pthread_mutex_lock(&dongle->mutex);
	heap_push(&dongle->queue, coder->id, priority);
	while (1)
	{
		if (!is_running(coder->sim))
		{
			heap_remove_coder(&dongle->queue, coder->id);
			pthread_mutex_unlock(&dongle->mutex);
			return (0);
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