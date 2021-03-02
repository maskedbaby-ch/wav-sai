/**HEADER********************************************************************
* Copyright (c) 2008-2013 Freescale Semiconductor;
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
* $FileName: test.h$
* $Version : 4.0.1.2$
* $Date    : Apr-22-2013$
*
* Comments:
*
*   This file contains definitions, extern and classes for
*   cplus test program.
*
*END************************************************************************/
#ifndef __CPLUS_H_
#define __CPLUS_H_

#include <mqx.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAIN_TASK       (10)
#define DELAY_TICKS     (50)
#define INIT_WIDTH      (50)
#define INIT_HEIGHT     (100)
#define DEFAULT_WIDTH   (10)
#define DEFAULT_HEIGHT  (10)
#define NEW_WIDTH       (100)
#define MAX_VALUE       (100000000)
#define INVALID_VALUE   (1)

/* Base class */
class Polygon
{
    protected:
        _mqx_uint height;
        _mqx_uint width;
    public:
       void Init(_mqx_uint, _mqx_uint);
       virtual _mqx_uint Area(void);
};

/* Derived Class */
class Rectangle : public Polygon
{
    public:
        Rectangle(void);
        Rectangle(_mqx_uint, _mqx_uint);
        virtual ~Rectangle(void);
    public:
        virtual _mqx_uint Area(void);
        void SetWidth(_mqx_uint);
        void TestApi(_mqx_uint);
        void TestPerf(_mqx_uint);
};

/* Main task prototype */
extern void main_task(uint32_t);
/* Get the valid field of a memory block */
_mqx_uint get_mem_valid_field(void *);
/* Set valid field of a memory block */
void set_mem_valid_field(void *, _mqx_uint);
/* Loop to test performance */
void loop_test(_mqx_uint);
/* Change the attributes of class */
void change_attributes(Rectangle rect1, Rectangle *rect2, Rectangle &rect3);

#ifdef __cplusplus
}
#endif

#endif

