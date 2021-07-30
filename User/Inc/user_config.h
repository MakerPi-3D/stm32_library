#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#if defined(USE_BOOT)
  #define ENABLE_UART1
  #define ENABLE_USB_HOST
#elif defined(USE_APP1)
  #define ENABLE_UART1
  #define ENABLE_SDRAM
  #define ENABLE_USB_HOST
#elif defined(USE_APP2)
  #define ENABLE_FREERTOS
  #define ENABLE_SDRAM
  #define ENABLE_USB_HOST
  #define ENABLE_UART1
  #ifdef ENABLE_UART1
    #define ENABLE_UART1_DMA
  #endif

  /**
  * S-Curve Acceleration
  *
  * This option eliminates vibration during printing by fitting a Bézier
  * curve to move acceleration, producing much smoother direction changes.
  *
  * See https://github.com/synthetos/TinyG/wiki/Jerk-Controlled-Motion-Explained
  */
  //#define S_CURVE_ACCELERATION
  //#define CPU_32_BIT
#endif

/**
 * The method of calculating these cycle-constants is unclear.
 * Most of them are no longer used directly for pulse timing, and exist
 * only to estimate a maximum step rate based on the user's configuration.
 * As 32-bit processors continue to diverge, maintaining cycle counts
 * will become increasingly difficult and error-prone.
 */
#ifdef CPU_32_BIT
  /**
  * Duration of START_TIMED_PULSE
  *
  * ...as measured on an LPC1768 with a scope and converted to cycles.
  * Not applicable to other 32-bit processors, but as long as others
  * take longer, pulses will be longer. For example the SKR Pro
  * (stm32f407zgt6) requires ~60 cyles.
  */
  #define TIMER_READ_ADD_AND_STORE_CYCLES 34UL

  // The base ISR takes 792 cycles
  #define ISR_BASE_CYCLES  792UL

  // Linear advance base time is 64 cycles
  #if defined(LIN_ADVANCE)
    #define ISR_LA_BASE_CYCLES 64UL
  #else
    #define ISR_LA_BASE_CYCLES 0UL
  #endif

  // S curve interpolation adds 40 cycles
  #if defined(S_CURVE_ACCELERATION)
    #define ISR_S_CURVE_CYCLES 40UL
  #else
    #define ISR_S_CURVE_CYCLES 0UL
  #endif

  // Stepper Loop base cycles
  #define ISR_LOOP_BASE_CYCLES 4UL

  // To start the step pulse, in the worst case takes
  #define ISR_START_STEPPER_CYCLES 13UL

  // And each stepper (start + stop pulse) takes in worst case
  #define ISR_STEPPER_CYCLES 16UL

#else
  // Cycles to perform actions in START_TIMED_PULSE
  #define TIMER_READ_ADD_AND_STORE_CYCLES 13UL

  // The base ISR takes 752 cycles
  #define ISR_BASE_CYCLES  752UL

  // Linear advance base time is 32 cycles
  #if defined(LIN_ADVANCE)
    #define ISR_LA_BASE_CYCLES 32UL
  #else
    #define ISR_LA_BASE_CYCLES 0UL
  #endif

  // S curve interpolation adds 160 cycles
  #if defined(S_CURVE_ACCELERATION)
    #define ISR_S_CURVE_CYCLES 160UL
  #else
    #define ISR_S_CURVE_CYCLES 0UL
  #endif

  // Stepper Loop base cycles
  #define ISR_LOOP_BASE_CYCLES 32UL

  // To start the step pulse, in the worst case takes
  #define ISR_START_STEPPER_CYCLES 57UL

  // And each stepper (start + stop pulse) takes in worst case
  #define ISR_STEPPER_CYCLES 88UL

#endif

#endif /* USER_CONFIG_H */
