#include "stm32f0xx.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "data_processing.h"


//PA0 echo PC3 trigg `

// Глобальные переменные
uint8_t catcher_status = 0;     // Состояние ловушки: 0 - нарастающий фронт, 1 - спадающий
uint16_t duration = 0;          // Длительность последнего пойманного импульса




void Delay_ms(uint32_t ms)
{
  volatile uint32_t nCount;
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq (&RCC_Clocks);

  nCount = (RCC_Clocks.HCLK_Frequency/10000)*ms;
  for (; nCount!=0; nCount--);
}

void init_ports()
{
//GPIO_InitTypeDef PORT;
//  // Порт С, пин 3 - выход. Подключить к линии Trig модуля HC-SR04
//  // Порт A, пин 0 - вход. Подключить к линии Echo модуля HC-SR04
////  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
//  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;
//  PORT.GPIO_Pin = GPIO_Pin_3;
//  PORT.GPIO_Mode = GPIO_Mode_Out_PP;
//  PORT.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_Init(GPIOC, &PORT);
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN |RCC_AHBENR_GPIOBEN;
	GPIOC->MODER |= GPIO_MODER_MODER3_0;

}

void init_interrupts()
{
  //========================================================================
  //                          Настройка таймера 6
  // Используется для 2-х целей:
  // 1) Подсчёт длительности Echo импульса (150 мкс - 25 мс)
  // 2) Подсчёт с прерыванием для отчёта периода цикла - времени,
  // необходимого для затухания остаточных колебаний в линии Echo
  //========================================================================
  // Включаем тактирование таймера
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;

  // Выставляем предделитель на срабатывание раз в мкс
  TIM6->PSC = 24 - 1;
  // Граница срабатывания - 50 мс = 50 000 мкс
  TIM6->ARR = 50000;
  //Разрешение TIM6_IRQn прерывания - необходимо для отсчёта периода цикла
  NVIC_SetPriority(TIM6_DAC_IRQn, 3);
  NVIC_EnableIRQ(TIM6_DAC_IRQn);
  //========================================================================
  //                          Настройка таймера 7
  //========================================================================
  // Включаем тактирование таймера
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
  RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
  // Выставляем предделитель на срабатывание раз в мкс
  TIM7->PSC = 24 - 1;
  // Граница срабатывания - 10 мкс
  TIM7->ARR = 10;
  //Разрешение TIM7_IRQn прерывания - необходимо для отсчёта сигнального импульса
  NVIC_SetPriority(TIM7_IRQn, 2);
  NVIC_EnableIRQ(TIM7_IRQn);
  //========================================================================
  //                          Настройка внешнего прерывания
  // Внешние прерывания по умолчанию включены для всего порта A.
  // Здесь используется 0-й пин порта А.
  //========================================================================
  // Включаем Alternate function I/O clock
//  RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN , ENABLE);
  SYSCFG->EXTICR[0] &= (uint16_t)~SYSCFG_EXTICR1_EXTI0_PA;
  // Прерывания от нулевой ноги разрешены
  EXTI->IMR |= EXTI_IMR_MR0;
  // Прерывания по нарастающему фронту
  EXTI->RTSR |= EXTI_RTSR_TR0;
  //Разрешаем прерывание
  NVIC_SetPriority(EXTI0_1_IRQn, 1);
  NVIC_EnableIRQ (EXTI0_1_IRQn);
}

int main(void)
{
              // Настройка USART для вывода printf в COM
  init_ports();
  init_interrupts();
  USARTInit();
  Array distances;
  initArray(&distances, 150);



  // Запускаем таймер 7 первый раз на отсчёт 10 мс
  TIM7->DIER |= TIM_DIER_UIE;          // Разрешаем прерывание от таймера 7
  GPIOC->ODR |= GPIO_Pin_3;            // Включаем сигнальный импульс
  TIM7->CR1 |= TIM_CR1_CEN;
  // Запускаем таймер
  int i = 0;
  while(i != 60)
    {
      Delay_ms(500);
      printf("S = %d mm, %d\n", duration/29, duration);
      insertArray(&distances, duration / 29);
      i++;
    }
    for (int i = 0; i < 6; i++) {
    	printf("%d \n", distances.array[i]);
    }

    sendToAndroid(processData(distances));
}

// Обработчик прерывания EXTI0: изменение уровня сигнала
void EXTI0_1_IRQHandler(void)
{
  // Если поймали нарастающий фронт
  if (!catcher_status)
  {
    // Запускаем отсчёт длительности импульса
    TIM6->CR1 |= TIM_CR1_CEN;
    // Переключаемся на отлов спадающего фронта
    catcher_status = 1;
    EXTI->RTSR &= ~EXTI_RTSR_TR0;
    EXTI->FTSR |= EXTI_FTSR_TR0;
  }
  // Если поймали спадающий фронт
  else
  {
    TIM6->CR1 &= ~TIM_CR1_CEN;         // Останавливаем таймер
    duration = TIM6->CNT;              // Считываем значение длительности в мкс
    TIM6->CNT = 0;                     // Обнуляем регистр-счётчик
    // Переключаемся на отлов нарастающего фронта
    catcher_status = 0;
    EXTI->FTSR &= ~EXTI_FTSR_TR0;
    EXTI->RTSR |= EXTI_RTSR_TR0;
    // Запускаем таймер 6 на отсчёт 50 мс
    TIM6->DIER |= TIM_DIER_UIE;        // Разрешаем прерывание от таймера
    TIM6->CR1 |= TIM_CR1_CEN;          // Запускаем таймер
  }
  EXTI->PR |= 0x01;                    //Очищаем флаг
}

// Обработчик прерывания TIM7_DAC
// Вызывается после того, как таймер 7 отсчитал 10 мкс для сигнального импульса
void TIM7_IRQHandler(void)
{
  TIM7->SR &= ~TIM_SR_UIF;             // Сбрасываем флаг UIF
  GPIOC->ODR &= ~GPIO_Pin_3;           // Останавливаем сигнальный импульс
  TIM7->DIER &= ~TIM_DIER_UIE;         // Запрещаем прерывание от таймера 7
}

// Обработчик прерывания TIM6_DAC
// Вызывается после того, как таймер 6 отсчитал 50 мкс для периода цикла
void TIM6_DAC_IRQHandler(void)
{
  TIM6->SR &= ~TIM_SR_UIF;             // Сбрасываем флаг UIF
  GPIOC->ODR |= GPIO_Pin_3;            // Включаем сигнальный импульс
  // Запускаем таймер 7 на отсчёт 10 мс
  TIM7->DIER |= TIM_DIER_UIE;          // Разрешаем прерывание от таймера 7
  TIM7->CR1 |= TIM_CR1_CEN;            // Запускаем таймер
}


