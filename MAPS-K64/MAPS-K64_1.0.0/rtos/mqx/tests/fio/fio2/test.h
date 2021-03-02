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
* $Version : 4.0.2.0$
* $Date    : Jul-28-2013$
*
* Comments:
*
*   This file contains definitions, function prototypes and global variables
*   for test suite suite_fio2 of Standard I/O testing module.
*
*END************************************************************************/

#define MAIN_TASK (10)
#define FAKE_DEV  ("fake:")
#define LEN       (32)
#define I         (_mqx_uint)(555)
#define R         (double)(5.5)

/* Test printing characters */
const char * const print_arr[] =
{
    "%-+#0 |", "+555   |", "01053  |", "0x22b    |", "+5.50e+00  |", "+5.50      |",
    " %-+# |", "+555   |", "01053  |", "0x22b    |", "+5.50e+00  |", "+5.50      |",
    " %-+0 |", "+555   |", "1053   |", "22b      |", "+5.50e+00  |", "+5.50      |",
    "  %-+ |", "+555   |", "1053   |", "22b      |", "+5.50e+00  |", "+5.50      |",
    " %-#0 |", "555    |", "01053  |", "0x22b    |", "5.50e+00   |", "5.50       |",
    "  %-# |", "555    |", "01053  |", "0x22b    |", "5.50e+00   |", "5.50       |",
    "  %-0 |", "555    |", "1053   |", "22b      |", "5.50e+00   |", "5.50       |",
    "   %- |", "555    |", "1053   |", "22b      |", "5.50e+00   |", "5.50       |",
    " %+#0 |", "+00555 |", "001053 |", "0x00022b |", "+05.50e+00 |", "+000005.50 |",
    "  %+# |", "  +555 |", " 01053 |", "   0x22b |", " +5.50e+00 |", "     +5.50 |",
    "  %+0 |", "+00555 |", "001053 |", "0000022b |", "+05.50e+00 |", "+000005.50 |",
    "   %+ |", "  +555 |", "  1053 |", "     22b |", " +5.50e+00 |", "     +5.50 |",
    "  %#0 |", "000555 |", "001053 |", "0x00022b |", "005.50e+00 |", "0000005.50 |",
    "   %# |", "   555 |", " 01053 |", "   0x22b |", "  5.50e+00 |", "      5.50 |",
    "   %0 |", "000555 |", "001053 |", "0000022b |", "005.50e+00 |", "0000005.50 |",
    "    % |", "   555 |", "  1053 |", "     22b |", "  5.50e+00 |", "      5.50 |",
};

/* Test scanning characters */
const _mqx_uint scan_arr[] =
{
   123,   456,   789,   456,   789,      0,
   123,   789,     0,   789,     0,      0,
   123,   456,     0,   456,     0,      0,
   123,     0,     0,     0,     0,      0,
   123,   456,   789,  0xff,   0x4,   0xf8,
   0x1,   0x2,   0x3,   0x4,   0x5,    0x6,
   0x7,   0x8,   0x9,   0xa,   0xb,    0xc,
   0xd,   0xe,   0xf,  0x10,  0x11,   0x12,
   0x1,   0x2,   0x3,   0x1,   0x2,    0x3,
   0x1,   0x2,   0x3,   0x1,   0x2,    0x3,
   0xa,   0xb,   0xc, 0xabc, 0xdef, 0xabab,
   123,   456,   789,   123,   456,    789,
   123,   456,   789,     1,    23,    456,
    12,    34,   567,   123,    45,    678,
  1234,    56,   789, 12345,    67,     89,
   123,   456,   789,   123,   456,    789,
   123,   456,   789,   123,   789,      0,
   123,   345,     7,
};

/* Help testing functions _io_vfprintf and _io_vprintf */
_mqx_int test_vfprintf(MQX_FILE_PTR, const char *, ...);
/* Help testing functions _io_vsprintf and _io_vsnprintf */
_mqx_int test_vsnprintf(char *, _mqx_int, const char *, ...);
/* Help testing functions _io_fscanf and _io_scanf */
bool test_fscanf(MQX_FILE_PTR, const char *);

/* Main task prototype */
void main_task(uint32_t);
