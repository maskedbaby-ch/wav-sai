/**HEADER********************************************************************
*
* Copyright (c) 2013 Freescale Semiconductor;
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
* $FileName: test.h$
* $Version : 4.0.2$
* $Date    : Aug-12-2013$
*
* Comments:
*
*   This file contains definitions, externs and structs for
*   test suite suite_sem2 of Message testing module.
*
*END************************************************************************/

#define MAIN_TASK       (10)
#define OWNER_TASK      (11)
#define WAIT_TASK       (12)
#define PRIO1_TASK      (13)
#define PRIO2_TASK      (14)
#define MAIN_PRIO       (9)
#define SEM_NAME        ("sem")
#define SEM_INDEX       (0)
#define DELAY_MS        (500)
#define DELAY_TICKS     (100)
#define INVALID_VALUE   (1)

/* Main task prototype */
void main_task(uint32_t);
/* Owner task prototype */
void owner_task(uint32_t);
/* Wait task prototype */
void wait_task(uint32_t);
/* Prio task prototype */
void prio_task(uint32_t);
