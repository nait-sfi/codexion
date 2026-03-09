/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nait-sfi <nait-sfi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 21:48:24 by nait-sfi          #+#    #+#             */
/*   Updated: 2026/03/09 00:25:11 by nait-sfi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_completing(t_sim *sim)
{
	int	completing;

	pthread_mutex_lock(&sim->running_mutex);
	completing = sim->completing;
	pthread_mutex_unlock(&sim->running_mutex);
	return (completing);
}

void	set_completing(t_sim *sim, int value)
{
	pthread_mutex_lock(&sim->running_mutex);
	sim->completing = value;
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

void	precise_usleep_graceful(long long duration_ms, t_sim *sim)
{
	long long	start;

	start = get_time_ms();
	while (get_time_ms() - start < duration_ms)
	{
		if (!is_running(sim))
			return ;
		if (is_completing(sim))
		{
			usleep(200);
			continue ;
		}
		usleep(200);
	}
}

void	log_action(t_sim *sim, int coder_id, char *action)
{
	long long	ts;
	int			running;

	pthread_mutex_lock(&sim->log_mutex);
	pthread_mutex_lock(&sim->running_mutex);
	running = sim->running;
	pthread_mutex_unlock(&sim->running_mutex);
	if (running || strcmp(action, "burned out") == 0)
	{
		ts = get_time_ms() - sim->start_time;
		printf("%lld %d %s\n", ts, coder_id, action);
	}
	pthread_mutex_unlock(&sim->log_mutex);
}
