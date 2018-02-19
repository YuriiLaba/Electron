#include "motor_control.h"
void initMotor() {
	GPIOC->MODER |= GPIO_MODER_MODER10_0;
	GPIOC->MODER |= GPIO_MODER_MODER11_0;
}

void turnOnMotorDown() {
	GPIOC->ODR |= GPIO_ODR_10;
	GPIOC->ODR &= ~GPIO_ODR_11;
}

void turnOnMotorUp() {
	GPIOC->ODR &= ~GPIO_ODR_10;
	GPIOC->ODR |= ~GPIO_ODR_11;
}

void turnoffMotor() {
	GPIOC->ODR &= ~GPIO_ODR_10;
	GPIOC->ODR &= ~GPIO_ODR_11;
}
