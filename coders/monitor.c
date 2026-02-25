/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nait-sfi <nait-sfi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 23:27:04 by nait-sfi          #+#    #+#             */
/*   Updated: 2026/02/25 18:06:11 by nait-sfi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_burnout(t_sim *sim)
{
	int			i;
	long long	last;
	long long	now;
	int			success_compiles;

	now = get_time_ms();
	i = 0;
	while (i < sim->num_coders)
	{
		pthread_mutex_lock(&sim->coders[i].state_mutex);
		last = sim->coders[i].last_compile_start;
		success_compiles = sim->coders[i].compile_count;
		pthread_mutex_unlock(&sim->coders[i].state_mutex);
		if (now - last >= sim->time_to_burnout
			&& success_compiles < sim->compiles_required)
			return (sim->coders[i].id);
		i++;
	}
	return (0);
}

static int	check_completion(t_sim *sim)
{
	int	i;
	int	count;

	i = 0;
	while (i < sim->num_coders)
	{
		pthread_mutex_lock(&sim->coders[i].state_mutex);
		count = sim->coders[i].compile_count;
		pthread_mutex_unlock(&sim->coders[i].state_mutex);
		if (count < sim->compiles_required)
			return (0);
		i++;
	}
	return (1);
}

void	*monitor_routine(void *arg)
{
	t_sim	*sim;
	int		burned_id;

	sim = (t_sim *)arg;
	while (is_running(sim))
	{
		usleep(500);
		burned_id = check_burnout(sim);
		if (burned_id > 0)
		{
			log_action(sim, burned_id, "burned out");
			set_running(sim, 0);
			wake_all_dongles(sim);
			return (NULL);
		}
		if (check_completion(sim))
		{
			set_running(sim, 0);
			wake_all_dongles(sim);
			return (NULL);
		}
	}
	return (NULL);
}
