//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#ifndef __linux__
#define INPUT            0
#define OUTPUT           1
#define PWM_OUTPUT       2
#define GPIO_CLOCK       3
#define SOFT_PWM_OUTPUT      4
#define SOFT_TONE_OUTPUT     5
#define PWM_TONE_OUTPUT      6

#define LOW          0
#define HIGH             1

// Pull up/down/none
#define PUD_OFF          0
#define PUD_DOWN         1
#define PUD_UP           2

// PWM
#define PWM_MODE_MS     0
#define PWM_MODE_BAL        1

// Interrupt levels
#define INT_EDGE_SETUP      0
#define INT_EDGE_FALLING    1
#define INT_EDGE_RISING     2
#define INT_EDGE_BOTH       3

int wiringPiSetupPhys(void);

void pinMode(int, int);
void pullUpDnControl(int, int);
int digitalRead(int);
void digitalWrite(int, int);

int wiringPiISR(int, int, void(*)(void));

void pwmWrite(int, int);

void pwmSetMode(int);
void pwmSetRange(unsigned int);
void pwmSetClock(int);

void delay(int s);
void delayMicroseconds(int ms);

unsigned int micros(void);
#endif