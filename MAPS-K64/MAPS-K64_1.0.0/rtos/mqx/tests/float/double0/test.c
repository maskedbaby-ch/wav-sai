/**HEADER********************************************************************
* 
* Copyright (c) 2008 Freescale Semiconductor;
* All Rights Reserved                       
*
* Copyright (c) 1989-2008 ARC International;
* All Rights Reserved
*
*************************************************************************** 
*
* THIS SOFTWARE IS PROVIDED BY FREESCALE "AS IS" AND ANY EXPRESSED OR 
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  
* IN NO EVENT SHALL FREESCALE OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
* THE POSSIBILITY OF SUCH DAMAGE.
*
**************************************************************************
*
* $FileName: test.c$
* $Version : 3.8.1.0$
* $Date    : Aug-10-2012$
*
* Comments:
*
*   This file contains the source functions for the demo of the
*   time slicing functions of the kernel.
*
*END************************************************************************/

#include <math.h>
#include <mqx.h>
#include <bsp.h>
#include "util.h"
#include "EUnit.h"
#include "eunit_crit_secs.h"

#define MAIN_TASK       10
#define FLOAT1_TASK     11
#define FLOAT2_TASK     12
#define DELAY_TASK      13
#define FILE_BASENAME  test


extern void main_task(uint32_t);
extern void double1_task(uint32_t index);
extern void double2_task(uint32_t index);
extern void delay_task(uint32_t index);
void tc_1_main_task(void);
void tc_1_double1_task(void);
void tc_1_double2_task(void);
void tc_1_delay_task(void);

/*volatile _mqx_uint _mqx_monitor_type = 3;*/

TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{

{ DELAY_TASK,  delay_task,   1750,  5, "delay",  MQX_AUTO_START_TASK},

{ FLOAT2_TASK,  double2_task, 3100,  6, "double2", MQX_AUTO_START_TASK | MQX_FLOATING_POINT_TASK},

{ MAIN_TASK,    main_task,   1800,  4, "Main",   MQX_AUTO_START_TASK | MQX_FLOATING_POINT_TASK},

{ FLOAT1_TASK,  double1_task,3100,  8, "double1", MQX_AUTO_START_TASK | MQX_FLOATING_POINT_TASK},

{ 0,                0,        0,    0,     0,        0 }
};



void force
   (
     uint32_t a, uint32_t b, uint32_t c, uint32_t d,
     uint32_t e, uint32_t f, uint32_t g, uint32_t h,
     uint32_t i, uint32_t j, uint32_t k, uint32_t l,
     uint32_t m, uint32_t n, uint32_t o, uint32_t p
   ) 
{/* Body */

   volatile uint32_t t = a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;

}/* Endbody */

void forced
   (
     double a, double b, double c, double d,
     double e, double f, double g, double h,
     double i, double j, double k, double l,
     double m, double n, double o, double p
   ) 
{/* Body */

   volatile double t = a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;

}/* Endbody */

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_main_task
* Comments     : TEST #1:
*
*END*---------------------------------------------------------------------*/
void tc_1_main_task(void)
{
     double angle;
   int count1, count2;
   bool error = FALSE;
   _mqx_uint old_prio;
   volatile uint32_t a,  b,  c,  d,  e,  f,  g,  h;
   register uint32_t a1, b1, c1, d1, e1, f1, g1, h1;
   volatile uint32_t i,  j,  k,  l,  m,  n,  o,  p;
   register uint32_t i1, j1, k1, l1, m1, n1, o1, p1;
   volatile uint32_t q,  r,  s,  t,  u,  v,  w,  x;
   register uint32_t q1, r1, s1, t1, u1, v1, w1, x1;
   volatile uint32_t y,  z,  A,  B,  C,  D,  E,  F;
   register uint32_t y1, z1, A1, B1, C1, D1, E1, F1;

   a1 = a = 0x101;
   b1 = b = 0x102;
   c1 = c = 0x103;
   d1 = d = 0x104;
   e1 = e = 0x105;
   f1 = f = 0x106;
   g1 = g = 0x107;
   h1 = h = 0x108;
   i1 = i = 0x109;
   j1 = j = 0x10A;
   k1 = k = 0x10B;
   l1 = l = 0x10C;
   m1 = m = 0x10D;
   n1 = n = 0x10E;
   o1 = o = 0x10F;
   p1 = p = 0x1010;
   q1 = q = 0x1011;
   r1 = r = 0x1012;
   s1 = s = 0x1013;
   t1 = t = 0x1014;
   u1 = u = 0x1015;
   v1 = v = 0x1016;
   w1 = w = 0x1017;
   x1 = x = 0x1018;
   y1 = y = 0x1019;
   z1 = z = 0x101A;
   A1 = A = 0x101B;
   B1 = B = 0x101C;
   C1 = C = 0x101D;
   D1 = D = 0x101E;
   E1 = E = 0x101F;
   F1 = F = 0x1020;

   _int_disable();   
   _int_install_unexpected_isr();
   count1 = 10;
   angle = 10.0;    // this is now a floating point task
   count2 = 0;
   _task_set_priority(0, 7, &old_prio);
   _int_enable();

   for (count1 = 0; count1 < 64; count1++ ) {
      _time_delay_ticks(40);
      if (++count2 == 10) {
         count2 = 0;
      } /* Endif */
      force(a1,b1,c1,d1,e1,f1,g1,h1,i1,j1,k1,l1,m1,n1,o1,p1);
      force(q1,r1,s1,t1,u1,v1,w1,x1,y1,z1,A1,B1,C1,D1,E1,F1);
      if (a1 != a) {
        EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.1: Test register value in main task")
         error = TRUE; break;
      }
      
      if (b1 != b) {
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.2: Test register value in main task")
         error = TRUE; break;
      }
      
      if (c1 != c) {
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.3: Test register value in main task")
         error = TRUE; break;
      }
      
      if (d1 != d) {
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.4: Test register value in main task")
         error = TRUE; break;
      }
      
      if (e1 != e) {
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.5: Test register value in main task")
         error = TRUE; break;
      }
      
      if (f1 != f) {
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.6: Test register value in main task")
         error = TRUE; break;
      }
      
      if (g1 != g) {
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.7: Test register value in main task")
         error = TRUE; break;
      }
      
      if (h1 != h) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.8: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (i1 != i) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.9: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (j1 != j) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.10: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (k1 != k) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.11: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (l1 != l) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.12: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (m1 != m) {                        
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.13: Test register value in main task")
         error = TRUE; break;
      }
      
      if (n1 != n) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.14: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (o1 != o) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.15: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (p1 != p) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.16: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (q1 != q) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.17: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (r1 != r) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.18: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (s1 != s) {                        
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.19: Test register value in main task")
         error = TRUE; break;
      }
      
      if (t1 != t) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.20: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (u1 != u) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.21: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (v1 != v) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.22: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (w1 != w) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.23: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (x1 != x) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.24: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (y1 != y) {                        
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.25: Test register value in main task")
         error = TRUE; break;
      }
      
      if (z1 != z) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.26: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (A1 != A) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.27: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (B1 != B) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.28: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (C1 != C) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.29: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (D1 != D) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.30: Test register value in main task")
         error = TRUE; break;                 
      }                                       
      
      if (E1 != E) {                        
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.31: Test register value in main task")
         error = TRUE; break;
      }
      
      if (F1 != F) {
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.32: Test register value in main task")
         error = TRUE; break;
      }
   } /* Endfor */

   if (error) {
      _int_disable();
      while (1){
      } /* Endwhile */
   }/* Endif */

}

volatile double w_val = 6.793;
volatile double z_val = 3.1459;

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_double1_task
* Comments     :
*
*END*---------------------------------------------------------------------*/
void tc_1_double1_task(void)
{
   register double test_w, test_x, test_y, test_z;
   volatile double a,  b,  c,  d,  e,  f,  g,  h;
   register double a1, b1, c1, d1, e1, f1, g1, h1;
   volatile double i,  j,  k,  l,  m,  n,  o,  p;
   register double i1, j1, k1, l1, m1, n1, o1, p1;
   volatile double q,  r,  s,  t,  u,  v,  w,  x;
   register double q1, r1, s1, t1, u1, v1, w1, x1;
   volatile double y,  z,  A,  B,  C,  D,  E,  F;
   register double y1, z1, A1, B1, C1, D1, E1, F1;

   a1 = a = 101.0;
   b1 = b = 102.0;
   c1 = c = 103.0;
   d1 = d = 104.0;
   e1 = e = 105.0;
   f1 = f = 106.0;
   g1 = g = 107.0;
   h1 = h = 108.0;
   i1 = i = 109.0;
   j1 = j = 1010.0;
   k1 = k = 1011.0;
   l1 = l = 1012.0;
   m1 = m = 1013.0;
   n1 = n = 1014.0;
   o1 = o = 1015.0;
   p1 = p = 1016.0;
   q1 = q = 1017.0;
   r1 = r = 1018.0;
   s1 = s = 1019.0;
   t1 = t = 1020.0;
   u1 = u = 1021.0;
   v1 = v = 1022.0;
   w1 = w = 1023.0;
   x1 = x = 1024.0;
   y1 = y = 1025.0;
   z1 = z = 1026.0;
   A1 = A = 1027.0;
   B1 = B = 1028.0;
   C1 = C = 1029.0;
   D1 = D = 1030.0;
   E1 = E = 1031.0;
   F1 = F = 1032.0;

   test_w = w_val;
   test_z = z_val;
   _task_stop_preemption();
   test_x = sin(test_w * test_z);  /* sin may not be re-entrant */
   _task_start_preemption();
   while (1) {
      _time_delay_ticks(5);
      forced(a1,b1,c1,d1,e1,f1,g1,h1,i1,j1,k1,l1,m1,n1,o1,p1);
      forced(q1,r1,s1,t1,u1,v1,w1,x1,y1,z1,A1,B1,C1,D1,E1,F1);
      _task_stop_preemption();
      test_y = sin(test_w * test_z);  /* Sin may not be re-entrant */
      _task_start_preemption();
      
      if (test_y != test_x) {
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.33:X should equal to Y")
         _int_disable();
         while (1){
         } /* Endwhile */
      } /* Endif */
      if (a1 != a) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.34: Test register value in double1 task")
         break;                               
      }                                       
      
      if (b1 != b) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.35: Test register value in double1 task")
         break;                               
      }                                       
      
      if (c1 != c) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.36: Test register value in double1 task")
         break;                               
      }                                       
      
      if (d1 != d) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.37: Test register value in double1 task")
         break;                               
      }                                       
      
      if (e1 != e) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.38: Test register value in double1 task")
         break;                               
      }                                       
      
      if (f1 != f) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.39: Test register value in double1 task")
         break;
      }
      
      if (g1 != g) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.40: Test register value in double1 task")
         break;                               
      }                                       
      
      if (h1 != h) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.41: Test register value in double1 task")
         break;                               
      }                                       
      
      if (i1 != i) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.42: Test register value in double1 task")
         break;                               
      }                                       
      
      if (j1 != j) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.43: Test register value in double1 task")
         break;                               
      }                                       
      
      if (k1 != k) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.44: Test register value in double1 task")
         break;                               
      }                                       
      
      if (l1 != l) {                        
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.45: Test register value in double1 task")
         break;
      }
      
      if (m1 != m) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.46: Test register value in double1 task")
         break;                               
      }                                       
      
      if (n1 != n) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.47: Test register value in double1 task")
         break;                               
      }                                       
      
      if (o1 != o) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.48: Test register value in double1 task")
         break;                               
      }                                       
      
      if (p1 != p) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.49: Test register value in double1 task")
         break;                               
      }                                       
      
      if (q1 != q) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.50: Test register value in double1 task")
         break;                               
      }                                       
      
      if (r1 != r) {                        
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.51: Test register value in double1 task")
         break;
      }
      
      if (s1 != s) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.52: Test register value in double1 task")
         break;                               
      }                                       
      
      if (t1 != t) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.53: Test register value in double1 task")
         break;                               
      }                                       
      
      if (u1 != u) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.54: Test register value in double1 task")
         break;                               
      }                                       
      
      if (v1 != v) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.55: Test register value in double1 task")
         break;                               
      }                                       
      
      if (w1 != w) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.56: Test register value in double1 task")
         break;                               
      }                                       
      
      if (x1 != x) {                        
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.57: Test register value in double1 task")
         break;
      }
      
      if (y1 != y) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.58: Test register value in double1 task")
         break;                               
      }                                       
      
      if (z1 != z) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.59: Test register value in double1 task")
         break;                               
      }                                       
      
      if (A1 != A) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.60: Test register value in double1 task")
         break;                               
      }                                       
      
      if (B1 != B) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.61: Test register value in double1 task")
         break;                               
      }                                       
      
      if (C1 != C) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.62: Test register value in double1 task")
         break;                               
      }                                       
      
      if (D1 != D) {                        
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.63: Test register value in double1 task")
         break;
      }
      
      if (E1 != E) {                          
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.64: Test register value in double1 task")
         break;                               
      }                                       
      
      if (F1 != F) {                        
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.65: Test register value in double1 task")
         break;
      }
   }/* Endwhile */
   _int_disable();
   while (1){
   } /* Endwhile */

}

volatile uint32_t count2;
volatile double  w2_val = 3.22491;
volatile double  z2_val = 1.44892;

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_double2_task
* Comments     :
*
*END*---------------------------------------------------------------------*/
void tc_1_double2_task(void)
{
    register double test_w, test_x, test_y, test_z;
   volatile double a,  b,  c,  d,  e,  f,  g,  h;
   register double a1, b1, c1, d1, e1, f1, g1, h1;
   volatile double i,  j,  k,  l,  m,  n,  o,  p;
   register double i1, j1, k1, l1, m1, n1, o1, p1;
   volatile double q,  r,  s,  t,  u,  v,  w,  x;
   register double q1, r1, s1, t1, u1, v1, w1, x1;
   volatile double y,  z,  A,  B,  C,  D,  E,  F;
   register double y1, z1, A1, B1, C1, D1, E1, F1;

   a1 = a = 1.0;
   b1 = b = 2.0;
   c1 = c = 3.0;
   d1 = d = 4.0;
   e1 = e = 5.0;
   f1 = f = 6.0;
   g1 = g = 7.0;
   h1 = h = 8.0;
   i1 = i = 9.0;
   j1 = j = 10.0;
   k1 = k = 11.0;
   l1 = l = 12.0;
   m1 = m = 13.0;
   n1 = n = 14.0;
   o1 = o = 15.0;
   p1 = p = 16.0;
   q1 = q = 17.0;
   r1 = r = 18.0;
   s1 = s = 19.0;
   t1 = t = 20.0;
   u1 = u = 21.0;
   v1 = v = 22.0;
   w1 = w = 23.0;
   x1 = x = 24.0;
   y1 = y = 25.0;
   z1 = z = 26.0;
   A1 = A = 27.0;
   B1 = B = 28.0;
   C1 = C = 29.0;
   D1 = D = 30.0;
   E1 = E = 31.0;
   F1 = F = 32.0;

   count2 = 0;
   test_w = w2_val;
   test_z = z2_val;
   _task_stop_preemption();
   test_x = sin(test_w * test_z); /* sin may not be re-entrant */
   _task_start_preemption();
   while (1) {
      _time_delay_ticks(10);
      count2++;
      forced(a1,b1,c1,d1,e1,f1,g1,h1,i1,j1,k1,l1,m1,n1,o1,p1);
      forced(q1,r1,s1,t1,u1,v1,w1,x1,y1,z1,A1,B1,C1,D1,E1,F1);
      _task_stop_preemption();
      test_y = sin(test_w * test_z);   /* sin may not be re-entrant */
      _task_start_preemption();
      
      if (test_y != test_x) {
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.66: X should equal to Y")
         _int_disable();
         while(1){
         }/* Endwhile */
      } /* Endif */
      
      if (a1 != a) {
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.67: Test register value in double2 task")
         break;
      }
      
      if (b1 != b) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.68: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (c1 != c) {                                                  
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.69: Test register value in double2 task")
         break;                               
      }                                       
      
      if (d1 != d) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.70: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (e1 != e) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.71: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (f1 != f) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.72: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (g1 != g) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.73: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (h1 != h) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.74: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (i1 != i) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.75: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (j1 != j) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.76: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (k1 != k) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.77: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (l1 != l) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.78: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (m1 != m) {                                                  
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.79: Test register value in double2 task")
         break;                               
      }                                       
      
      if (n1 != n) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.80: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (o1 != o) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.81: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (p1 != p) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.82: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (q1 != q) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.83: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (r1 != r) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.84: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (s1 != s) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.85: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (t1 != t) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.86: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (u1 != u) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.87: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (v1 != v) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.88: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (w1 != w) {                                                  
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.89: Test register value in double2 task")
         break;                               
      }                                       
      
      if (x1 != x) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.90: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (y1 != y) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.91: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (z1 != z) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.92: Test register value in double2 task")
         break;                                                         
      }                                                                
      
      if (A1 != A) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.93: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (B1 != B) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.94: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (C1 != C) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.95: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (D1 != D) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.96: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (E1 != E) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.97: Test register value in double2 task")
         break;                                                         
      }                                                                 
      
      if (F1 != F) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.98: Test register value in double2 task")
         break;                                                         
      }                                                                
   }/* Endwhile */         
   _int_disable();
   while (1){
   } /* Endwhile */

}

/*TASK*--------------------------------------------------------------------
* 
* Task Name    : tc_1_delay_task
* Comments     :
*
*END*---------------------------------------------------------------------*/
void tc_1_delay_task(void)
{
     volatile uint32_t a,  b,  c,  d,  e,  f,  g,  h;
   register uint32_t a1, b1, c1, d1, e1, f1, g1, h1;
   volatile uint32_t i,  j,  k,  l,  m,  n,  o,  p;
   register uint32_t i1, j1, k1, l1, m1, n1, o1, p1;
   volatile uint32_t q,  r,  s,  t,  u,  v,  w,  x;
   register uint32_t q1, r1, s1, t1, u1, v1, w1, x1;
   volatile uint32_t y,  z,  A,  B,  C,  D,  E,  F;
   register uint32_t y1, z1, A1, B1, C1, D1, E1, F1;

   a1 = a = 0x1;
   b1 = b = 0x2;
   c1 = c = 0x3;
   d1 = d = 0x4;
   e1 = e = 0x5;
   f1 = f = 0x6;
   g1 = g = 0x7;
   h1 = h = 0x8;
   i1 = i = 0x9;
   j1 = j = 0xA;
   k1 = k = 0xB;
   l1 = l = 0xC;
   m1 = m = 0xD;
   n1 = n = 0xE;
   o1 = o = 0xF;
   p1 = p = 0x10;
   q1 = q = 0x11;
   r1 = r = 0x12;
   s1 = s = 0x13;
   t1 = t = 0x14;
   u1 = u = 0x15;
   v1 = v = 0x16;
   w1 = w = 0x17;
   x1 = x = 0x18;
   y1 = y = 0x19;
   z1 = z = 0x1A;
   A1 = A = 0x1B;
   B1 = B = 0x1C;
   C1 = C = 0x1D;
   D1 = D = 0x1E;
   E1 = E = 0x1F;
   F1 = F = 0x20;

   while (TRUE) {
      _time_delay_ticks(20);
      force(a1,b1,c1,d1,e1,f1,g1,h1,i1,j1,k1,l1,m1,n1,o1,p1);
      force(q1,r1,s1,t1,u1,v1,w1,x1,y1,z1,A1,B1,C1,D1,E1,F1);
      
      if (a1 != a) {
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.99: Test register value in delay task")
         break;
      }
      
      if (b1 != b) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.100: Test register value in delay task")
         break;                                                         
      }                                                                
      
      if (c1 != c) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.101: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (d1 != d) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.102: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (e1 != e) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.103: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (f1 != f) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.104: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (g1 != g) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.105: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (h1 != h) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.106: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (i1 != i) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.107: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (j1 != j) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.108: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (k1 != k) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.109: Test register value in delay task")
         break;                               
      }                                       
      
      if (l1 != l) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.110: Test register value in delay task")
         break;                                                         
      }                                                                
      
      if (m1 != m) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.111: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (n1 != n) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.112: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (o1 != o) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.113: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (p1 != p) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.114: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (q1 != q) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.115: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (r1 != r) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.116: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (s1 != s) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.117: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (t1 != t) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.118: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (u1 != u) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.119: Test register value in delay task")
         break;                               
      }                                       
      
      if (v1 != v) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.120: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (w1 != w) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.121: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (x1 != x) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.122: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (y1 != y) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.123: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (z1 != z) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.124: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (A1 != A) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.125: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (B1 != B) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.126: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (C1 != C) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.127: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (D1 != D) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.128: Test register value in delay task")
         break;                                                         
      }                                                                 
      
      if (E1 != E) {                                                    
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.129: Test register value in delay task")
         break;                               
      }                                       
      
      if (F1 != F) {                        
         EU_ASSERT_REF_TC_MSG(tc_1_main_task, FALSE, "Test #1: Testing 1.130: Test register value in delay task")
         break;
      }
   }/* Endwhile */
   _int_disable();
   while (1){
   } /* Endwhile */
}
/*TASK*-------------------------------------------------------------------
* 
* Task Name    : 
* Comments     :
*
*END*----------------------------------------------------------------------*/

void delay_task
   (
      uint32_t index
   )
{/* Body */
    tc_1_delay_task();
}/* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : 
* Comments     :
*
*END*----------------------------------------------------------------------*/


void double1_task
   (
      uint32_t index
   )
{/* Body */
    tc_1_double1_task();
}/* Endbody */


/*TASK*-------------------------------------------------------------------
* 
* Task Name    : 
* Comments     :
*
*END*----------------------------------------------------------------------*/
void double2_task
   (
      uint32_t index
   )
{/* Body */
   tc_1_double2_task();
} /* Endbody */

//------------------------------------------------------------------------
// Define Test Suite
 EU_TEST_SUITE_BEGIN(suite_double0)
    EU_TEST_CASE_ADD(tc_1_main_task, " TEST #1: Test of Floating Point")
 EU_TEST_SUITE_END(suite_double0)
// Add test suites
 EU_TEST_REGISTRY_BEGIN()
        EU_TEST_SUITE_ADD(suite_double0, " Test of Floating Point")
 EU_TEST_REGISTRY_END()
//------------------------------------------------------------------------

/*TASK*-------------------------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*
*END*----------------------------------------------------------------------*/

void main_task
   (
      uint32_t parameter
   )
{ /* Body */
   eunit_mutex_init();
   EU_RUN_ALL_TESTS(eunit_tres_array);
   eunit_test_stop();
} /* Endbody */

/* EOF */

