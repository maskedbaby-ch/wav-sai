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
* $FileName: test_gcc.c$
* $Version : 3.8.2.0$
* $Date    : Jun-22-2012$
*
* Comments:
*
*   This i
*
*END************************************************************************/
#include <stdio.h>
#include "../test.h"


int main(void) {
   int nr, f, s, psuff, ppref;
   char tmpstr[BUF_LENGTH];
   float *b;
printf("const char * const compare_array[] = \n{\n");
#if TEST_INTEGER_NRS
    printf("#if TEST_INTEGER_NRS\n");
    for (nr = 0; nr < NR_CNT; nr++)
    {
        for (f = 0; f < NR_FLAGS; f++)
        {
            for (s = 0; s < NR_SPECS; s++)
            {
                for (ppref = 0; ppref < NR_PREC_PREF; ppref++)
                {
                    for (psuff = 0; psuff < NR_PREC_SUFF; psuff++)
                    {
                        sprintf(tmpstr, "%%%s%s%s%c", flags[f], prec_prefix[ppref], prec_suffix[psuff], specifiers[s]);
                        printf("/*%10s : */ \"",tmpstr);
                        printf(tmpstr, numbers[nr]);
                        printf("\",\n");      
                    }
                }
            }
        }
    }
    printf("#endif\n");
#endif

#if TEST_FLOATING_NRS
    printf("#if TEST_FLOATING_NRS\n");
    for (nr = 0; nr < NR_FCNT; nr++)
    {
        for (f = 0; f < NR_FLAGS; f++)
        {
            for (s = 0; s < NR_FSPECS; s++)
            {
                for (ppref = 0; ppref < NR_PREC_PREF; ppref++)
                {
                    for (psuff = 0; psuff < NR_PREC_SUFF; psuff++)
                    {
                        sprintf(tmpstr, "%%%s%s%s%c", flags[f], prec_prefix[ppref], prec_suffix[psuff], fspecifiers[s]);
                        printf("/*%10s : */ \"",tmpstr);
                        printf(tmpstr, fnumbers[nr]);
                        printf("\",\n");
                    }
                }
            }
        }
    }
    for (nr = 0; nr < NR_FSCNT; nr++)
    {
        for (f = 0; f < NR_FLAGS; f++)
        {
             for (s = 0; s < NR_FSPECS; s++)
             {
                for (ppref = 0; ppref < NR_PREC_PREF; ppref++)
                {
                    for (psuff = 0; psuff < NR_PREC_SUFF; psuff++)
                    {
                        sprintf(tmpstr, "%%%s%s%s%c", flags[f], prec_prefix[ppref], prec_suffix[psuff], fspecifiers[s]);
                        printf("/*%10s : */ \"",tmpstr);
                        b = (float*)&fspec[nr];
                        printf(tmpstr, *b);
                        printf("\",\n");
                    }
                }
            }
        }
    }
    printf("%10s \"\"\n","/* End of array */");
    printf("#endif\n");
#endif
    printf("};  \n");
   return 0;
}

