/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nait-sfi <nait-sfi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 21:48:24 by nait-sfi          #+#    #+#             */
/*   Updated: 2026/03/02 12:22:49 by nait-sfi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((long long)tv.tv_sec * 1000LL + (long long)tv.tv_usec / 1000LL);
}

int	is_running(t_sim *sim)
{
	int	running;

	pthread_mutex_lock(&sim->running_mutex);
	running = sim->running;
	pthread_mutex_unlock(&sim->running_mutex);
	return (running);
}

void	set_running(t_sim *sim, int value)
{
	pthread_mutex_lock(&sim->running_mutex);
	sim->running = value;
	pthread_mutex_unlock(&sim->running_mutex);
}

void	precise_usleep(long long duration_ms, t_sim *sim)
{
	long long	start;

	start = get_time_ms();
	while (get_time_ms() - start < duration_ms)
	{
		if (!is_running(sim))
			return ;
		usleep(200);
	}
}

void	log_action(t_sim *sim, int coder_id, char *action)
{
	long long	ts;
	int			running;

	pthread_mutex_lock(&sim->running_mutex);
	running = sim->running;
	pthread_mutex_unlock(&sim->running_mutex);
	pthread_mutex_lock(&sim->log_mutex);
	if (running || strcmp(action, "burned out") == 0)
	{
		ts = get_time_ms() - sim->start_time;
		printf("%lld %d %s\n", ts, coder_id, action);
	}
	pthread_mutex_unlock(&sim->log_mutex);
}
