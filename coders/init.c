/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nait-sfi <nait-sfi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 21:49:31 by nait-sfi          #+#    #+#             */
/*   Updated: 2026/03/03 17:44:21 by nait-sfi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	assign_coder(t_sim *sim, int i)
{
	sim->coders[i].id = i + 1;
	sim->coders[i].compile_count = 0;
	sim->coders[i].sim = sim;
	sim->coders[i].left_dongle = i;
	sim->coders[i].right_dongle = (i + 1) % sim->num_coders;
}

static int	init_coders(t_sim *sim)
{
	int	i;

	sim->coders = malloc(sizeof(t_coder) * sim->num_coders);
	if (!sim->coders)
		return (1);
	i = 0;
	while (i < sim->num_coders)
	{
		memset(&sim->coders[i], 0, sizeof(t_coder));
		assign_coder(sim, i);
		if (sim->coders[i].left_dongle <= sim->coders[i].right_dongle)
		{
			sim->coders[i].first_dongle = sim->coders[i].left_dongle;
			sim->coders[i].second_dongle = sim->coders[i].right_dongle;
		}
		else
		{
			sim->coders[i].first_dongle = sim->coders[i].right_dongle;
			sim->coders[i].second_dongle = sim->coders[i].left_dongle;
		}
		if (pthread_mutex_init(&sim->coders[i].state_mutex, NULL) != 0)
			return (1);
		i++;
	}
	return (0);
}

static int	init_dongles(t_sim *sim)
{
	int	i;

	sim->dongles = malloc(sizeof(t_dongle) * sim->num_coders);
	if (!sim->dongles)
		return (1);
	i = 0;
	while (i < sim->num_coders)
	{
		memset(&sim->dongles[i], 0, sizeof(t_dongle));
		sim->dongles[i].id = i;
		sim->dongles[i].available = 1;
		sim->dongles[i].release_time = 0;
		sim->dongles[i].sim = sim;
		if (pthread_mutex_init(&sim->dongles[i].mutex, NULL) != 0)
			return (1);
		if (pthread_cond_init(&sim->dongles[i].cond, NULL) != 0)
			return (1);
		if (heap_init(&sim->dongles[i].queue, sim->num_coders) != 0)
			return (1);
		if (!sim->dongles[i].queue.nodes)
			return (1);
		i++;
	}
	return (0);
}

int	init_simulation(t_sim *sim)
{
	if (pthread_mutex_init(&sim->running_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->log_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&sim->counter_mutex, NULL) != 0)
		return (1);
	sim->fifo_counter = 0;
	sim->running = 0;
	sim->completing = 0;
	if (init_coders(sim) != 0)
		return (1);
	if (init_dongles(sim) != 0)
		return (1);
	return (0);
}
