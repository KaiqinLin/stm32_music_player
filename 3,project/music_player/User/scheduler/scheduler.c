#include "./scheduler/scheduler.h"
#include "stm32f4xx.h"
#include "./timer/bsp_timer.h"
#include "common.h"
#include "stdio.h"


/*
 * \brief    Scheduler Class Implementation
 *
 */
typedef struct _scheduler{
  fp_get_micros          get_micros;         /* get time in micros */  
  fp_innerloop           inloop;             /* inner loop */
  fp_sched_run           run;                /* scheduling algorithm */
  fp_count               count;              /* inner loop runing count */  
  uint16_t               _loop_rate;         /* inner loop rate */
  uint32_t               _loop_count;        /* inner loop counter */
  uint8_t                _run_flag;          /* inner loop run enable flag */
  uint32_t               _ticks;             /* timer ticks */
  uint8_t                _task_num;          /* task numbers according to user defined task array */
  const task_t          *_array;             /* user defined task array */
}sched_t;

sched_t g_sched;                /* create the scheduler */


const uint8_t *heartbeat1 = " ------- heartbeat ";
const uint8_t *heartbeat2 = " ------- \r\n";
static uint32_t beats = 0;

/*
 * forward declaration
 */
uint32_t              default_get_micros (void);
void                  default_innerloop(sched_t *thiz, void* ctx);
void                  default_run(sched_t *thiz, uint32_t time_available);
static void           count(sched_t *thiz);
static inline int32_t _get_loop_period_us(sched_t *thiz);  
static inline int32_t _get_loop_period_ms(sched_t *thiz);

/**
 * \brief  default scheduler constructor 
 *
 * \note   user can override this function to initialize their own inner loop
 *      and other scheduler properties.
 */
extern uint32_t default_get_micros (void);
void default_innerloop(sched_t *thiz, void* ctx);

void default_init(sched_t *thiz, const task_t *task_array, uint8_t task_num)
{
  thiz->get_micros    = default_get_micros;       
  thiz->inloop        = default_innerloop;
  thiz->run           = default_run;
  thiz->count         = count;
  thiz->_loop_rate    = 100;
  thiz->_loop_count   = 0; 
  thiz->_run_flag     = 0; 
  thiz->_ticks        = 0; 
  thiz->_task_num     = task_num;
  thiz->_array        = task_array;
}


/*
 * \brief  The main scheduling algorithm default implementation
 * 
 * \param[in]  thiz: this pointer points to the scheduler itself
 * \param[in]  time_available: remaining time after running the inner_loop
 *
 * \note       user can override this function by providing their own scheduling algorithm implementations. 
 * 
 */
void default_run (sched_t *thiz, uint32_t time_available)
{
  uint32_t task_time_allowed   = 0;
  uint32_t task_time_started   = 0;
  uint32_t task_time_taken  = 0;
  uint8_t  current_task     = 0;
  task_t  *cur_task         = NULL;
  
  uint32_t now = thiz->get_micros();
  uint8_t i = 0;
  
  for (i = 0; i < thiz->_task_num; i++) {
    current_task = i;
    cur_task     = (task_t *)(thiz->_array + current_task);
    
    uint16_t dt             = thiz->_loop_count - cur_task->_last_ticks;
    uint16_t interval_ticks = thiz->_loop_rate / cur_task->_rate_hz;
    
    if (interval_ticks < 1) {
      interval_ticks = 1;
    }
  
      if (dt >= interval_ticks) {
      // this task is due to run. Do we have enough time to run it?
      task_time_allowed = cur_task->_max_time;

    /*  if (dt >= interval_ticks * 2) {
          sdebug(DEBUG_FD,
                 W,
           "[W] %s: we've slipped a whole run of task %s!\r\n",
           __func__,
           cur_task->_name);
      }
  */    if (task_time_allowed <= time_available) {
        // run it
        task_time_started = now;
        cur_task->run(cur_task, cur_task->_ctx);

        // record the tick counter when we ran. This drives
        // when we next run the event
        cur_task->_last_ticks = thiz->_loop_count;
        
        // work out how long the event actually took
        now = thiz->get_micros();
        task_time_taken = now - task_time_started;

        if (task_time_taken > task_time_allowed) {
//          sdebug(DEBUG_FD, 
//                 W,
//                 "[W] %s: task %s overran! \r\n\
//                 %u allowed but %u actually consumed.\r\n",
//                 __func__,
//                 cur_task->_name,
//                 task_time_allowed,
//                 task_time_taken);
        }
        if (task_time_taken >= time_available) {
          thiz->_run_flag = 1;
          return;
        }
        time_available -= task_time_taken;
      }
    }
  }
    
}
/*
 * \brief timer interface provided by scheduler to other modules 
 *
 */
uint32_t sched_get_micros (sched_t* thiz)
{
  return thiz->get_micros();
}

uint32_t default_get_micros (void)
{
  return micros();
}

/**
 * \brief  inner_loop function which should be placed into main loop
 *
 * \note   this is a default implementation of inner loop
 */
void default_innerloop (sched_t *thiz, void* ctx)
{
  static uint16_t i = 0;
//  watchdog_toggle();
  ctx = NULL; // supress compiler warning, will be optimized out
  if (i >= 100) {
    debug(            "%s %d %s", 
                     heartbeat1, 
                     beats++, heartbeat2);
    i = 0;
  } else {
    i++;
  }
}

/**
 * \brief  main_loop function which should be placed into while(1)
 *
 */
void main_loop (sched_t *thiz)
{
  _RETURN_IF_FAIL(thiz);
  uint32_t start_time       = 0;
  uint32_t end_time         = 0;
  uint32_t time_available    = 0;

  if (thiz->_run_flag == 1) {                      /* Wait for the 10ms mark bit */
    thiz->_run_flag = 0;

    start_time = thiz->get_micros();
    thiz->inloop(thiz, NULL);                    /* execute the inner loop */
    end_time = thiz->get_micros();  

    thiz->count(thiz);                           /* inner loop counts +1 */
    
    time_available = ( _get_loop_period_us(thiz) + start_time ) - end_time;    /* calculate the remaining time */
    
//        if (time_available > 500000) {
//            slog(DEBUG_FD, "******time out*******\r\n");
//        }
        
    thiz->run(thiz, time_available);                  /* start scheduling with time available */
  }
}

/**
 * \brief set the method scheduler uses to get time values (in micros )
 *
 */
void set_get_micros(sched_t *thiz, fp_get_micros fp)
{
  _RETURN_IF_FAIL(thiz);
  thiz->get_micros = fp;  
}

/**
 * \brief set the inner loop of scheduler
 */
void set_inner_loop(sched_t *thiz, fp_innerloop fp)
{
  _RETURN_IF_FAIL(thiz);
  thiz->inloop = fp;  
}

/**
 * \brief  timer callback, used to decide if it is time for inner loop to run
 *
 * \note   this function should be called from a 1ms timer's ISR
 *
 */
void timer_tick_callback (sched_t *thiz)
{
  _RETURN_IF_FAIL(thiz);
  if (( thiz->_ticks++ % _get_loop_period_ms(thiz)) == 0) {
    thiz->_run_flag = 1;
  }
}

/**
 * \brief default scheduler destructor
 *
 */
void default_destroy(sched_t *thiz)
{
  _RETURN_IF_FAIL(thiz);
  thiz->get_micros = NULL;
  thiz->inloop = NULL;
  thiz->run = NULL;
  thiz->_loop_rate = 0;
  thiz->_loop_count = 0;
  thiz->_run_flag = 0;
  thiz->_ticks = 0;
  thiz->_task_num = 0;
  thiz->_array = NULL;
  _SAFE_FREE(thiz);
}

/**
 * \brief  inner_loop running count
 *
 */
static void count(sched_t *thiz)
{
  thiz->_loop_count++;
}

/**
 * \brief  get inner loop running rate
 *
 * \retval  inner loop running rate
 */
static inline int16_t _get_loop_rate_hz(sched_t *thiz) 
{
  _RETURN_VAL_IF_FAIL(thiz, -1);
  return thiz->_loop_rate;
}

/**
 * \brief  get Inner Loop period in micro seconds 
 *
 * \param[in]  thiz:this pointer
 *
 * \retval  Inner Loop period in micro seconds
 */
static inline int32_t _get_loop_period_us(sched_t *thiz) 
{
  _RETURN_VAL_IF_FAIL(thiz,-1);
  // TODO: better idea to avoid integer division?
  return (int32_t)1000000UL / thiz->_loop_rate;
}

/**
 * \brief  get Inner Loop period in milli seconds 
 *
 * \param[in]  thiz:this pointer
 *
 * \retval  Inner Loop period in milli seconds
 */
static inline int32_t _get_loop_period_ms(sched_t *thiz) 
{
  _RETURN_VAL_IF_FAIL(thiz, -1);
  // TODO: better idea to avoid integer division?
  return (int32_t)1000UL / thiz->_loop_rate;
}
