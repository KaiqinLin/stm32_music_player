#include "./timer/bsp_timer.h"
#include "./systick/bsp_SysTick.h"


extern __IO int32_t OS_TimeMS;
/** TIM Interrput process func **/
void (*TIMx_ProcessIRQSrc)(void);

/* Global variables */
uint32_t g_time_val = 0;


 /**
  * @brief  通用定时器 TIMx,x[1,8]中断优先级配置
  * @param  无
  * @retval 无
  */
static void TIMx_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_IRQn;   
    // 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   
    // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * 注意：TIM_TimeBaseInitTypeDef结构体里面有5个成员，TIM6和TIM7的寄存器里面只有
 * TIM_Prescaler和TIM_Period，所以使用TIM6和TIM7的时候只需初始化这两个成员即可，
 * 另外三个成员是通用定时器和高级定时器才有.
 *-----------------------------------------------------------------------------
 * TIM_Prescaler         都有
 * TIM_CounterMode       TIMx,x[6,7]没有，其他都有（基本定时器）
 * TIM_Period            都有
 * TIM_ClockDivision     TIMx,x[6,7]没有，其他都有(基本定时器)
 * TIM_RepetitionCounter TIMx,x[1,8]才有(高级定时器)
 *-----------------------------------------------------------------------------
 */
static void TIM_Mode_Config(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  // 开启TIMx_CLK,x[6,7] 
  RCC_APB1PeriphClockCmd(GENERAL_TIM_CLK, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/
  //当定时器从0计数到4999，即为5000次，为一个定时周期
  TIM_TimeBaseStructure.TIM_Period = 1000-1;
  
  // 通用控制定时器时钟源TIMxCLK = HCLK/2=84MHz 
  // 设定定时器频率为=TIMxCLK/(TIM_Prescaler+1)=100000Hz
  TIM_TimeBaseStructure.TIM_Prescaler = 840-1;
  // 采样时钟分频
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  
  // 初始化定时器TIMx, x[1,8]
  TIM_TimeBaseInit(GENERAL_TIM, &TIM_TimeBaseStructure);
  
  
  // 清除定时器更新中断标志位
  TIM_ClearFlag(GENERAL_TIM, TIM_FLAG_Update);
  
  // 开启定时器更新中断
  TIM_ITConfig(GENERAL_TIM,TIM_IT_Update,ENABLE);
  
  // 使能定时器
  TIM_Cmd(GENERAL_TIM, ENABLE);  
}

/**
  * @brief  初始化高级控制定时器定时，10ms产生一次中断
  * @param  无
  * @retval 无
  */
void TIMx_Configuration(void(*cb)(void))
{
  TIMx_NVIC_Configuration();  
  
  TIM_Mode_Config();

  TIMx_ProcessIRQSrc = cb;
}

void  GENERAL_TIM_IRQHandler (void)
{
  if ( TIM_GetITStatus( GENERAL_TIM, TIM_IT_Update) != RESET ) 
  {
    TIM_ClearITPendingBit(GENERAL_TIM , TIM_IT_Update);
    (*TIMx_ProcessIRQSrc)();
  }
}

/**
 * @brief: TIM 6 NVIC config
 */
static void TIM6_NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure; 
    // 设置中断来源
    NVIC_InitStructure.NVIC_IRQChannel = SCHE_TIM_IRQn;   
    // 设置抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   
    // 设置子优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

static void TIM6_Mode_Config()
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  RCC_APB1PeriphClockCmd(SCHE_TIM_CLK, ENABLE); 

  /* 累计 TIM_Period个后产生一个更新或者中断*/
  TIM_TimeBaseStructure.TIM_Period = 1000-1;
  
  // 通用控制定时器时钟源TIMxCLK = HCLK/2=84MHz 
  TIM_TimeBaseStructure.TIM_Prescaler = 84-1;
  // 采样时钟分频
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
  // 计数方式
  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
  
  // 初始化定时器TIMx, x[1,8]
  TIM_TimeBaseInit(SCHE_TIM, &TIM_TimeBaseStructure);
  
  // 清除定时器更新中断标志位
  TIM_ClearFlag(SCHE_TIM, TIM_FLAG_Update);
  
  // 开启定时器更新中断
  TIM_ITConfig(SCHE_TIM,TIM_IT_Update,ENABLE);
  
  // 使能定时器
  TIM_Cmd(SCHE_TIM, ENABLE);  
}
/**
 * @brief: Config the tim6 for the time base to scheduler.
 */
void TIM6_Configuration(void)
{
  TIM6_NVIC_Configuration();
  TIM6_Mode_Config();
}

/**
 * \brief get the timer counter value (millisecond)
 */
uint32_t get_timer_counter (void)
{
  return TIM6->CNT;
}

/**
 * \brief get the program run time (microsecond)
 */
uint32_t micros (void)
{
  return g_time_val * 1000 + get_timer_counter();
}

uint32_t millis (void)      
{
  return g_time_val;
}
/* TIM Interrupt process */
void SCHE_TIM_IRQHandler(void)
{
  if ( TIM_GetITStatus( SCHE_TIM, TIM_IT_Update) != RESET ) 
  {
    /* GUI time base */
    OS_TimeMS ++;
    TIM_ClearITPendingBit(SCHE_TIM , TIM_IT_Update);
    g_time_val ++;
    timer_tick_callback(&g_sched);
  }
}

