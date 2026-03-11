/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nait-sfi <nait-sfi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 23:27:09 by nait-sfi          #+#    #+#             */
/*   Updated: 2026/03/11 02:59:52 by nait-sfi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	acquire_dongles(t_coder *coder)
{
	if (!acquire_dongle(coder, coder->first_dongle))
		return (0);
	log_action(coder->sim, coder->id, "has taken a dongle");
	if (!acquire_dongle(coder, coder->second_dongle))
	{
		release_dongle(coder, coder->first_dongle);
		return (0);
	}
	log_action(coder->sim, coder->id, "has taken a dongle");
	return (1);
}

static void	do_compile(t_coder *coder)
{
	pthread_mutex_lock(&coder->state_mutex);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->state_mutex);
	log_action(coder->sim, coder->id, "is compiling");
	precise_usleep(coder->sim->time_to_compile, coder->sim);
	release_dongle(coder, coder->first_dongle);
	release_dongle(coder, coder->second_dongle);
	pthread_mutex_lock(&coder->state_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->state_mutex);
}

int	get_my_compiles(t_coder *coder)
{
	int	my_compiles;

	pthread_mutex_lock(&coder->state_mutex);
	my_compiles = coder->compile_count;
	pthread_mutex_unlock(&coder->state_mutex);
	return (my_compiles);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->id % 2 == 0)
		usleep(1000);
	while (is_running(coder->sim))
	{
		if (get_my_compiles(coder) >= coder->sim->compiles_required)
			break ;
		if (!acquire_dongles(coder))
			break ;
		do_compile(coder);
		if (!is_running(coder->sim))
			break ;
		log_action(coder->sim, coder->id, "is debugging");
		precise_usleep(coder->sim->time_to_debug, coder->sim);
		if (!is_running(coder->sim))
			break ;
		log_action(coder->sim, coder->id, "is refactoring");
		precise_usleep(coder->sim->time_to_refactor, coder->sim);
		if (is_completing(coder->sim))
			break ;
	}
	return (NULL);
}
