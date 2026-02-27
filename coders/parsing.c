/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nait-sfi <nait-sfi@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/24 13:46:47 by nait-sfi          #+#    #+#             */
/*   Updated: 2026/02/27 15:17:04 by nait-sfi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	ft_atoi_strict(const char *str, int *out)
{
	int		i;
	long	num;

	i = 0;
	num = 0;
	if (!str || str[0] == '\0')
		return (0);
	if (str[i] == '+')
		i++;
	if (str[i] == '\0')
		return (0);
	while (str[i] != '\0')
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		num = num * 10 + (str[i] - '0');
		if (num > 2147483647)
			return (0);
		i++;
	}
	*out = (int)num;
	return (1);
}

static int	print_error(char *msg)
{
	fprintf(stderr, "%s", msg);
	return (1);
}

static int	parse_helper(t_sim *sim, char **argv)
{
	int	tmp;

	if (!ft_atoi_strict(argv[7], &tmp) || tmp < 0)
		return (print_error("Error: invalid dongle_cooldown\n"));
	sim->dongle_cooldown = (long long)tmp;
	if (strcmp(argv[8], "fifo") == 0)
		sim->scheduler = COD_SCHED_FIFO;
	else if (strcmp(argv[8], "edf") == 0)
		sim->scheduler = COD_SCHED_EDF;
	else
		return (print_error("Error: scheduler must be fifo or edf\n"));
	return (0);
}

int	parse_args(t_sim *sim, int argc, char **argv)
{
	int	tmp;

	if (argc != 9)
		return (print_error("Error: expected 8 arguments\n"));
	if (!ft_atoi_strict(argv[1], &tmp) || tmp < 1)
		return (print_error("Error: invalid number_of_coders\n"));
	sim->num_coders = tmp;
	if (!ft_atoi_strict(argv[2], &tmp) || tmp <= 0)
		return (print_error("Error: invalid time_to_burnout\n"));
	sim->time_to_burnout = (long long)tmp;
	if (!ft_atoi_strict(argv[3], &tmp) || tmp <= 0)
		return (print_error("Error: invalid time_to_compile\n"));
	sim->time_to_compile = (long long)tmp;
	if (!ft_atoi_strict(argv[4], &tmp) || tmp <= 0)
		return (print_error("Error: invalid time_to_debug\n"));
	sim->time_to_debug = (long long)tmp;
	if (!ft_atoi_strict(argv[5], &tmp) || tmp <= 0)
		return (print_error("Error: invalid time_to_refactor\n"));
	sim->time_to_refactor = (long long)tmp;
	if (!ft_atoi_strict(argv[6], &tmp) || tmp < 1)
		return (print_error("Error: invalid number_of_compiles\n"));
	sim->compiles_required = tmp;
	return (parse_helper(sim, argv));
}
