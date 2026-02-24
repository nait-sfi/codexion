/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nait-sfi <nait-sfi@student.1337.ma>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 23:27:19 by nai-sfi           #+#    #+#             */
/*   Updated: 2026/02/23 23:27:20 by nai-sfi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	create_threads(t_sim *sim, pthread_t *monitor)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		sim->coders[i].last_compile_start = sim->start_time;
		if (pthread_create(&sim->coders[i].thread, NULL,
				coder_routine, &sim->coders[i]) != 0)
		{
			fprintf(stderr, "Error: failed to create coder thread\n");
			set_running(sim, 0);
			return (1);
		}
		i++;
	}
	if (pthread_create(monitor, NULL, monitor_routine, sim) != 0)
	{
		fprintf(stderr, "Error: failed to create monitor thread\n");
		set_running(sim, 0);
		return (1);
	}
	return (0);
}

static void	join_threads(t_sim *sim, pthread_t monitor)
{
	int	i;

	i = 0;
	while (i < sim->num_coders)
	{
		pthread_join(sim->coders[i].thread, NULL);
		i++;
	}
	pthread_join(monitor, NULL);
}

int	main(int argc, char **argv)
{
	t_sim		sim;
	pthread_t	monitor;

	memset(&sim, 0, sizeof(t_sim));
	if (parse_args(&sim, argc, argv) != 0)
		return (1);
	if (init_simulation(&sim) != 0)
	{
		cleanup(&sim);
		return (1);
	}
	sim.start_time = get_time_ms();
	sim.running = 1;
	if (create_threads(&sim, &monitor) != 0)
	{
		cleanup(&sim);
		return (1);
	}
	join_threads(&sim, monitor);
	cleanup(&sim);
	return (0);
}
