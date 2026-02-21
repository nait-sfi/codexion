/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nai-sfi <nait-sfi@student.1337.ma>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 15:09:36 by nai-sfi           #+#    #+#             */
/*   Updated: 2026/02/21 15:51:54 by nai-sfi          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <errno.h>

typedef enum e_scheduler
{
	COD_SCHED_FIFO,
	COD_SCHED_EDF
}	t_scheduler;

typedef struct s_heap_node
{
	int			coder_id;
	long long	priority;
}	t_heap_node;

typedef struct s_heap
{
	t_heap_node	*nodes;
	int			size;
	int			capacity;
}	t_heap;

typedef struct s_sim	t_sim;
typedef struct s_coder	t_coder;

typedef struct s_dongle
{
	int				id;
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				available;
	long long		release_time;
	t_heap			queue;
	t_sim			*sim;
}	t_dongle;

struct	s_coder
{
	int				id;
	pthread_t		thread;
	long long		last_compile_start;
	int				compile_count;
	int				left_dongle;
	int				right_dongle;
	int				first_dongle;
	int				second_dongle;
	pthread_mutex_t	state_mutex;
	t_sim			*sim;
};

struct s_sim
{
	int				num_coders;
	long long		time_to_burnout;
	long long		time_to_compile;
	long long		time_to_debug;
	long long		time_to_refactor;
	int				compiles_required;
	long long		dongle_cooldown;
	t_scheduler		scheduler;
	long long		start_time;
	int				running;
	pthread_mutex_t	running_mutex;
	pthread_mutex_t	log_mutex;
	long long		fifo_counter;
	pthread_mutex_t	counter_mutex;
	t_dongle		*dongles;
	t_coder			*coders;
};

int			parse_args(t_sim *sim, int argc, char **argv);

void		heap_init(t_heap *h, int capacity);
void		heap_destroy(t_heap *h);
void		heap_push(t_heap *h, int coder_id, long long priority);
t_heap_node	heap_pop(t_heap *h);
int			heap_peek_id(t_heap *h);
int			heap_is_empty(t_heap *h);
void		heap_remove_coder(t_heap *h, int coder_id);

long long	get_time_ms(void);
void		precise_usleep(long long duration_ms, t_sim *sim);
void		log_action(t_sim *sim, int coder_id, char *action);
int			is_running(t_sim *sim);
void		set_running(t_sim *sim, int value);

int			init_simulation(t_sim *sim);
void		cleanup(t_sim *sim);

int			acquire_dongle(t_coder *coder, int dongle_idx);
void		release_dongle(t_coder *coder, int dongle_idx);
void		wake_all_dongles(t_sim *sim);

void		*coder_routine(void *arg);
void		*monitor_routine(void *arg);

#endif