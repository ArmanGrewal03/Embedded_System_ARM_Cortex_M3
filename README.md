# Embedded Systems Projects

This repository contains a collection of embedded systems projects developed using the ARM Cortex-M3 microcontroller and the RTX real-time operating system.  
Each project demonstrates key concepts in embedded software design, from low-level hardware control to multitasking and real-time scheduling.

---

## Technologies and Tools

- Microcontroller: NXP LPC1768 (ARM Cortex-M3)  
- IDE: Keil µVision5  
- RTOS: Keil RTX (CMSIS-RTOS API)  
- Language: C / Embedded C  
- Concepts: GPIO, ADC, Bit-Banding, Conditional Execution, Barrel Shifting, RTOS Thread Scheduling

---

## Repository Structure

**cortex-basics**  
Fundamental ARM Cortex-M3 programming with GPIO, ADC, and LCD integration.

**cortex-performance-optimization**  
Implements bit banding, conditional execution, and barrel shifting to improve execution efficiency.

**rtx-multitasking-round-robin**  
Demonstrates multitasking using the RTX RTOS with round-robin scheduling and thread management.

**os-based-round-robin-scheduling**  
Implements an operating-system-style workflow using round-robin scheduling between multiple tasks.

**priority-based-preemptive-os-scheduling**  
Shows thread prioritization and preemptive scheduling using RTX with performance analysis tools.

---

## Learning Outcomes

- Designed and executed real-time multitasking applications using round-robin and preemptive scheduling.  
- Implemented efficient bit-level operations such as bit-banding for direct hardware manipulation.  
- Analyzed CPU utilization and thread execution timing using Keil µVision debugging tools.  
- Integrated and controlled hardware peripherals including LEDs, ADC, and LCD modules.

---

## Author

**Arman Grewal**  
Embedded Systems Developer  
Focused on real-time software design, low-level hardware integration, and performance optimization.
