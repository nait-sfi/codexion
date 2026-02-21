#include "codexion.h"

int main(int argc, char **argv)
{
    t_sim sim;
    memset(&sim, 0, sizeof(t_sim));
    if (parse_args(&sim, argc, argv))
        return (1);
    printf("Parsed OK: %d coders, burnout=%lld, scheduler=%s\n",
        sim.num_coders, sim.time_to_burnout,
        sim.scheduler == SCHED_FIFO ? "fifo" : "edf");
    return (0);
}