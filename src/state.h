#include <Stdio.h>
#include <Arduino.h>
#ifndef STATE_H
#define STATE_H

// Habit name
String habitName = "Default Habit";

// Completion status
bool personA_done = false;
bool personB_done = false;

// Streak tracking (optional, not yet used in main.cpp)
int streakA = 0;
int streakB = 0;

// Button debounce flag
bool buttonPressed = false;

#endif