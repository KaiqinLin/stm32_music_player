/*
 * \module  inner loop
 *
 * \brief  inner loop executes non-schedulable parts of a controller program.
 *         for iridium base board, all necessary checks and pre-processing 
 *         procedures prior to running tasks should be placed here.
 *         different inner loops could be implemented here for running in 
 *         different system modes.
 */


#include "./scheduler/scheduler.h"


void innerloop_normal(sched_t *thiz, void* ctx)
{
    /* TODO: implement normal mode inner loop */
}
 
void innerloop_fixing(sched_t *thiz, void* ctx)
{
    /* TODO: implement fixing mode inner loop */
}
 
