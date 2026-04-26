// ============================================================
// IFAMDS — Intelligent Forest Advisory & Multi-Structure
//          Decision System
// File   : menu.h
// Purpose: Menu function declarations for the unified
//          console-based interface (menu.cpp implements these)
//
// Connects all three Partner A layers:
//   ArrayLayer      → Part 1 (arrays.h)
//   EventMemory     → Part 2 (linkedlist.h)
//   SchedulingEngine→ Part 3 (queues.h)
//
// Integration pipeline (anomaly path):
//   Part 1 flags anomaly → Part 2 logs to L3 → Part 3 adds to Q3
//
// Authors: Partner A
// Course : CL2001 — Data Structures 2026
// ============================================================

#ifndef MENU_H
#define MENU_H

#include "arrays.h"
#include "linkedlist.h"
#include "queues.h"

// ── Utility ─────────────────────────────────────────────────
void printBanner();
void printSeparator(char c = '=', int width = 56);
void clearScreen();
int  getIntInput(const std::string& prompt, int minVal, int maxVal);
float getFloatInput(const std::string& prompt);

// ── Main Menu ────────────────────────────────────────────────
void showMainMenu();
void runMenu(ArrayLayer& arr, EventMemory& em, SchedulingEngine& sched);

// ── Sub-Menu 1: Input Environmental Data ────────────────────
void menuInputData(ArrayLayer& arr, EventMemory& em, SchedulingEngine& sched);

// ── Sub-Menu 2: View Forest Grid Status ─────────────────────
void menuViewGrid(ArrayLayer& arr);

// ── Sub-Menu 3: Event Memory System ─────────────────────────
void menuEventMemory(EventMemory& em);

// ── Sub-Menu 4: Fire Detection & Control ────────────────────
void menuFireControl(ArrayLayer& arr, EventMemory& em, SchedulingEngine& sched);

// ── Sub-Menu 5: Task Scheduling System ──────────────────────
void menuScheduling(SchedulingEngine& sched);

// ── Integration: anomaly pipeline ───────────────────────────
// Called whenever Part 1 confirms an anomaly.
// Simultaneously logs to L3 (Part 2) and adds emergency task (Part 3).
void triggerAnomalyPipeline(SensorReading& r, EventMemory& em,
                             SchedulingEngine& sched, int eventTime);

#endif // MENU_H
