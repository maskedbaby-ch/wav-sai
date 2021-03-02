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
* $Date    : Jun-23-2011$
*
* Comments:
*
*   This include file is used to define the demonstation constants
*
*END************************************************************************/

#define MAIN_TASK       10


/* 
** If this define is uncommented the mem driver
** will use block mode - 2.50 only
*/
/* #define USE_BLOCK_MODE */

/*
** Defines about the disk infomation
**
** TOTAL NUMBER OF SECTORS = 
**   SECTS_PER_TRACK * NUM_OF_HEADS * MFS_CYL - HIDDEN_SECTS
**
*/
#define MFS_CYL          738
#define PHYSICAL_DRI     0x80
#define MEDIA_DESC       0xF8
#define BYTES_PER_SECT   512
#define SECTS_PER_TRACK  32
#define NUM_OF_HEADS     4
#define NUM_SECTORS      20
#define HIDDEN_SECTS     32
#define RESERVED_SECTS   1

#define SUBDIR_NAME "mysubdir"

#define FILE1_STR  "abcdefgn.gha.txt"
#define FILE2_STR  "abcdefgn.gha.txt4"
#define FILE3_STR  "bob"
#define FILE4_STR  "bob.txt"
#define FILE5_STR  "bob.udp"
#define FILE6_STR  "nbob"
#define FILE7_STR  "nbob.abc"
#define FILE8_STR  "nbob12345678.xyz"
#define FILE9_STR  "nbob12345678b.xyz"
#define FILE10_STR "pabcdabcdefgh.nnn"
#define FILE11_STR "pabcdefgh.nnn"

#define SFILE1_STR  "ABCDEF~1.TXT"
#define SFILE2_STR  "ABCDEF~2.TXT"
#define SFILE3_STR  "BOB"
#define SFILE4_STR  "BOB.TXT"
#define SFILE5_STR  "BOB.UDP"
#define SFILE6_STR  "NBOB"
#define SFILE7_STR  "NBOB.ABC"
#define SFILE8_STR  "NBOB12~1.XYZ"
#define SFILE9_STR  "NBOB12~2.XYZ"
#define SFILE10_STR "PABCDA~1.NNN"
#define SFILE11_STR "PABCDE~1.NNN"

#define NUM_FILES 11


#define SRC1_STR   "*.*"
#define SRC2_STR   "*.*.**.*?"
#define SRC3_STR   "bob.*"
#define SRC4_STR   "*.txt"
#define SRC5_STR   "*."
#define SRC6_STR   "*.udp"
#define SRC7_STR   "*.txt4"
#define SRC8_STR   "*"
#define SRC9_STR   "*b.*"
#define SRC10_STR  "*b*.*"
#define SRC11_STR  "bob"
#define SRC12_STR  "???"
#define SRC13_STR  "???.*"
#define SRC14_STR  "*.???"
#define SRC15_STR  "?bob"
#define SRC16_STR  "?bob?"
#define SRC17_STR  "?bob*78.*"
#define SRC18_STR  "?bob*78*.*"
#define SRC19_STR  "pabcde*.nnn"
#define SRC20_STR  "p*abcde*.nnn"
/*Start CR1300 & CR1313*/
#define SRC21_STR  "\\mysubdir\\?bob*78*.*"

//#define NUM_PATTERNS 20
#define NUM_PATTERNS 21
/*End CR1300 & CR1313*/

#ifdef USE_BLOCK_MODE
#define MEM_OPEN_MODE    "b"
#else
#define MEM_OPEN_MODE    0
#endif

typedef struct ftest_srch_res_struct {
   char *PATTERN;
   char *RESULTS[NUM_FILES + 1];
} FTEST_SRCH_RES_STRUCT, * FTEST_SRCH_RES_STRUCT_PTR;

#ifdef __cplusplus
extern "C" {
#endif
 
extern void main_task(uint32_t param);

#ifdef __cplusplus
}
#endif
 
/* EOF */

