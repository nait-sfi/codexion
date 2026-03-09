/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils1.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nait-sfi <nait-sfi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/09 00:24:37 by nait-sfi          #+#    #+#             */
/*   Updated: 2026/03/09 00:26:59 by nait-sfi         ###   ########.fr       */
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
