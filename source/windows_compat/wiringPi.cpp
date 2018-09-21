//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <chrono>
#include <thread>

#ifndef __linux__
int wiringPiSetupPhys(void) { return 0; }

void pinMode(int, int) {}
void pullUpDnControl(int, int) {}
int digitalRead(int) { return 0; }
void digitalWrite(int, int) {}

int wiringPiISR(int, int, void(*)(void)) { return 0; }

void pwmWrite(int, int) {}

void pwmSetMode(int) {}
void pwmSetRange(unsigned int) {}
void pwmSetClock(int) {}

void delay(int s) { std::this_thread::sleep_for(std::chrono::seconds(s)); }
void delayMicroseconds(int ms) { std::this_thread::sleep_for(std::chrono::microseconds(ms)); }

unsigned int micros(void) { return 1; }
#endif