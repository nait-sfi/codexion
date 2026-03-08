/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nait-sfi <nait-sfi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 13:53:07 by nait-sfi          #+#    #+#             */
/*   Updated: 2026/03/08 15:08:09 by nait-sfi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_priority(t_coder *coder)
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

struct timespec	make_timespec(long long abs_time_ms)
{
	struct timespec	ts;

	ts.tv_sec = abs_time_ms / 1000;
	ts.tv_nsec = (abs_time_ms % 1000) * 1000000L;
	return (ts);
}

int	can_take_dongle(t_dongle *dongle, t_coder *coder)
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

long long	compute_wait_timeout(t_dongle *dongle, t_sim *sim)
{
	long long	now;
	long long	cooldown_end;

	now = get_time_ms();
	cooldown_end = dongle->release_time + sim->dongle_cooldown;
	if (dongle->available && now < cooldown_end)
		return (cooldown_end);
	return (now + 5);
}
