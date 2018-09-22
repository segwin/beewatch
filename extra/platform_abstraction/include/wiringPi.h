//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <chrono>
#include <thread>

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

static inline int wiringPiSetupPhys(void) { return 0; }

static inline void pinMode(int, int) {}
static inline void pullUpDnControl(int, int) {}
static inline int digitalRead(int) { return 0; }
static inline void digitalWrite(int, int) {}

static inline int wiringPiISR(int, int, void(*)(void)) { return 0; }

static inline void pwmWrite(int, int) {}

static inline void pwmSetMode(int) {}
static inline void pwmSetRange(unsigned int) {}
static inline void pwmSetClock(int) {}

static inline void delay(int s) { std::this_thread::sleep_for(std::chrono::seconds(s)); }
static inline void delayMicroseconds(int ms) { std::this_thread::sleep_for(std::chrono::microseconds(ms)); }

static inline unsigned int micros(void) { return 1; }
#endif