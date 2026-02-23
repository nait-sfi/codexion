#include "codexion.h"

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->id % 2 == 0)
		usleep(1000);
	while (is_running(coder->sim))
	{
		if (!acquire_dongle(coder, coder->first_dongle))
			break ;
		log_action(coder->sim, coder->id, "has taken a dongle");
		if (!acquire_dongle(coder, coder->second_dongle))
		{
			release_dongle(coder, coder->first_dongle);
			break ;
		}
		log_action(coder->sim, coder->id, "has taken a dongle");
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
		if (!is_running(coder->sim))
			break ;
		log_action(coder->sim, coder->id, "is debugging");
		precise_usleep(coder->sim->time_to_debug, coder->sim);
		if (!is_running(coder->sim))
			break ;
		log_action(coder->sim, coder->id, "is refactoring");
		precise_usleep(coder->sim->time_to_refactor, coder->sim);
	}
	return (NULL);
}