# RTX Multitasking – Three Task Round-Robin Scheduling

This project demonstrates multitasking on the ARM Cortex-M3 using the RTX real-time operating system.  
Three independent threads are created and executed using a round-robin scheduling approach, each sharing equal CPU time slices.

[View Full Report (PDF)](./report3.pdf)

## Overview
The project explores the concept of cooperative multitasking through equal-time-slice scheduling in an embedded environment.  
Each thread performs a distinct function and is executed sequentially based on the RTX round-robin mechanism.

## Key Topics
- Thread creation using CMSIS-RTOS APIs  
- Round-robin scheduling configuration in RTX  
- System tick timer and time-slice management  
- Event Viewer and Performance Analyzer usage

## Learning Highlights
- Implemented three concurrent threads with shared CPU time  
- Observed thread switching behavior using Keil µVision’s Event Viewer  
- Compared execution timing and performance between threads  
- Gained practical experience with the RTX kernel and real-time thread management

## Tools and Environment
- Microcontroller: NXP LPC1768 (ARM Cortex-M3)  
- IDE: Keil µVision5  
- RTOS: Keil RTX (CMSIS-RTOS API)  
- Language: C / Embedded C
