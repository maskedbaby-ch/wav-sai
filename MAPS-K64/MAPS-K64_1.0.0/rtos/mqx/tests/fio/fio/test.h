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
* $Version : 3.8.5.0$
* $Date    : Sep-4-2012$
*
* Comments:
*
*
*END************************************************************************/

const char * const compare1[] =
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

const _mqx_uint compare2[] =
{
 /* #2.1  */      1,
 /*       */      2,      3,     97,
 /*       */      4,      5,     98,
 /*       */      6,      7,     99,
 /*       */      8,      9,    100,
 /*       */      5,      4,    321,
 /* #2.2  */    123,    456,    789,
 /* #2.3  */    456,    789,      0,
 /* #2.4  */    123,    789,      0,
 /* #2.5  */    789,      0,      0,
 /* #2.6  */    123,    456,      0,
 /* #2.7  */    456,      0,      0,
 /* #2.8  */    123,      0,      0,
 /* #2.9  */      0,      0,      0,
 /* #2.10 */    123,    456,    789,
 /* #2.11 */   0xff,    0x4,   0xf8,
 /* #2.12 */    0x1,    0x2,    0x3,
 /* #2.12 */    0x4,    0x5,    0x6,
 /* #2.12 */    0x7,    0x8,    0x9,
 /* #2.12 */    0xa,    0xb,    0xc,
 /* #2.12 */    0xd,    0xe,    0xf,
 /* #2.12 */   0x10,   0x11,   0x12,
 /* #2.13 */    0x1,    0x2,    0x3,
 /* #2.14 */    0x1,    0x2,    0x3,
 /* #2.15 */    0x1,    0x2,    0x3,
 /* #2.16 */    0x1,    0x2,    0x3,
 /* #2.17 */    0xa,    0xb,    0xc,
 /* #2.18 */  0xabc,  0xdef, 0xabab,
 /* #2.19 */    123,    456,    789,
 /* #2.20 */    123,    456,    789,
 /* #2.21 */    123,    456,    789,
 /* #2.22 */      1,     23,    456,
 /* #2.23 */     12,     34,    567,
 /* #2.24 */    123,     45,    678,
 /* #2.25 */   1234,     56,    789,
 /* #2.26 */  12345,     67,     89,
 /* #2.27 */    123,    456,    789,
 /* #2.28 */    123,    456,    789,
 /* #2.29 */    123,    456,    789,
 /* #2.30 */    123,    789,      0,
 /* #2.31 */    123,    345,      7,
};

/* EOF */
