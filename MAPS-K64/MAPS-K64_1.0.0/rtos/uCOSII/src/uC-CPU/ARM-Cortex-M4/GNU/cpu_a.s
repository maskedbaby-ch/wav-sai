@********************************************************************************************************
@                                                uC/CPU
@                                    CPU CONFIGURATION & PORT LAYER
@
@                          (c) Copyright 2004-2013; Micrium, Inc.; Weston, FL
@
@               All rights reserved.  Protected by international copyright laws.
@
@               uC/CPU is provided in source form to registered licensees ONLY.  It is
@               illegal to distribute this source code to any third party unless you receive
@               written permission by an authorized Micrium representative.  Knowledge of
@               the source code may NOT be used to develop a similar product.
@
@               Please help us continue to provide the Embedded community with the finest
@               software available.  Your honesty is greatly appreciated.
@
@               You can find our product's user manual, API reference, release notes and
@               more information at https://doc.micrium.com.
@               You can contact us at www.micrium.com.
@********************************************************************************************************

@********************************************************************************************************
@
@                                            CPU PORT FILE
@
@                                            ARM-Cortex-M4
@                                            GNU C Compiler
@
@ Filename      : cpu_a.asm
@ Version       : V1.30.00.00
@ Programmer(s) : JJL
@********************************************************************************************************


@********************************************************************************************************
@                                           PUBLIC FUNCTIONS
@********************************************************************************************************

        .global  CPU_IntDis
        .global  CPU_IntEn

        .global  CPU_SR_Save
        .global  CPU_SR_Restore

        .global  CPU_WaitForInt
        .global  CPU_WaitForExcept


        .global  CPU_CntLeadZeros
        .global  CPU_CntTrailZeros
        .global  CPU_RevBits


@********************************************************************************************************
@                                      CODE GENERATION DIRECTIVES
@********************************************************************************************************

.text
.align 2
.syntax unified


@********************************************************************************************************
@                                    DISABLE and ENABLE INTERRUPTS
@
@ Description: Disable/Enable interrupts.
@
@ Prototypes : void  CPU_IntDis(void);
@              void  CPU_IntEn (void);
@********************************************************************************************************

.thumb_func
CPU_IntDis:
        CPSID   I
        BX      LR

.thumb_func
CPU_IntEn:
        CPSIE   I
        BX      LR


@********************************************************************************************************
@                                      CRITICAL SECTION FUNCTIONS
@
@ Description : Disable/Enable interrupts by preserving the state of interrupts.  Generally speaking, the
@               state of the interrupt disable flag is stored in the local variable 'cpu_sr' & interrupts
@               are then disabled ('cpu_sr' is allocated in all functions that need to disable interrupts).
@               The previous interrupt state is restored by copying 'cpu_sr' into the CPU's status register.
@
@ Prototypes  : CPU_SR  CPU_SR_Save   (void);
@               void    CPU_SR_Restore(CPU_SR  cpu_sr);
@
@ Note(s)     : (1) These functions are used in general like this :
@
@                       void  Task (void  *p_arg)
@                       {
@                           CPU_SR_ALLOC();                     /* Allocate storage for CPU status register */
@                               :
@                               :
@                           CPU_CRITICAL_ENTER();               /* cpu_sr = CPU_SR_Save();                  */
@                               :
@                               :
@                           CPU_CRITICAL_EXIT();                /* CPU_SR_Restore(cpu_sr);                  */
@                               :
@                       }
@********************************************************************************************************

.thumb_func
CPU_SR_Save:
        MRS     R0, PRIMASK                     @ Set prio int mask to mask all (except faults)
        CPSID   I
        BX      LR

.thumb_func
CPU_SR_Restore:                                  @ See Note #2.
        MSR     PRIMASK, R0
        BX      LR


@********************************************************************************************************
@                                         WAIT FOR INTERRUPT
@
@ Description : Enters sleep state, which will be exited when an interrupt is received.
@
@ Prototypes  : void  CPU_WaitForInt (void)
@
@ Argument(s) : none.
@********************************************************************************************************

.thumb_func
CPU_WaitForInt:
        WFI                                     @ Wait for interrupt
        BX      LR


@********************************************************************************************************
@                                         WAIT FOR EXCEPTION
@
@ Description : Enters sleep state, which will be exited when an exception is received.
@
@ Prototypes  : void  CPU_WaitForExcept (void)
@
@ Argument(s) : none.
@********************************************************************************************************

.thumb_func
CPU_WaitForExcept:
        WFE                                     @ Wait for exception
        BX      LR


@********************************************************************************************************
@                                         CPU_CntLeadZeros()
@                                        COUNT LEADING ZEROS
@
@ Description : Counts the number of contiguous, most-significant, leading zero bits before the
@                   first binary one bit in a data value.
@
@ Prototype   : CPU_DATA  CPU_CntLeadZeros(CPU_DATA  val);
@
@ Argument(s) : val         Data value to count leading zero bits.
@
@ Return(s)   : Number of contiguous, most-significant, leading zero bits in 'val'.
@
@ Caller(s)   : Application.
@
@               This function is an INTERNAL CPU module function but MAY be called by application
@               function(s).
@
@ Note(s)     : (1) (a) Supports 32-bit data value size as configured by 'CPU_DATA' (see 'cpu.h
@                       CPU WORD CONFIGURATION  Note #1').
@
@                   (b) For 32-bit values :
@
@                             b31  b30  b29  ...  b04  b03  b02  b01  b00    # Leading Zeros
@                             ---  ---  ---       ---  ---  ---  ---  ---    ---------------
@                              1    x    x         x    x    x    x    x            0
@                              0    1    x         x    x    x    x    x            1
@                              0    0    1         x    x    x    x    x            2
@                              :    :    :         :    :    :    :    :            :
@                              :    :    :         :    :    :    :    :            :
@                              0    0    0         1    x    x    x    x           27
@                              0    0    0         0    1    x    x    x           28
@                              0    0    0         0    0    1    x    x           29
@                              0    0    0         0    0    0    1    x           30
@                              0    0    0         0    0    0    0    1           31
@                              0    0    0         0    0    0    0    0           32
@
@
@               (2) MUST be defined in 'cpu_a.asm' (or 'cpu_c.c') if CPU_CFG_LEAD_ZEROS_ASM_PRESENT is
@                   #define'd in 'cpu_cfg.h' or 'cpu.h'.
@********************************************************************************************************

.thumb_func
CPU_CntLeadZeros:
        CLZ     R0, R0                          @ Count leading zeros
        BX      LR


@********************************************************************************************************
@                                         CPU_CntTrailZeros()
@                                        COUNT TRAILING ZEROS
@
@ Description : Counts the number of contiguous, least-significant, trailing zero bits before the
@                   first binary one bit in a data value.
@
@ Prototype   : CPU_DATA  CPU_CntTrailZeros(CPU_DATA  val);
@
@ Argument(s) : val         Data value to count trailing zero bits.
@
@ Return(s)   : Number of contiguous, least-significant, trailing zero bits in 'val'.
@
@ Caller(s)   : Application.
@
@               This function is an INTERNAL CPU module function but MAY be called by application
@               function(s).
@
@ Note(s)     : (1) (a) Supports 32-bit data value size as configured by 'CPU_DATA' (see 'cpu.h
@                       CPU WORD CONFIGURATION  Note #1').
@
@                   (b) For 32-bit values :
@
@                             b31  b30  b29  b28  b27  ...  b02  b01  b00    # Trailing Zeros
@                             ---  ---  ---  ---  ---       ---  ---  ---    ----------------
@                              x    x    x    x    x         x    x    1            0
@                              x    x    x    x    x         x    1    0            1
@                              x    x    x    x    x         1    0    0            2
@                              :    :    :    :    :         :    :    :            :
@                              :    :    :    :    :         :    :    :            :
@                              x    x    x    x    1         0    0    0           27
@                              x    x    x    1    0         0    0    0           28
@                              x    x    1    0    0         0    0    0           29
@                              x    1    0    0    0         0    0    0           30
@                              1    0    0    0    0         0    0    0           31
@                              0    0    0    0    0         0    0    0           32
@
@
@               (2) MUST be defined in 'cpu_a.asm' (or 'cpu_c.c') if CPU_CFG_TRAIL_ZEROS_ASM_PRESENT is
@                   #define'd in 'cpu_cfg.h' or 'cpu.h'.
@********************************************************************************************************

.thumb_func
CPU_CntTrailZeros:
        RBIT    R0, R0                          @ Reverse bits
        CLZ     R0, R0                          @ Count trailing zeros
        BX      LR


@********************************************************************************************************
@                                            CPU_RevBits()
@                                            REVERSE BITS
@
@ Description : Reverses the bits in a data value.
@
@ Prototypes  : CPU_DATA  CPU_RevBits(CPU_DATA  val);
@
@ Argument(s) : val         Data value to reverse bits.
@
@ Return(s)   : Value with all bits in 'val' reversed (see Note #1).
@
@ Caller(s)   : Application.
@
@               This function is an INTERNAL CPU module function but MAY be called by application function(s).
@
@ Note(s)     : (1) The final, reversed data value for 'val' is such that :
@
@                       'val's final bit  0       =  'val's original bit  N
@                       'val's final bit  1       =  'val's original bit (N - 1)
@                       'val's final bit  2       =  'val's original bit (N - 2)
@
@                               ...                           ...
@
@                       'val's final bit (N - 2)  =  'val's original bit  2
@                       'val's final bit (N - 1)  =  'val's original bit  1
@                       'val's final bit  N       =  'val's original bit  0
@********************************************************************************************************

.thumb_func
CPU_RevBits:
        RBIT    R0, R0                          @ Reverse bits
        BX      LR


@********************************************************************************************************
@                                     CPU ASSEMBLY PORT FILE END
@********************************************************************************************************

.end

