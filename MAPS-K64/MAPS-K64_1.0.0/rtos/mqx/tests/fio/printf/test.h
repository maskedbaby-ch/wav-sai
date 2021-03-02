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
* $FileName: test.h$
* $Version : 3.8.2.0$
* $Date    : Apr-17-2012$
*
* Comments:
*
*   This include file is used to define configuration
*
*END************************************************************************/
#ifndef TEST_H
#define TEST_H

#define BUF_LENGTH  50      /* Size of temporary buffers used in testcases */

#define NR_CNT      3       /* Count of items in number array */
#define NR_FCNT     4       /* Count of items in fnumber array */
#define NR_FSCNT    4       /* Count of items in fspec array */

#define NR_FLAGS    6       /* Count of items in flag array */

#define NR_SPECS    6       /* Count of items in specifiers array */
#define NR_FSPECS   6       /* Count of items in fspecifiers array */
    
#define NR_PREC_SUFF    3   /* Count of items in prec_prefix array */
#define NR_PREC_PREF    3   /* Count of items in prec_suffix array */

#define TEST_FLOATING_NRS   1   /* Enable floating test */
#define TEST_INTEGER_NRS    1   /* Enable integer test */


/* Numbers for integer testing */
int numbers[] =
   { -10, 0, 0xAFFFF };

/* Numbers for float testing */
float fnumbers[] =
   { -3.141592653589e-8, 0.0, -0.0, 3.141592653589};

   /* Special numbers for float testing */
int fspec[] =
{
    0x7f800000,    /*  inf  */
    0xff800000,    /* -inf  */
    0x7f800001,    /*  Qnan */
    0xffc00001,    /* -Snan */
}; 
char *flags[] =
   { "-", "+", " ", "#", "0","" };
char specifiers[] =
   { 'd', 'i', 'o', 'u', 'x', 'X' };
char fspecifiers[] =
   { 'e', 'E', 'f','F', 'g', 'G' };
char * prec_prefix[] =
   { "4", "7","" };
char * prec_suffix[] =
   {".7", ".4","" };
 #endif /* TEST_H */
/* EOF */
