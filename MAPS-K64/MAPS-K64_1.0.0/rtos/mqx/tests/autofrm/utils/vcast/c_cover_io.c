#include "vcast_c_options.h"
/*
---------------------------------------
-- Copyright 2010 Vector Software    --
-- East Greenwich, Rhode Island USA --
---------------------------------------
*/

/* Defined variable usage for this file:

   Variable names that are indented apply only if the enclosing variable is set

   <<no defined variables set>>    : Output is written to the file TESTINSS.DAT

   VCAST_USE_STDOUT                : Output is written to stdout using puts
      VCAST_USE_PUTCHAR            : Output is written to stdout using putchar
      VCAST_USE_GH_SYSCALL         : Output is written to stdout using the GH syscall
                                     (For Green Hills INTEGRITY 178B)

   VCAST_CUSTOM_STD_OUTPUT         : custom user code required to write Output 
                                     to stdout or serial port interface
                                   : Need to also define VCAST_USE_STDOUT in order
                                     to set the stdout flag. 

   VCAST_CUSTOM_FILE_OUTPUT        : custom user file i/o code required to write 
                                     Output to TESTINSS.DAT 

   VCAST_USE_STD_STRING            : This define turns ON the use of memset from the
                                     system header string.h.

   VCAST_USE_STATIC_MEMORY         : No malloc is available, use alternate data.
      VCAST_MAX_MCDC_STATEMENTS    : The number of MCDC statement conditions 
                                     that can be reached when malloc is not 
                                     available. 
   VCAST_MAX_COVERED_SUBPROGRAMS   : The number of subprograms that may be
                                     covered. 
   VCAST_ENABLE_DATA_CLEAR_API     : Enable this macro to add the API
                                     VCAST_CLEAR_COVERAGE_DATA, which allows
                                     you to clear the currently collected
                                     coverage data during the execution of
                                     the instrumented executable.
   VCAST_ATTRIBUTE_CODE            : Allows the user to specify an attribute
                                     that will be placed before the ascii, 
                                     binary and subprogram coverage pool 
                                     global variables. This is useful for 
                                     putting this data in specific places
                                     in memory.
   VCAST_DUMP_CALLBACK             : If this is defined to a function name, 
                                     then when the user calls 
                                     VCAST_DUMP_COVERAGE_DATA, the function
                                     this was defined to will be called. The 
                                     purpose is to allow the users main loop
                                     to be given a chance to run within a 
                                     certain time frame.
   VCAST_FLUSH_DATA                : Use the flush system call after each
                                     string is written with fprintf in
                                     VCAST_WRITE_TO_INST_FILE_COVERAGE. The
                                     default is disabled. Define to any value
                                     to enable.
   VCAST_APPEND_WIN32_PID          : Append the process id to the TESTINSS.DAT
                                     file. This uses windows specific system
                                     calls to get the pid.
   VCAST_APPEND_POSIX_PID          : Append the process id to the TESTINSS.DAT
                                     file. This uses Posix specific system
                                     calls to get the pid.
   VCAST_APPEND_SECONDS_SINCE_EPOCH: Append the number of seconds since the
                                     epoch to the TESTINSS.DAT
                                     file. This uses the C library time() call.
*/

#ifndef __C_COVER_H__
#define __C_COVER_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VCAST_PRINTF_INTEGER 
#define VCAST_PRINTF_INTEGER "%d"
#endif
#ifndef VCAST_PRINTF_STRING
#define VCAST_PRINTF_STRING "%s"
#endif

#ifndef VCAST_ANIMATION_PARAMETERS
#define VCAST_ANIMATION_PARAMETERS int unit_index, int subprogram_number
#endif

/* AVL prototypes */
typedef struct vcast_mcdc_statement* VCAST_elementType;
struct AVLNode;
typedef struct AVLNode *VCAST_position;
typedef struct AVLNode *AVLTree;
VCAST_position vcast_find (VCAST_elementType VCAST_X, AVLTree VCAST_T);
AVLTree vcast_insert (VCAST_elementType VCAST_X, AVLTree VCAST_T);

struct vcast_mcdc_statement
{
  unsigned long mcdc_bits;
  unsigned long mcdc_bits_used;
};
typedef struct vcast_mcdc_statement *vcast_mcdc_statement_ptr;

struct vcast_statement_coverage
{
  /* Allocate N/8 bytes where N is the number of statements */
  char *coverage_bits;
  /* The number of statement statements */
  short vcast_num_statement_statements;
};
typedef struct vcast_statement_coverage *vcast_statement_coverage_ptr;

struct vcast_branch_coverage
{
  /* Allocate N/4 bytes where N is the number of statements */
  char *branch_bits_true;
  char *branch_bits_false;
  /* The number of branch statements */
  short vcast_num_branch_statements;
};
typedef struct vcast_branch_coverage *vcast_branch_coverage_ptr;

struct vcast_mcdc_coverage
{
  /* For each statement hit, a 'struct vcast_mcdc_statement'
   * will be inserted into this structure to represent the
   * statements that have been hit.
   * Allocate N tree's where N is the number of statements.
   */
  AVLTree *mcdc_data;
  /* The number of mcdc statements */
  short vcast_num_mcdc_statements;
};
typedef struct vcast_mcdc_coverage *vcast_mcdc_coverage_ptr;

struct vcast_STATEMENT_MCDC_coverage
{
  struct vcast_statement_coverage *statement_coverage;
  struct vcast_mcdc_coverage *mcdc_coverage;
};
typedef struct vcast_STATEMENT_MCDC_coverage *vcast_STATEMENT_MCDC_coverage_ptr;

struct vcast_STATEMENT_BRANCH_coverage
{
  struct vcast_statement_coverage *statement_coverage;
  struct vcast_branch_coverage *branch_coverage;
};
typedef struct vcast_STATEMENT_BRANCH_coverage *vcast_STATEMENT_BRANCH_coverage_ptr;

enum vcast_coverage_kind
{
  VCAST_COVERAGE_STATEMENT = 1,
  VCAST_COVERAGE_BRANCH = 2,
  VCAST_COVERAGE_MCDC = 4,
  VCAST_COVERAGE_STATEMENT_MCDC = 8,
  VCAST_COVERAGE_STATEMENT_BRANCH = 16
};

#ifndef VCAST_COVERAGE_KIND_TYPE
#define VCAST_COVERAGE_KIND_TYPE char
#endif

struct vcast_subprogram_coverage
{
  /* The unit id */
  short vcast_unit_id;
  /* The subprogram id */
  short vcast_subprogram_id;

  /* if coverage_kind == VCAST_COVERAGE_STATEMENT
   *   coverage_ptr is of type 'struct vcast_statement_coverage *'
   * if coverage_kind == VCAST_COVERAGE_BRANCH
   *   coverage_ptr is of type 'struct vcast_branch_coverage *'
   * if coverage_kind == VCAST_COVERAGE_MCDC
   *   coverage_ptr is of type 'struct vcast_mcdc_coverage *'
   * if coverage_kind == VCAST_COVERAGE_STATEMENT_MCDC
   *   coverage_ptr is of type 'struct vcast_STATEMENT_MCDC_coverage *'
   * if coverage_kind == VCAST_COVERAGE_STATEMENT_BRANCH
   *   coverage_ptr is of type 'struct vcast_STATEMENT_BRANCH_coverage *'
   */
  VCAST_COVERAGE_KIND_TYPE coverage_kind;
  void *coverage_ptr;
};

VCAST_CONDITION_TYP
VCAST_SAVE_MCDC_SUBCONDITION (struct vcast_mcdc_statement *mcdc_statement,
                              int bit_index, VCAST_CONDITION_TYP condition);

VCAST_CONDITION_TYP
VCAST_SAVE_MCDC_STATEMENT_MCDC_SUBCONDITION (
                              struct vcast_mcdc_statement *mcdc_statement,
                              int bit_index, VCAST_CONDITION_TYP condition);

void VCAST_SAVE_STATEMENT_REALTIME(struct vcast_subprogram_coverage *coverage, 
                             int statement);

VCAST_CONDITION_TYP 
VCAST_SAVE_BRANCH_CONDITION_REALTIME(struct vcast_subprogram_coverage *coverage,
                               int statement,
                               VCAST_CONDITION_TYP condition);

VCAST_CONDITION_TYP 
VCAST_SAVE_MCDC_CONDITION_REALTIME(struct vcast_subprogram_coverage *coverage,
                                   struct vcast_mcdc_statement *mcdc_statement,
                             int statement, VCAST_CONDITION_TYP condition);

void VCAST_SAVE_STATEMENT_STATEMENT_MCDC_REALTIME(
                             struct vcast_subprogram_coverage *coverage, 
                             int statement);
VCAST_CONDITION_TYP 
VCAST_SAVE_MCDC_STATEMENT_MCDC_CONDITION_REALTIME(
                             struct vcast_subprogram_coverage *coverage,
                             struct vcast_mcdc_statement *mcdc_statement,
                             int statement, VCAST_CONDITION_TYP condition);

void VCAST_SAVE_STATEMENT_STATEMENT_BRANCH_REALTIME(
                             struct vcast_subprogram_coverage *coverage, 
                             int statement);

VCAST_CONDITION_TYP 
VCAST_SAVE_BRANCH_STATEMENT_BRANCH_CONDITION_REALTIME(
                               struct vcast_subprogram_coverage *coverage,
                               int statement,
                               VCAST_CONDITION_TYP condition);
void VCAST_SAVE_STATEMENT_ANIMATION(
                             VCAST_ANIMATION_PARAMETERS, 
                             int statement);

VCAST_CONDITION_TYP 
VCAST_SAVE_BRANCH_CONDITION_ANIMATION(
                               VCAST_ANIMATION_PARAMETERS,
                               int statement,
                               VCAST_CONDITION_TYP condition,
                               VCAST_CONDITION_TYP onPath);
VCAST_CONDITION_TYP 
VCAST_SAVE_MCDC_CONDITION_ANIMATION(
                             VCAST_ANIMATION_PARAMETERS,
                             struct vcast_mcdc_statement *mcdc_statement,
                             int statement, VCAST_CONDITION_TYP condition);

void VCAST_SAVE_STATEMENT_STATEMENT_MCDC_ANIMATION(
                             VCAST_ANIMATION_PARAMETERS,
                             int statement);

VCAST_CONDITION_TYP 
VCAST_SAVE_MCDC_STATEMENT_MCDC_CONDITION_ANIMATION(
                             VCAST_ANIMATION_PARAMETERS,
                             struct vcast_mcdc_statement *mcdc_statement,
                             int statement, VCAST_CONDITION_TYP condition);

void VCAST_SAVE_STATEMENT_STATEMENT_BRANCH_ANIMATION(
                             VCAST_ANIMATION_PARAMETERS,
                             int statement);

VCAST_CONDITION_TYP 
VCAST_SAVE_BRANCH_STATEMENT_BRANCH_CONDITION_ANIMATION(
                               VCAST_ANIMATION_PARAMETERS,
                               int statement,
                               VCAST_CONDITION_TYP condition);

VCAST_CONDITION_TYP 
VCAST_SAVE_BRANCH_CONDITION_BUFFERED(
                            struct vcast_subprogram_coverage *coverage,
                            int statement,
                            VCAST_CONDITION_TYP condition);

VCAST_CONDITION_TYP 
VCAST_SAVE_MCDC_CONDITION_BUFFERED(
                          struct vcast_subprogram_coverage *coverage,
                          struct vcast_mcdc_statement *mcdc_statement,
                          int statement, VCAST_CONDITION_TYP condition);

VCAST_CONDITION_TYP 
VCAST_SAVE_MCDC_STATEMENT_MCDC_CONDITION_BUFFERED(
                          struct vcast_subprogram_coverage *coverage,
                          struct vcast_mcdc_statement *mcdc_statement,
                          int statement, VCAST_CONDITION_TYP condition);

VCAST_CONDITION_TYP 
VCAST_SAVE_BRANCH_STATEMENT_BRANCH_CONDITION_BUFFERED(
                            struct vcast_subprogram_coverage *coverage,
                            int statement,
                            VCAST_CONDITION_TYP condition);

void VCAST_REGISTER_STATEMENT (struct vcast_subprogram_coverage **cov, 
   char *vcast_bits_statement,
   short vcast_unit_id, short vcast_subprogram_id, short vcast_size);
void VCAST_REGISTER_BRANCH (struct vcast_subprogram_coverage **cov, 
   short vcast_unit_id, short vcast_subprogram_id, short vcast_size);
void VCAST_REGISTER_MCDC (struct vcast_subprogram_coverage **cov, 
   short vcast_unit_id, short vcast_subprogram_id, short vcast_size);
void VCAST_REGISTER_STATEMENT_MCDC (struct vcast_subprogram_coverage **cov, 
   char *vcast_bits_statement,
   short vcast_unit_id, short vcast_subprogram_id, short vcast_size,
   short vcast_include_stmnt);
void VCAST_REGISTER_STATEMENT_BRANCH (struct vcast_subprogram_coverage **cov, 
   char *vcast_bits_statement,
   short vcast_unit_id, short vcast_subprogram_id, short vcast_size,
   short vcast_include_stmnt);
void VCAST_SUBPROGRAM_INIT_STATEMENT(struct vcast_subprogram_coverage *cov);
void VCAST_SUBPROGRAM_INIT_BRANCH(struct vcast_subprogram_coverage *cov);
void VCAST_SUBPROGRAM_INIT_MCDC(struct vcast_subprogram_coverage *cov);
void VCAST_SUBPROGRAM_INIT_STATEMENT_MCDC(struct vcast_subprogram_coverage *cov,
  short vcast_include_stmnt);
void VCAST_SUBPROGRAM_INIT_STATEMENT_BRANCH(struct vcast_subprogram_coverage *cov,
  short vcast_include_stmnt);
void VCAST_CLEAR_COVERAGE_DATA (void);
void VCAST_DUMP_COVERAGE_DATA (void);
void VCAST_INITIALIZE (void);

#ifdef __cplusplus
}
#endif

#endif
#include "vcast_c_options.h"

#ifndef VCAST_UNIT_TEST_TOOL

/*Allow file operations*/
#ifndef VCAST_USE_STDIO_OPS
#define VCAST_USE_STDIO_OPS
#endif /* VCAST_USE_STDIO_OPS */

#ifndef VCAST_USE_STATIC_MEMORY
#include <stdlib.h> /* Needed for malloc */
/* In the unit test tool, this will use the VCAST_malloc
   already defined in the unit test harness. In the coverage
   tool, malloc is currently used. */
#define VCAST_malloc malloc
#define VCAST_free free
#endif /* VCAST_USE_STATIC_MEMORY */
#endif /* VCAST_UNIT_TEST_TOOL */

#ifndef VCAST_ATTRIBUTE_CODE
#define VCAST_ATTRIBUTE_CODE
#endif /* VCAST_ATTRIBUTE_CODE */

#ifdef VCAST_APPEND_SECONDS_SINCE_EPOCH
#include <time.h>
#endif

/* we needed to add this define because some compilers, Code Warrior in 
 * particular, will interpret the number "1" as a signed int, which when
 * bit shifted as we do for capturing coverage data, into the 15th bit
 * position, will cause the sign bit to get sign extended into the 
 * the upper 16 bits, so instead of getting 0x00008000 when you perform 
 * (1<<15), you get 0xFFFF8000.
 */
#define VCAST_UNSIGNED_1 ((unsigned long)1)

#ifdef VCAST_USE_BUFFERED_ASCII_DATA

/*Do not use file operations*/
#undef VCAST_USE_STDIO_OPS

/* This is where all the captured coverage data will be written in
 * human readable form. */
char VCAST_ATTRIBUTE_CODE vcast_ascii_coverage_data_pool[VCAST_MAX_CAPTURED_ASCII_DATA];
int vcast_ascii_coverage_data_pos = 0;
int vcast_coverage_data_buffer_full = 0;
const char *VCAST_MAX_CAPTURED_ASCII_DATA_EXCEEDED_ERROR = "\nVCAST_MAX_CAPTURED_ASCII_DATA_EXCEEDED";
int vcast_max_captured_ascii_data_exceeded_error_size = 39;

#endif /* VCAST_USE_BUFFERED_ASCII_DATA */

#ifdef VCAST_DUMP_COVERAGE_AT_EXIT
int vcast_has_registered_atexit = 0;
void vcast_register_atexit(void) {
   atexit(VCAST_DUMP_COVERAGE_DATA);
}
#endif /* VCAST_DUMP_COVERAGE_AT_EXIT */

#ifndef VCAST_USE_GH_SYSCALL
#ifdef VCAST_USE_STDIO_OPS
#include <stdio.h>
#endif /* VCAST_USE_STDIO_OPS */
#endif /*VCAST_USE_GH_SYSCALL*/

#ifndef VCAST_USE_STDOUT
/* Normal case, uses a file for output */
#ifndef VCAST_UNIT_TEST_TOOL
#ifdef VCAST_USE_STDIO_OPS
FILE *vCAST_INST_FILE = 0;
#endif /* VCAST_USE_STDIO_OPS */
#endif /* VCAST_UNIT_TEST_TOOL */
#endif /* VCAST_USE_STDOUT */

#if defined VCAST_UNIT_TEST_TOOL
#define VCAST_WRITE_COVERAGE_DATA(string) { vCAST_CREATE_INST_FILE(); vectorcast_fprint_string_with_cr(vCAST_INST_FILE, string); }
#define VCAST_WRITE_COVERAGE_DATA_FLUSH(string) { vCAST_CREATE_INST_FILE(); vectorcast_fprint_string_with_cr(vCAST_INST_FILE, string); }
#else /* VCAST_UNIT_TEST_TOOL */
#define VCAST_WRITE_COVERAGE_DATA(string) (VCAST_WRITE_TO_INST_FILE_COVERAGE(string, 0))
#define VCAST_WRITE_COVERAGE_DATA_FLUSH(string) (VCAST_WRITE_TO_INST_FILE_COVERAGE(string, 1))
#endif /* VCAST_UNIT_TEST_TOOL */

/* The instrumentation code depends on this data, usually found in S2.c in a
 * test harness.  For standalone coverage, they need to be supplied here: */
#ifndef LONG_MAX
#define LONG_MAX 2147483647L
#endif /* LONG_MAX */

#ifndef VCAST_UNIT_TEST_TOOL

static char *
vcast_strcat (char *VC_S, const char *VC_T);
int
vcast_strlen(const char *VC_S);
static char *
vcast_int_to_string (char *buf, unsigned long vc_x);

#ifndef VCAST_USE_STDOUT

#ifdef VCAST_APPEND_WIN32_PID
#include <windows.h>
#elif VCAST_APPEND_POSIX_PID
#include <sys/types.h>
#include <unistd.h>
#endif /* end VCAST_APPEND_WIN32_PID */

/**
 * function to open the file that will get all the coverage data 
 * written to it. 
 *
 * @param  void 
 * @return void
 */
void vCAST_CREATE_INST_FILE (void)
{
#ifdef VCAST_CUSTOM_FILE_OUTPUT
  /* Insert your code here using your API to create the TESTINSS.DAT */
  /* file.  For example: my_fopen("TESTINSS.DAT");  */
  
  /* Use the vCAST_CREATE_INST_FILE() code below after the #else */
  /* as a guideline for what your code should do, e.g. set       */
  /* the file pointer vCAST_INST_FILE accordingly, if it         */
  /* hasn't already been set, and open for writing ("w")         */
  /* or appending ("a"). Then you can remove the #error line.     */

  /* Also set the defined variable: VCAST_CUSTOM_FILE_OUTPUT */
  /* in the VectorCAST Tool Options Dialog -> C/C++ tab */

#error You need to insert code in vCAST_CREATE_INST_FILE() function

#else
  char result_filename[32] = "TESTINSS.DAT";
#ifdef VCAST_APPEND_WIN32_PID
  DWORD pid = GetCurrentProcessId();
  sprintf (result_filename,
           VCAST_PRINTF_STRING "-" VCAST_PRINTF_INTEGER "." VCAST_PRINTF_STRING,
           "TESTINSS",
           pid, 
           "DAT");
#elif defined VCAST_APPEND_POSIX_PID
   result_filename[0] = 0;
   vcast_strcat(result_filename, "TESTINSS-");
   vcast_int_to_string(result_filename+9, (int)getpid());
   vcast_strcat(result_filename, ".DAT");
#elif defined VCAST_APPEND_SECONDS_SINCE_EPOCH
   sprintf(result_filename,
           VCAST_PRINTF_STRING "-" VCAST_PRINTF_INTEGER "." VCAST_PRINTF_STRING,
           "TESTINSS",
           (int)time(0),
           "DAT");
#endif
#ifdef VCAST_USE_STDIO_OPS
   if (!vCAST_INST_FILE)
#ifdef VCAST_APPEND_TO_TESTINSS
      vCAST_INST_FILE = fopen (result_filename, "a");
#else /* VCAST_APPEND_TO_TESTINSS */
      vCAST_INST_FILE = fopen (result_filename, "w");
#endif /* VCAST_APPEND_TO_TESTINSS */
#endif /* VCAST_USE_STDIO_OPS */
#endif /* VCAST_CUSTOM_FILE_OUTPUT */
}

 /**
 * function to close the file that has all the coverage data 
 * written in it. default file name is TESTINSS.DAT 
 *
 * @param  void
 * @return void
 */
void VCAST_CLOSE_FILE (void)
{
#ifdef VCAST_CUSTOM_FILE_OUTPUT
  /* Insert code here using your API to close the TESTINSS.DAT */
  /* file.  For example: "my_fclose(vCAST_INST_FILE);  */
  
  /* Use the vCAST_CLOSE_FILE() code after the #else       */
  /* below as a guideline for what your function should do */
  /* Then you can remove the #error line.                  */

  /* Also set the defined variable: VCAST_CUSTOM_FILE_OUTPUT */
  /* in the VectorCAST Tool Options Dialog -> C/C++ tab */

#error You need to insert code in VCAST_CLOSE_FILE() function

#else
#ifdef VCAST_USE_STDIO_OPS
   if (vCAST_INST_FILE > (FILE*)0 )
      fclose(vCAST_INST_FILE);
   vCAST_INST_FILE = (FILE*)0;
#endif /* VCAST_USE_STDIO_OPS */
#endif /* VCAST_CUSTOM_FILE_OUTPUT */
}
#endif /* VCAST_USE_STDOUT */
#endif /* VCAST_UNIT_TEST_TOOL */

#ifndef VCAST_UNIT_TEST_TOOL

#ifdef VCAST_USE_GH_SYSCALL

#ifdef __cplusplus
extern "C" int __ghs_syscall(int, ...);
#else /* __cplusplus */
extern int __ghs_syscall(int, ...);
#endif /* __cplusplus */

#endif /* end VCAST_USE_GH_SYS_CALL */


#define VCAST_DUMP_CALLBACK debugger_dump_data
void debugger_dump_data(void);
void debugger_dump_data(void)
{
    int i=0;
    vcast_ascii_coverage_data_pos=0;
    vcast_coverage_data_buffer_full=0;
    
    for(i=0; i<VCAST_MAX_CAPTURED_ASCII_DATA; i++){
        vcast_ascii_coverage_data_pool[i] = 0;
    }
    
}
 /**
 * Original vcast_write_to_inst_file function.  Currently used by default 
 * when coverage optimizations are turned off. 
 *
 * @param const char S[] string with single line of coverage data 
 * @param int flush  flag to cause data flush for file i/o if flush != 0
 * @return void
 */
void VCAST_WRITE_TO_INST_FILE_COVERAGE (const char S[], int flush)
{
#ifdef VCAST_USE_BUFFERED_ASCII_DATA
  if (!vcast_coverage_data_buffer_full) {
    int vc_size = vcast_strlen(S);
    if (vcast_ascii_coverage_data_pos + vc_size + 2 >= VCAST_MAX_CAPTURED_ASCII_DATA) {
       vcast_ascii_coverage_data_pool[vcast_ascii_coverage_data_pos] = '\0';
       debugger_dump_data();
    } 
    if(!vcast_coverage_data_buffer_full) {
       while (*S) {
         vcast_ascii_coverage_data_pool[vcast_ascii_coverage_data_pos++] = *S;
         ++S;
       }
    }

    /* if not enough room in the buffer for all the coverage data */
    if (vcast_coverage_data_buffer_full) {
      const char *vcast_cur = VCAST_MAX_CAPTURED_ASCII_DATA_EXCEEDED_ERROR;
      vcast_ascii_coverage_data_pos -= (vcast_max_captured_ascii_data_exceeded_error_size+2);
      for (; *vcast_cur; ++vcast_cur) {
        vcast_ascii_coverage_data_pool[vcast_ascii_coverage_data_pos++] = *vcast_cur;
      }
    }

    vcast_ascii_coverage_data_pool[vcast_ascii_coverage_data_pos++] = '\n';
    // vcast_ascii_coverage_data_pool[vcast_ascii_coverage_data_pos] = '\0';
  }

#else /* VCAST_USE_BUFFERED_ASCII_DATA */
#ifdef VCAST_CUSTOM_FILE_OUTPUT
  /* Insert your own call to an API to write "S" out of your target */
  /* Note that you should append a carriage return "/n" to the end */
  /*      of the "S" string before printing           */

  /* For example: see code snippet below ...          */

  /*    vCAST_CREATE_INST_FILE();                     */
  /*    my_fprintf ( vCAST_INST_FILE, "%s\n", S );    */
  /*    if (flush) {                                  */
  /*        my_fflush (vCAST_INST_FILE);              */
  /*    }                                             */

  /* Then you can remove the #error line below.       */

  /* Also set the defined variable: VCAST_CUSTOM_FILE_OUTPUT */
  /* in the VectorCAST Tool Options Dialog -> C/C++ tab */

#error Need to insert file i/o code in VCAST_WRITE_TO_INST_FILE_COVERAGE() 

#else
#ifdef VCAST_USE_STDIO_OPS
#ifdef VCAST_USE_STDOUT
#ifdef VCAST_USE_PUTCHAR

   int i, len;
   len = vcast_strlen( S );
   for( i=0; i<len; ++i ) {
      putchar( S[i] );
   }
   putchar( '\n' );

#else /* ndef VCAST_USE_PUTCHAR */
#ifdef VCAST_USE_GH_SYSCALL

   char Str[42];
   int  Len = vcast_strlen (S);
   Str[0] = 0;
   VC_strcat (Str, S);
   VC_strcat (Str, "\n"); 
   /* we hardcode stdout (1) as the file handle */
   __ghs_syscall(0x40001, 1, Str, Len+1);
     
#else /* ndef VCAST_USE_GH_SYSCALL */    
#ifdef VCAST_CUSTOM_STD_OUTPUT
  /* Insert user code here to write "S" out to serial port  */
  /* or STDOUT of your target,                              */
  /* then you can remove the #error line below.             */
  /* you will have to cut&paste this serial port data from  */
  /* the application receiving the data (e.g. HyperTerminal */
  /* and put into a file that can be imported into Cover    */
  /* environment   */

  /* Also set the defined variable: VCAST_CUSTOM_STD_OUTPUT */
  /* in the VectorCAST Tool Options Dialog -> C/C++ tab */

  /* Need to also define VCAST_USE_STDOUT to set the stdout flag */

#error Need to insert serial/std i/o code in VCAST_WRITE_TO_INST_FILE_COVERAGE() 

#else
   puts( S );
#endif /* VCAST_CUSTOM_STD_OUTPUT */
     
#endif /* end VCAST_USE_GH_SYS_CALL */
#endif /* end VCAST_USE_PUTCHAR */

#else /* ndef VCAST_USE_STDOUT */

   vCAST_CREATE_INST_FILE();
   fprintf ( vCAST_INST_FILE, VCAST_PRINTF_STRING "\n", S );
   if (flush) {
     fflush (vCAST_INST_FILE);
   }
#ifdef VCAST_FLUSH_DATA
   else {
     fflush (vCAST_INST_FILE);
   }
#endif /* VCAST_FLUSH_DATA */
   
#endif /* end VCAST_USE_STDOUT */
#endif /* VCAST_USE_STDIO_OPS */
#endif /* VCAST_CUSTOM_FILE_OUTPUT */
#endif /* VCAST_USE_BUFFERED_ASCII_DATA */
}
#endif /* end VCAST_UNIT_TEST_TOOL */

static char *
vcast_strcat (char *VC_S, const char *VC_T)
{
   int vc_i,vc_j;
   vc_i = vc_j = 0;
   while ( VC_S[vc_i] != 0 )
      vc_i++;           /* find end of VC_S */
   while ( (VC_S[vc_i++] = VC_T[vc_j++]) != 0 ) ; /* copy VC_T */
   return VC_S;
}

int
vcast_strlen(const char *VC_S)
{
   int vc_result = 0;

   while(*VC_S) {
      vc_result++;
      VC_S++;
   }

   return vc_result;
}

static char *
vcast_int_to_string (char *buf, unsigned long vc_x)
{
   int vc_i;
   unsigned long vc_cur = vc_x;
   int vc_size = 0;

   if (vc_x == 0) {
      buf[0] = '0';
      buf[1] = '\0';
      return buf;
   }

   while (vc_cur != 0) {
      vc_size++;
      vc_cur /= 10;
   }

   for (vc_i = 0; vc_x != 0; vc_i++){
      buf[vc_size-vc_i-1] = (char)((vc_x % 10) + '0');
      vc_x /= 10;
   }

   buf[vc_size] = 0;

   return buf;
}

/*
   VCAST_STATEMENT_ID_BUF_SIZE     : The number of characters needed to print
                                     a statement id. The default is 5, which 
                                     supports 9,999 statements in a single
                                     function.
   VCAST_SUBPROGRAM_ID_BUF_SIZE    : The number of characters needed to print
                                     a subprogram id. The default is 5, which 
                                     supports 9,999 functions in a single
                                     unit.
   VCAST_UNIT_ID_BUF_SIZE          : The number of characters needed to print
                                     a unit id. The default is 5, which 
                                     supports 9,999 units in a project.
*/

/* The following defines will control the sizes of intermediate character 
 * strings that VectorCAST uses to output coverage data. Keeping these 
 * values close to the sizes needed results in less memory being used 
 * on the stack, during test execution.
 */
#define VCAST_STATEMENT_ID_BUF_SIZE 5
#define VCAST_SUBPROGRAM_ID_BUF_SIZE 5
#define VCAST_UNIT_ID_BUF_SIZE 5

/* The user can never go above the maximum of the supported 32 bit 
   limit, which is 4,294,967,296. */
#define VCAST_MCDC_ID_BUF_SIZE 11

/* The amount of space needed to write a line of statement data is,
 * The size of the unit id, plus a space, plus the size of the 
 * subprogram id, plus a space, plus the size of the statement id,
 * plus the NUL character.
 */
#define VCAST_STATEMENT_DATA_BUF_SIZE 18

/* The branch data is the same as the statement data, except it has
 * an extra space, and then an extra character (T or F) at the end. 
 * The statement case takes care of the NUL char.
 */
#define VCAST_BRANCH_DATA_BUF_SIZE 20

/* The mcdc data is the same as the statement data, except that
 * it has an extra space, plus a mcdc bits number, plus a space,
 * plus an mcdc bits used number.
 * The statement case takes care of the NUL char.
 */
#define VCAST_MCDC_DATA_BUF_SIZE 42

void
VCAST_WRITE_STATEMENT_DATA (short unit, short sub, int statement)
{
   char vcast_out_str[VCAST_STATEMENT_DATA_BUF_SIZE];
   char sub_buf[VCAST_SUBPROGRAM_ID_BUF_SIZE];
   char statement_buf[VCAST_STATEMENT_ID_BUF_SIZE];
   vcast_int_to_string (vcast_out_str, unit);
   vcast_int_to_string (sub_buf, sub);
   vcast_strcat (vcast_out_str, " ");
   vcast_strcat (vcast_out_str, sub_buf);
   vcast_int_to_string (statement_buf, statement);
   vcast_strcat (vcast_out_str, " ");
   vcast_strcat (vcast_out_str, statement_buf);
   VCAST_WRITE_COVERAGE_DATA (vcast_out_str);
}

void
VCAST_WRITE_BRANCH_DATA (short unit, short sub, int statement, const char *TorF)
{
   char vcast_out_str[VCAST_BRANCH_DATA_BUF_SIZE];
   char sub_buf[VCAST_SUBPROGRAM_ID_BUF_SIZE];
   char statement_buf[VCAST_STATEMENT_ID_BUF_SIZE];
   vcast_int_to_string (vcast_out_str, unit);
   vcast_int_to_string (sub_buf, sub);
   vcast_strcat (vcast_out_str, " ");
   vcast_strcat (vcast_out_str, sub_buf);
   vcast_int_to_string (statement_buf, statement);
   vcast_strcat (vcast_out_str, " ");
   vcast_strcat (vcast_out_str, statement_buf);
   vcast_strcat (vcast_out_str, " ");
   vcast_strcat (vcast_out_str, TorF);
   VCAST_WRITE_COVERAGE_DATA (vcast_out_str);
}

void
VCAST_WRITE_MCDC_DATA (short unit, short sub, int statement, 
      unsigned long mcdc_bits, unsigned long mcdc_bits_used)
{
   char vcast_out_str[VCAST_MCDC_DATA_BUF_SIZE];
   char sub_buf[VCAST_SUBPROGRAM_ID_BUF_SIZE];
   char statement_buf[VCAST_STATEMENT_ID_BUF_SIZE]; 
   char mcdc_bits_buf[VCAST_MCDC_ID_BUF_SIZE]; 
   char mcdc_bits_used_buf[VCAST_MCDC_ID_BUF_SIZE];
   vcast_int_to_string (vcast_out_str, unit);
   vcast_int_to_string (sub_buf, sub);
   vcast_strcat (vcast_out_str, " ");
   vcast_strcat (vcast_out_str, sub_buf);
   vcast_int_to_string (statement_buf, statement);
   vcast_strcat (vcast_out_str, " ");
   vcast_strcat (vcast_out_str, statement_buf);
   vcast_int_to_string (mcdc_bits_buf, mcdc_bits);
   vcast_strcat (vcast_out_str, " ");
   vcast_strcat (vcast_out_str, mcdc_bits_buf);
   vcast_int_to_string (mcdc_bits_used_buf, mcdc_bits_used);
   vcast_strcat (vcast_out_str, " ");
   vcast_strcat (vcast_out_str, mcdc_bits_used_buf);
   VCAST_WRITE_COVERAGE_DATA (vcast_out_str);
}

void
vcast_fatal_error (const char *msg)
{
  VCAST_WRITE_COVERAGE_DATA_FLUSH (msg);
}

#ifdef VCAST_USE_STATIC_MEMORY
#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
struct vcast_mcdc_statement vcast_mcdc_statement_pool[VCAST_MAX_MCDC_STATEMENTS];
struct vcast_mcdc_statement *vcast_mcdc_statement_pool_ptr = vcast_mcdc_statement_pool;
struct vcast_mcdc_statement *vcast_mcdc_statement_end = vcast_mcdc_statement_pool + VCAST_MAX_MCDC_STATEMENTS;
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */
static int vcast_max_covered_subprograms_exceeded = 0;
#endif /* VCAST_USE_STATIC_MEMORY */

#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
static int vcast_max_mcdc_statements_exceeded = 0;
struct vcast_mcdc_statement *
vcast_get_mcdc_statement (void)
{
  struct vcast_mcdc_statement *ptr;
#ifdef VCAST_USE_STATIC_MEMORY
  if (vcast_mcdc_statement_pool_ptr < vcast_mcdc_statement_pool ||
      vcast_mcdc_statement_pool_ptr >= vcast_mcdc_statement_end)
  {
    if (vcast_max_mcdc_statements_exceeded == 0)
	   vcast_fatal_error ("VCAST_MAX_MCDC_STATEMENTS exceeded!");
    vcast_max_mcdc_statements_exceeded = 1;
    return 0;
  }
  ptr = vcast_mcdc_statement_pool_ptr++;
#else /* VCAST_USE_STATIC_MEMORY */
  ptr = (struct vcast_mcdc_statement*) 
    VCAST_malloc (sizeof (struct vcast_mcdc_statement));
#endif /* VCAST_USE_STATIC_MEMORY */

  return ptr;
}

int
vcast_mcdc_statement_compare (struct vcast_mcdc_statement *ptr1,
                        struct vcast_mcdc_statement *ptr2)
{
  if (ptr1->mcdc_bits == ptr2->mcdc_bits)
  {
    if (ptr1->mcdc_bits_used == ptr2->mcdc_bits_used)
      return 0;
    else if (ptr1->mcdc_bits_used < ptr2->mcdc_bits_used)
      return -1;
    else
      return 1;
  } else if (ptr1->mcdc_bits < ptr2->mcdc_bits)
    return -1;
  else
    return 1;
}
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */

#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
VCAST_CONDITION_TYP
VCAST_SAVE_MCDC_SUBCONDITION (struct vcast_mcdc_statement *mcdc_statement,
                              int bit_index, VCAST_CONDITION_TYP condition)
{
   /* This is a sub-condition for an MCDC expression, just record the bit */
  if (condition)
    mcdc_statement->mcdc_bits |= (VCAST_UNSIGNED_1 << bit_index);

  mcdc_statement->mcdc_bits_used |= (VCAST_UNSIGNED_1 << bit_index);

  return condition;
}
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */

#ifdef VCAST_COVERAGE_TYPE_STATEMENT_MCDC
VCAST_CONDITION_TYP
VCAST_SAVE_MCDC_STATEMENT_MCDC_SUBCONDITION ( 
                              struct vcast_mcdc_statement *mcdc_statement,
                              int bit_index, VCAST_CONDITION_TYP condition)
{
   /* This is a sub-condition for an MCDC expression, just record the bit */
  if (condition)
    mcdc_statement->mcdc_bits |= (VCAST_UNSIGNED_1 << bit_index);

  mcdc_statement->mcdc_bits_used |= (VCAST_UNSIGNED_1 << bit_index);

  return condition;
}

#endif /* VCAST_COVERAGE_TYPE_STATEMENT_MCDC */

#if defined(VCAST_COVERAGE_IO_REAL_TIME)
#ifdef VCAST_COVERAGE_TYPE_STATEMENT
void VCAST_SAVE_STATEMENT_REALTIME(struct vcast_subprogram_coverage *coverage, 
                             int statement)
{
   int vcast_index = (statement >> 3), vcast_shift = statement % 8;
   char *coverage_bits =
     ((struct vcast_statement_coverage*)coverage->coverage_ptr)->coverage_bits;

   if ((coverage_bits[vcast_index] & (VCAST_UNSIGNED_1 << vcast_shift)) == 0)
   {
     coverage_bits[vcast_index] |= (VCAST_UNSIGNED_1 << vcast_shift);

     VCAST_WRITE_STATEMENT_DATA (coverage->vcast_unit_id,
         coverage->vcast_subprogram_id, statement+1);
   }
}

#endif /* VCAST_COVERAGE_TYPE_STATEMENT */

#ifdef VCAST_COVERAGE_TYPE_BRANCH
VCAST_CONDITION_TYP 
VCAST_SAVE_BRANCH_CONDITION_REALTIME(struct vcast_subprogram_coverage *coverage,
                               int statement,
                               VCAST_CONDITION_TYP condition)
{
   int vcast_index = (statement >> 3), vcast_shift = statement % 8;
   char *coverage_bits;

   if (condition)
     coverage_bits = ((struct vcast_branch_coverage*)coverage->coverage_ptr)->branch_bits_true;
   else
     coverage_bits = ((struct vcast_branch_coverage*)coverage->coverage_ptr)->branch_bits_false;

   if ((coverage_bits[vcast_index] & (VCAST_UNSIGNED_1 << vcast_shift)) == 0)
   {
     coverage_bits[vcast_index] |= (VCAST_UNSIGNED_1 << vcast_shift);

     VCAST_WRITE_BRANCH_DATA (coverage->vcast_unit_id, 
         coverage->vcast_subprogram_id, statement, 
         condition ? "T" : "F");
   }
 
  return condition;
}
#endif /* VCAST_COVERAGE_TYPE_BRANCH */

#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
VCAST_CONDITION_TYP 
VCAST_SAVE_MCDC_CONDITION_REALTIME(struct vcast_subprogram_coverage *coverage,
                             struct vcast_mcdc_statement *mcdc_statement,
                             int statement, VCAST_CONDITION_TYP condition)
{
   AVLTree *vcast_mcdc_data_ptr =
     ((struct vcast_mcdc_coverage*) coverage->coverage_ptr)->mcdc_data;

   /* Store the total expression */
   if (condition)
     mcdc_statement->mcdc_bits |= 1;
   mcdc_statement->mcdc_bits_used |= 1;

   /* Get the coverage data necessary */
   if (!vcast_find (mcdc_statement, vcast_mcdc_data_ptr[statement]))
   {
      struct vcast_mcdc_statement *vcast_mcdc_statement = vcast_get_mcdc_statement();
      if (!vcast_mcdc_statement)
        return condition;
      *vcast_mcdc_statement = *mcdc_statement;

      vcast_mcdc_data_ptr[statement] = 
         vcast_insert (vcast_mcdc_statement, vcast_mcdc_data_ptr[statement]);

      VCAST_WRITE_MCDC_DATA (coverage->vcast_unit_id, 
            coverage->vcast_subprogram_id, statement,
            mcdc_statement->mcdc_bits,
            mcdc_statement->mcdc_bits_used);
   }

   mcdc_statement->mcdc_bits = 0;
   mcdc_statement->mcdc_bits_used = 0;

   return condition;
}
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */

#ifdef VCAST_COVERAGE_TYPE_STATEMENT_MCDC
void VCAST_SAVE_STATEMENT_STATEMENT_MCDC_REALTIME(
                             struct vcast_subprogram_coverage *coverage, 
                             int statement)
{
   int vcast_index = (statement >> 3), vcast_shift = statement % 8;
   char *coverage_bits =
     ((struct vcast_STATEMENT_MCDC_coverage*)coverage->coverage_ptr)->statement_coverage->coverage_bits;

   if ((coverage_bits[vcast_index] & (VCAST_UNSIGNED_1 << vcast_shift)) == 0)
   {
     coverage_bits[vcast_index] |= (VCAST_UNSIGNED_1 << vcast_shift);

     VCAST_WRITE_STATEMENT_DATA (coverage->vcast_unit_id,
       coverage->vcast_subprogram_id, statement+1);
   }
}

VCAST_CONDITION_TYP 
VCAST_SAVE_MCDC_STATEMENT_MCDC_CONDITION_REALTIME( 
                             struct vcast_subprogram_coverage *coverage,
                             struct vcast_mcdc_statement *mcdc_statement,
                             int statement, VCAST_CONDITION_TYP condition)
{
   AVLTree *vcast_mcdc_data_ptr =
     ((struct vcast_STATEMENT_MCDC_coverage*) coverage->coverage_ptr)->mcdc_coverage->mcdc_data;

   /* Store the total expression */
   if (condition)
     mcdc_statement->mcdc_bits |= 1;
   mcdc_statement->mcdc_bits_used |= 1;

   /* Get the coverage data necessary */
   if (!vcast_find (mcdc_statement, vcast_mcdc_data_ptr[statement]))
   {
      struct vcast_mcdc_statement *vcast_mcdc_statement = vcast_get_mcdc_statement();
      if (!vcast_mcdc_statement)
        return condition;
      *vcast_mcdc_statement = *mcdc_statement;

      vcast_mcdc_data_ptr[statement] = 
         vcast_insert (vcast_mcdc_statement, vcast_mcdc_data_ptr[statement]);

      VCAST_WRITE_MCDC_DATA (coverage->vcast_unit_id, 
            coverage->vcast_subprogram_id, statement,
            mcdc_statement->mcdc_bits,
            mcdc_statement->mcdc_bits_used);
   }

   mcdc_statement->mcdc_bits = 0;
   mcdc_statement->mcdc_bits_used = 0;

   return condition;
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_MCDC */

#ifdef VCAST_COVERAGE_TYPE_STATEMENT_BRANCH
void VCAST_SAVE_STATEMENT_STATEMENT_BRANCH_REALTIME(
                             struct vcast_subprogram_coverage *coverage, 
                             int statement)
{
   int vcast_index = (statement >> 3), vcast_shift = statement % 8;
   char *coverage_bits =
     ((struct vcast_STATEMENT_BRANCH_coverage*)coverage->coverage_ptr)->statement_coverage->coverage_bits;

   if ((coverage_bits[vcast_index] & (VCAST_UNSIGNED_1 << vcast_shift)) == 0)
   {
     coverage_bits[vcast_index] |= (VCAST_UNSIGNED_1 << vcast_shift);

     VCAST_WRITE_STATEMENT_DATA (coverage->vcast_unit_id,
       coverage->vcast_subprogram_id, statement+1);
   }
}

VCAST_CONDITION_TYP 
VCAST_SAVE_BRANCH_STATEMENT_BRANCH_CONDITION_REALTIME(
                               struct vcast_subprogram_coverage *coverage,
                               int statement,
                               VCAST_CONDITION_TYP condition)
{
   int vcast_index = (statement >> 3), vcast_shift = statement % 8;
   char *coverage_bits; 

   if (condition)
     coverage_bits = (((struct vcast_STATEMENT_BRANCH_coverage*)coverage->coverage_ptr)->branch_coverage)->branch_bits_true;
   else
     coverage_bits = (((struct vcast_STATEMENT_BRANCH_coverage*)coverage->coverage_ptr)->branch_coverage)->branch_bits_false;

   if ((coverage_bits[vcast_index] & (VCAST_UNSIGNED_1 << vcast_shift)) == 0)
   {
     coverage_bits[vcast_index] |= (VCAST_UNSIGNED_1 << vcast_shift);

     VCAST_WRITE_BRANCH_DATA (coverage->vcast_unit_id, 
         coverage->vcast_subprogram_id, statement, 
         condition ? "T" : "F");
   }
 
  return condition;
}

#endif /* VCAST_COVERAGE_TYPE_STATEMENT_BRANCH */
#endif /* VCAST_COVERAGE_IO_REAL_TIME */

#if defined(VCAST_COVERAGE_IO_ANIMATION)
#ifdef VCAST_COVERAGE_TYPE_STATEMENT
void VCAST_SAVE_STATEMENT_ANIMATION(
                             int unit_index,
                             int subprogram_number, 
                             int statement)
{
  VCAST_WRITE_STATEMENT_DATA (unit_index,
    subprogram_number, statement+1);
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT */

#ifdef VCAST_COVERAGE_TYPE_BRANCH
VCAST_CONDITION_TYP 
VCAST_SAVE_BRANCH_CONDITION_ANIMATION(
                               int unit_index,
                               int subprogram_number,
                               int statement,
                               VCAST_CONDITION_TYP condition,
                               VCAST_CONDITION_TYP onPath )
{
  if ( onPath )
    VCAST_WRITE_BRANCH_DATA (unit_index, 
      subprogram_number, statement, 
      condition ? "T" : "F");
  else
    VCAST_WRITE_BRANCH_DATA (unit_index, 
      subprogram_number, statement, 
      condition ? "TX" : "FX");
 
  return condition;
}
#endif /* VCAST_COVERAGE_TYPE_BRANCH */

#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
VCAST_CONDITION_TYP 
VCAST_SAVE_MCDC_CONDITION_ANIMATION(
                             int unit_index,
                             int subprogram_number,
                             struct vcast_mcdc_statement *mcdc_statement,
                             int statement, VCAST_CONDITION_TYP condition)
{
   /* Store the total expression */
   if (condition)
     mcdc_statement->mcdc_bits |= 1;
   mcdc_statement->mcdc_bits_used |= 1;

   VCAST_WRITE_MCDC_DATA (unit_index,
         subprogram_number, statement,
         mcdc_statement->mcdc_bits,
         mcdc_statement->mcdc_bits_used);
   
   mcdc_statement->mcdc_bits = 0;
   mcdc_statement->mcdc_bits_used = 0;

   return condition;
}
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */

#ifdef VCAST_COVERAGE_TYPE_STATEMENT_MCDC
void VCAST_SAVE_STATEMENT_STATEMENT_MCDC_ANIMATION(
                             int unit_index,
                             int subprogram_number,
                             int statement)
{
  VCAST_WRITE_STATEMENT_DATA (unit_index,
    subprogram_number, statement+1);
}

VCAST_CONDITION_TYP
VCAST_SAVE_MCDC_STATEMENT_MCDC_CONDITION_ANIMATION(
                             int unit_index,
                             int subprogram_number,
                             struct vcast_mcdc_statement *mcdc_statement,
                             int statement, VCAST_CONDITION_TYP condition)
{
   /* Store the total expression */
   if (condition)
     mcdc_statement->mcdc_bits |= 1;
   mcdc_statement->mcdc_bits_used |= 1;

   VCAST_WRITE_MCDC_DATA (unit_index,
         subprogram_number, statement,
         mcdc_statement->mcdc_bits,
         mcdc_statement->mcdc_bits_used);

   mcdc_statement->mcdc_bits = 0;
   mcdc_statement->mcdc_bits_used = 0;

   return condition;
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_MCDC */

#ifdef VCAST_COVERAGE_TYPE_STATEMENT_BRANCH
void VCAST_SAVE_STATEMENT_STATEMENT_BRANCH_ANIMATION(
                             int unit_index,
                             int subprogram_number,
                             int statement)
{
  VCAST_WRITE_STATEMENT_DATA (unit_index,
    subprogram_number, statement+1);
}

VCAST_CONDITION_TYP 
VCAST_SAVE_BRANCH_STATEMENT_BRANCH_CONDITION_ANIMATION(
                               int unit_index,
                               int subprogram_number,
                               int statement,
                               VCAST_CONDITION_TYP condition)
{
  VCAST_WRITE_BRANCH_DATA (unit_index,
      subprogram_number, statement, 
      condition ? "T" : "F");
 
  return condition;
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_BRANCH */
#endif /* VCAST_COVERAGE_IO_ANIMATION */


#if defined(VCAST_COVERAGE_IO_BUFFERED)
#ifdef VCAST_COVERAGE_TYPE_BRANCH
VCAST_CONDITION_TYP 
VCAST_SAVE_BRANCH_CONDITION_BUFFERED(struct vcast_subprogram_coverage *coverage,
                            int statement,
                            VCAST_CONDITION_TYP condition)
{
   int vcast_index = (statement >> 3), vcast_shift = statement % 8;

   if (condition)
     ((struct vcast_branch_coverage*)coverage->coverage_ptr)->branch_bits_true[vcast_index] |= (VCAST_UNSIGNED_1 << vcast_shift);
   else
     ((struct vcast_branch_coverage*)coverage->coverage_ptr)->branch_bits_false[vcast_index] |= (VCAST_UNSIGNED_1 << vcast_shift);

   return condition;
}
#endif /* VCAST_COVERAGE_TYPE_BRANCH */

#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
VCAST_CONDITION_TYP 
VCAST_SAVE_MCDC_CONDITION_BUFFERED(struct vcast_subprogram_coverage *coverage,
                          struct vcast_mcdc_statement *mcdc_statement,
                          int statement, VCAST_CONDITION_TYP condition)
{
   AVLTree *vcast_mcdc_data_ptr =
     ((struct vcast_mcdc_coverage*) coverage->coverage_ptr)->mcdc_data;

   /* Store the total expression */
   if (condition)
     mcdc_statement->mcdc_bits |= 1;
   mcdc_statement->mcdc_bits_used |= 1;

   /* Get the coverage data necessary */
   if (!vcast_find (mcdc_statement, vcast_mcdc_data_ptr[statement]))
   {
      struct vcast_mcdc_statement *vcast_mcdc_statement = vcast_get_mcdc_statement();
      if (!vcast_mcdc_statement)
        return condition;
      *vcast_mcdc_statement = *mcdc_statement;

      vcast_mcdc_data_ptr[statement] = 
         vcast_insert (vcast_mcdc_statement, vcast_mcdc_data_ptr[statement]);
   }

   mcdc_statement->mcdc_bits = 0;
   mcdc_statement->mcdc_bits_used = 0;

   return condition;
}
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */

#ifdef VCAST_COVERAGE_TYPE_STATEMENT_MCDC
VCAST_CONDITION_TYP 
VCAST_SAVE_MCDC_STATEMENT_MCDC_CONDITION_BUFFERED(
                          struct vcast_subprogram_coverage *coverage,
                          struct vcast_mcdc_statement *mcdc_statement,
                          int statement, VCAST_CONDITION_TYP condition)
{
   AVLTree *vcast_mcdc_data_ptr =
     ((struct vcast_STATEMENT_MCDC_coverage*) coverage->coverage_ptr)->mcdc_coverage->mcdc_data;

   /* Store the total expression */
   if (condition)
     mcdc_statement->mcdc_bits |= 1;
   mcdc_statement->mcdc_bits_used |= 1;

   /* Get the coverage data necessary */
   if (!vcast_find (mcdc_statement, vcast_mcdc_data_ptr[statement]))
   {
      struct vcast_mcdc_statement *vcast_mcdc_statement = vcast_get_mcdc_statement();
      if (!vcast_mcdc_statement)
        return condition;
      *vcast_mcdc_statement = *mcdc_statement;

      vcast_mcdc_data_ptr[statement] = 
         vcast_insert (vcast_mcdc_statement, vcast_mcdc_data_ptr[statement]);
   }

   mcdc_statement->mcdc_bits = 0;
   mcdc_statement->mcdc_bits_used = 0;

   return condition;
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_MCDC */

#ifdef VCAST_COVERAGE_TYPE_STATEMENT_BRANCH
VCAST_CONDITION_TYP 
VCAST_SAVE_BRANCH_STATEMENT_BRANCH_CONDITION_BUFFERED(
                            struct vcast_subprogram_coverage *coverage,
                            int statement,
                            VCAST_CONDITION_TYP condition)
{
   int vcast_index = (statement >> 3), vcast_shift = statement % 8;

   if (condition)
     (((struct vcast_STATEMENT_BRANCH_coverage*)coverage->coverage_ptr)->branch_coverage)->branch_bits_true[vcast_index] |= (VCAST_UNSIGNED_1 << vcast_shift);
   else
     (((struct vcast_STATEMENT_BRANCH_coverage*)coverage->coverage_ptr)->branch_coverage)->branch_bits_false[vcast_index] |= (VCAST_UNSIGNED_1 << vcast_shift);

   return condition;
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_BRANCH */
#endif /* VCAST_COVERAGE_IO_BUFFERED */

struct AVLNode
{
  VCAST_elementType element;
  AVLTree left;
  AVLTree right;
  int vcast_height;
};

#ifdef VCAST_USE_STATIC_MEMORY
#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
struct AVLNode vcast_avlnode_pool[VCAST_MAX_MCDC_STATEMENTS];
AVLTree vcast_avlnode_pool_ptr = vcast_avlnode_pool;
AVLTree vcast_avlnode_pool_end = vcast_avlnode_pool + VCAST_MAX_MCDC_STATEMENTS;
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */
#endif /* VCAST_USE_STATIC_MEMORY */

#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
AVLTree
vcast_getAVLTree (void)
{
  AVLTree ptr;
#ifdef VCAST_USE_STATIC_MEMORY
  if (vcast_avlnode_pool_ptr < vcast_avlnode_pool || vcast_avlnode_pool_ptr >= vcast_avlnode_pool_end)
    return 0;
  ptr = vcast_avlnode_pool_ptr++;
#else /* VCAST_USE_STATIC_MEMORY */
  ptr = (struct AVLNode *)VCAST_malloc (sizeof (struct AVLNode));
#endif /* VCAST_USE_STATIC_MEMORY */

  return ptr;
}

int vcast_find_val;
VCAST_position
vcast_find (VCAST_elementType VCAST_X, AVLTree VCAST_T)
{
  if (VCAST_T == 0)
    return 0;
  vcast_find_val = vcast_mcdc_statement_compare (VCAST_X, VCAST_T->element);
  if (vcast_find_val < 0)
    return vcast_find (VCAST_X, VCAST_T->left);
  else if (vcast_find_val > 0)
    return vcast_find (VCAST_X, VCAST_T->right);
  else
    return VCAST_T;
}

static int
vcast_height (VCAST_position P)
{
  if (P == 0)
    return -1;
  else
    return P->vcast_height;
}

static int
vcast_avl_max (int Lhs, int Rhs)
{
  return Lhs > Rhs ? Lhs : Rhs;
}

	/* This function can be called only if K2 has a left child */
	/* Perform a rotate between a node (K2) and its left child */
	/* Update heights, then return new root */

static VCAST_position
vcast_singleRotateWithLeft (VCAST_position K2)
{
  VCAST_position K1;

  K1 = K2->left;
  K2->left = K1->right;
  K1->right = K2;

  K2->vcast_height = vcast_avl_max (vcast_height (K2->left), vcast_height (K2->right)) + 1;
  K1->vcast_height = vcast_avl_max (vcast_height (K1->left), K2->vcast_height) + 1;

  return K1;			/* New root */
}

	/* This function can be called only if K1 has a right child */
	/* Perform a rotate between a node (K1) and its right child */
	/* Update heights, then return new root */

static VCAST_position
vcast_singleRotateWithRight (VCAST_position K1)
{
  VCAST_position K2;

  K2 = K1->right;
  K1->right = K2->left;
  K2->left = K1;

  K1->vcast_height = vcast_avl_max (vcast_height (K1->left), vcast_height (K1->right)) + 1;
  K2->vcast_height = vcast_avl_max (vcast_height (K2->right), K1->vcast_height) + 1;

  return K2;			/* New root */
}

	/* This function can be called only if K3 has a left */
	/* child and K3's left child has a right child */
	/* Do the left-right double rotation */
	/* Update heights, then return new root */

static VCAST_position
vcast_doubleRotateWithLeft (VCAST_position K3)
{
  /* Rotate between K1 and K2 */
  K3->left = vcast_singleRotateWithRight (K3->left);

  /* Rotate between K3 and K2 */
  return vcast_singleRotateWithLeft (K3);
}

	/* This function can be called only if K1 has a right */
	/* child and K1's right child has a left child */
	/* Do the right-left double rotation */
	/* Update heights, then return new root */

static VCAST_position
vcast_doubleRotateWithRight (VCAST_position K1)
{
  /* Rotate between K3 and K2 */
  K1->right = vcast_singleRotateWithLeft (K1->right);

  /* Rotate between K1 and K2 */
  return vcast_singleRotateWithRight (K1);
}

int vcast_insert_val;
AVLTree
vcast_insert (VCAST_elementType VCAST_X, AVLTree VCAST_T)
{
  if (VCAST_T == 0)
    {
      /* Create and return a one-node tree */
      VCAST_T = vcast_getAVLTree ();
      if (VCAST_T == 0)
      {
        if (vcast_max_mcdc_statements_exceeded == 0)
	  vcast_fatal_error ("VCAST_MAX_MCDC_STATEMENTS exceeded!");
        vcast_max_mcdc_statements_exceeded = 1;
      }
      else
	{
	  VCAST_T->element = VCAST_X;
	  VCAST_T->vcast_height = 0;
	  VCAST_T->left = VCAST_T->right = 0;
	}
    }
  else if ((vcast_insert_val = vcast_mcdc_statement_compare (VCAST_X, VCAST_T->element)) < 0)
    {
      VCAST_T->left = vcast_insert (VCAST_X, VCAST_T->left);
      if (vcast_height (VCAST_T->left) - vcast_height (VCAST_T->right) == 2)
	{
	  if (vcast_mcdc_statement_compare (VCAST_X, VCAST_T->left->element) < 0)
	    VCAST_T = vcast_singleRotateWithLeft (VCAST_T);
	  else
	    VCAST_T = vcast_doubleRotateWithLeft (VCAST_T);
	}
    }
  else if (vcast_insert_val > 0)
    {
      VCAST_T->right = vcast_insert (VCAST_X, VCAST_T->right);
      if (vcast_height (VCAST_T->right) - vcast_height (VCAST_T->left) == 2)
	{
	  if (vcast_mcdc_statement_compare (VCAST_X, VCAST_T->right->element) > 0)
	    VCAST_T = vcast_singleRotateWithRight (VCAST_T);
	  else
	    VCAST_T = vcast_doubleRotateWithRight (VCAST_T);
	}
    }
  /* Else X is in the tree already; we'll do nothing */

  VCAST_T->vcast_height = vcast_avl_max (vcast_height (VCAST_T->left), vcast_height (VCAST_T->right)) + 1;
  return VCAST_T;
}
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */

/* All the coverage data necessary to capture 100% coverage 
 * This currently only works as a static array, but if needs be,
 * this could be replaced with data from the heap instead.
 */
#if defined (VCAST_COVERAGE_TYPE_STATEMENT) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH)
#ifdef VCAST_USE_STATIC_MEMORY
struct vcast_statement_coverage VCAST_ATTRIBUTE_CODE vcast_statement_coverage_data_pool[VCAST_NUMBER_STATEMENT_STRUCTS];
int vcast_statement_coverage_data_pos = 0;
#endif /* VCAST_USE_STATIC_MEMORY */

struct vcast_statement_coverage *
vcast_statement_coverage_new()
{
#ifdef VCAST_USE_STATIC_MEMORY
  return &vcast_statement_coverage_data_pool[vcast_statement_coverage_data_pos++];
#else
  return (struct vcast_statement_coverage*)
    VCAST_malloc(sizeof(struct vcast_statement_coverage));
#endif
}
#endif /* defined (VCAST_COVERAGE_TYPE_STATEMENT) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH) */

#if defined (VCAST_COVERAGE_TYPE_BRANCH) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH)
#ifdef VCAST_USE_STATIC_MEMORY
struct vcast_branch_coverage VCAST_ATTRIBUTE_CODE vcast_branch_coverage_data_pool[VCAST_NUMBER_BRANCH_STRUCTS];
int vcast_branch_coverage_data_pos = 0;
#endif /* VCAST_USE_STATIC_MEMORY */

struct vcast_branch_coverage *
vcast_branch_coverage_new()
{
#ifdef VCAST_USE_STATIC_MEMORY
  return &vcast_branch_coverage_data_pool[vcast_branch_coverage_data_pos++];
#else
  return (struct vcast_branch_coverage*)
    VCAST_malloc(sizeof(struct vcast_branch_coverage));
#endif
}
#endif /* defined (VCAST_COVERAGE_TYPE_BRANCH) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH) */

#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
#ifdef VCAST_USE_STATIC_MEMORY
struct vcast_mcdc_coverage VCAST_ATTRIBUTE_CODE vcast_mcdc_coverage_data_pool[VCAST_NUMBER_MCDC_STRUCTS];
int vcast_mcdc_coverage_data_pos = 0;
AVLTree VCAST_ATTRIBUTE_CODE vcast_avltree_data_pool[VCAST_NUMBER_AVLTREE_POINTERS];
int vcast_avltree_data_pos = 0;
#endif /* VCAST_USE_STATIC_MEMORY */

struct vcast_mcdc_coverage *
vcast_mcdc_coverage_new()
{
#ifdef VCAST_USE_STATIC_MEMORY
  return &vcast_mcdc_coverage_data_pool[vcast_mcdc_coverage_data_pos++];
#else
  return (struct vcast_mcdc_coverage*)
    VCAST_malloc(sizeof(struct vcast_mcdc_coverage));
#endif
}

AVLTree *
vcast_avltree_new(int vcast_size)
{
#ifdef VCAST_USE_STATIC_MEMORY
  AVLTree *tree = &vcast_avltree_data_pool[vcast_avltree_data_pos];
  vcast_avltree_data_pos += vcast_size;
  return tree;
#else
  return (AVLTree *)VCAST_malloc(sizeof(AVLTree)*vcast_size);
#endif
}

#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */

#ifdef VCAST_COVERAGE_TYPE_STATEMENT_MCDC
#ifdef VCAST_USE_STATIC_MEMORY
struct vcast_STATEMENT_MCDC_coverage VCAST_ATTRIBUTE_CODE vcast_STATEMENT_MCDC_coverage_data_pool[VCAST_NUMBER_STATEMENT_MCDC_STRUCTS];
int vcast_STATEMENT_MCDC_coverage_data_pos = 0;
#endif /* VCAST_USE_STATIC_MEMORY */

struct vcast_STATEMENT_MCDC_coverage *
vcast_STATEMENT_MCDC_coverage_new()
{
#ifdef VCAST_USE_STATIC_MEMORY
  return &vcast_STATEMENT_MCDC_coverage_data_pool[vcast_STATEMENT_MCDC_coverage_data_pos++];
#else
  return (struct vcast_STATEMENT_MCDC_coverage*)
    VCAST_malloc(sizeof(struct vcast_STATEMENT_MCDC_coverage));
#endif
}

#endif /* VCAST_COVERAGE_TYPE_STATEMENT_MCDC */

#ifdef VCAST_COVERAGE_TYPE_STATEMENT_BRANCH
#ifdef VCAST_USE_STATIC_MEMORY
struct vcast_STATEMENT_BRANCH_coverage VCAST_ATTRIBUTE_CODE vcast_STATEMENT_BRANCH_coverage_data_pool[VCAST_NUMBER_STATEMENT_BRANCH_STRUCTS];
int vcast_STATEMENT_BRANCH_coverage_data_pos = 0;
#endif

struct vcast_STATEMENT_BRANCH_coverage *
vcast_STATEMENT_BRANCH_coverage_new()
{
#ifdef VCAST_USE_STATIC_MEMORY
  return &vcast_STATEMENT_BRANCH_coverage_data_pool[vcast_STATEMENT_BRANCH_coverage_data_pos++];
#else
  return (struct vcast_STATEMENT_BRANCH_coverage*)
    VCAST_malloc(sizeof(struct vcast_STATEMENT_BRANCH_coverage));
#endif
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_BRANCH */

#ifdef VCAST_USE_STATIC_MEMORY
struct vcast_subprogram_coverage VCAST_ATTRIBUTE_CODE vcast_subprogram_coverage_data_pool[VCAST_NUMBER_SUBPROGRAM_STRUCTS];
int vcast_subprogram_coverage_data_pos = 0;
#endif

struct vcast_subprogram_coverage *
vcast_subprogram_coverage_new()
{
#ifdef VCAST_USE_STATIC_MEMORY
  return &vcast_subprogram_coverage_data_pool[vcast_subprogram_coverage_data_pos++];
#else
  return (struct vcast_subprogram_coverage*)
    VCAST_malloc(sizeof(struct vcast_subprogram_coverage));
#endif
}

#if defined (VCAST_COVERAGE_TYPE_BRANCH) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH)
#ifdef VCAST_USE_STATIC_MEMORY
char VCAST_ATTRIBUTE_CODE vcast_binary_coverage_data_pool[VCAST_NUMBER_BINARY_BYTES];
int vcast_binary_coverage_data_pos = 0;
#endif /* VCAST_USE_STATIC_MEMORY */

char *
vcast_binary_coverage_new(int vcast_size)
{
#ifdef VCAST_USE_STATIC_MEMORY
  char *result = &vcast_binary_coverage_data_pool[vcast_binary_coverage_data_pos];
  vcast_binary_coverage_data_pos += vcast_size;
  return result;
#else
  return (char*)VCAST_malloc(sizeof(char)*vcast_size);
#endif
}
#endif /* defined (VCAST_COVERAGE_TYPE_BRANCH) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH) */

#ifdef VCAST_USE_STATIC_MEMORY
/* A staticall defined representation */
/* All the coverage data necessary to dump all covered subprograms */
struct vcast_subprogram_coverage VCAST_ATTRIBUTE_CODE *subprogram_coverage_pool[VCAST_NUMBER_SUBPROGRAM_STRUCTS];
int vcast_subprogram_coverage_pool_pos = 0;

#else /* VCAST_USE_STATIC_MEMORY */
struct vcast_subprog_list_ptr {
  struct vcast_subprogram_coverage *vcast_data;
  struct vcast_subprog_list_ptr *vcast_next;
} *vcast_subprog_root = 0, *vcast_subprog_cur = 0;

void VCAST_APPEND_SUBPROG_LIST_PTR (struct vcast_subprogram_coverage *coverage)
{
  struct vcast_subprog_list_ptr *vcast_new = (struct vcast_subprog_list_ptr*)
    VCAST_malloc (sizeof (struct vcast_subprog_list_ptr));
  vcast_new->vcast_data = coverage;
  vcast_new->vcast_next = 0;
  if (vcast_subprog_root)
  {
    vcast_subprog_cur->vcast_next = vcast_new;
    vcast_subprog_cur = vcast_subprog_cur->vcast_next;
  }
  else
  {
    vcast_subprog_root = vcast_new;
    vcast_subprog_cur = vcast_new;
  }
}
#endif /* VCAST_USE_STATIC_MEMORY */

void 
VCAST_REGISTER_SUBPROGRAM (struct vcast_subprogram_coverage *coverage)
{
#ifdef VCAST_DUMP_COVERAGE_AT_EXIT
  if (vcast_has_registered_atexit == 0) {
     vcast_register_atexit();
     vcast_has_registered_atexit = 1;
   }
#endif  /* VCAST_DUMP_COVERAGE_AT_EXIT */

#ifdef VCAST_USE_STATIC_MEMORY
  if (vcast_subprogram_coverage_pool_pos >= VCAST_MAX_COVERED_SUBPROGRAMS)
  {
    if (vcast_max_covered_subprograms_exceeded == 0)
      vcast_fatal_error ("VCAST_MAX_COVERED_SUBPROGRAMS exceeded!");
    vcast_max_covered_subprograms_exceeded = 1;
    return;
  }
  subprogram_coverage_pool[vcast_subprogram_coverage_pool_pos++] = coverage;
#else /* VCAST_USE_STATIC_MEMORY */
  VCAST_APPEND_SUBPROG_LIST_PTR (coverage);
#endif /* VCAST_USE_STATIC_MEMORY */
}

#ifndef VCAST_COVERAGE_IO_ANIMATION
#if defined VCAST_COVERAGE_TYPE_STATEMENT
void VCAST_REGISTER_STATEMENT (
   struct vcast_subprogram_coverage **cov, 
   char *vcast_bits_statement,
   short vcast_unit_id,
   short vcast_subprogram_id,
   short vcast_size)
{
  struct vcast_statement_coverage *statement_data;
  int vcast_num_bytes = (vcast_size%8 == 0)?vcast_size/8:vcast_size/8+1;
  int vcast_cur;

  for (vcast_cur = 0; vcast_cur < vcast_num_bytes; ++vcast_cur) {
     vcast_bits_statement[vcast_cur] = 0;
  }

  statement_data = vcast_statement_coverage_new();
  statement_data->coverage_bits = vcast_bits_statement;
  statement_data->vcast_num_statement_statements = vcast_size;
#if defined(VCAST_COVERAGE_IO_BUFFERED)
  statement_data->coverage_bits[0] |= 1;
#endif /* VCAST_COVERAGE_IO_BUFFERED */

  *cov = vcast_subprogram_coverage_new();
  (*cov)->vcast_unit_id = vcast_unit_id;
  (*cov)->vcast_subprogram_id = vcast_subprogram_id;
  (*cov)->coverage_kind = VCAST_COVERAGE_STATEMENT;
  (*cov)->coverage_ptr = statement_data;

  VCAST_REGISTER_SUBPROGRAM (*cov);
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT */

#if defined (VCAST_COVERAGE_TYPE_BRANCH)
void VCAST_REGISTER_BRANCH (
   struct vcast_subprogram_coverage **cov, 
   short vcast_unit_id,
   short vcast_subprogram_id,
   short vcast_size)
{
  int vcast_num_bytes, vcast_cur;
  struct vcast_branch_coverage *branch_data;

  vcast_num_bytes = (vcast_size%8 == 0)?vcast_size/8:vcast_size/8+1;

  branch_data = vcast_branch_coverage_new();
  branch_data->branch_bits_true = vcast_binary_coverage_new(vcast_num_bytes);
  branch_data->branch_bits_false = vcast_binary_coverage_new(vcast_num_bytes);

  for (vcast_cur = 0; vcast_cur < vcast_num_bytes; ++vcast_cur) {
     branch_data->branch_bits_true[vcast_cur] = 0;
     branch_data->branch_bits_false[vcast_cur] = 0;
  }

  branch_data->vcast_num_branch_statements = vcast_size;
#if defined(VCAST_COVERAGE_IO_BUFFERED)
  branch_data->branch_bits_true[0] |= 1;
#endif /* VCAST_COVERAGE_IO_BUFFERED */

  *cov = vcast_subprogram_coverage_new();
  (*cov)->vcast_unit_id = vcast_unit_id;
  (*cov)->vcast_subprogram_id = vcast_subprogram_id;
  (*cov)->coverage_kind = VCAST_COVERAGE_BRANCH;
  (*cov)->coverage_ptr = branch_data;

  VCAST_REGISTER_SUBPROGRAM (*cov);
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT */

#if defined (VCAST_COVERAGE_TYPE_MCDC)
void VCAST_REGISTER_MCDC (
   struct vcast_subprogram_coverage **cov, 
   short vcast_unit_id,
   short vcast_subprogram_id,
   short vcast_size)
{
  struct vcast_mcdc_coverage *mcdc_data;
  int vcast_cur;
#if defined(VCAST_COVERAGE_IO_BUFFERED)
  struct vcast_mcdc_statement mcdc_statement;
#endif

  mcdc_data = vcast_mcdc_coverage_new();
  mcdc_data->mcdc_data = vcast_avltree_new(vcast_size);
  mcdc_data->vcast_num_mcdc_statements = vcast_size;
  for (vcast_cur = 0; vcast_cur < vcast_size; ++vcast_cur)
    mcdc_data->mcdc_data[vcast_cur] = 0;

  *cov = vcast_subprogram_coverage_new();
  (*cov)->vcast_unit_id = vcast_unit_id;
  (*cov)->vcast_subprogram_id = vcast_subprogram_id;
  (*cov)->coverage_kind = VCAST_COVERAGE_MCDC;
  (*cov)->coverage_ptr = mcdc_data;

  VCAST_REGISTER_SUBPROGRAM (*cov);

#if defined(VCAST_COVERAGE_IO_BUFFERED)
  mcdc_statement.mcdc_bits = 1;
  mcdc_statement.mcdc_bits_used = 1;
  VCAST_SAVE_MCDC_CONDITION_BUFFERED(*cov, &mcdc_statement, 0, 1);
#endif /* VCAST_COVERAGE_IO_BUFFERED */
}
#endif /* VCAST_COVERAGE_TYPE_MCDC */

#if defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
void VCAST_REGISTER_STATEMENT_MCDC (
   struct vcast_subprogram_coverage **cov, 
   char *vcast_bits_statement,
   short vcast_unit_id,
   short vcast_subprogram_id,
   short vcast_size,
   short vcast_include_stmnt)
{
  struct vcast_statement_coverage *statement_data;
  struct vcast_mcdc_coverage *mcdc_data;
  struct vcast_STATEMENT_MCDC_coverage *SM_data;
  int vcast_cur;
  int vcast_num_bytes = (vcast_size%8 == 0)?vcast_size/8:vcast_size/8+1;
#if defined(VCAST_COVERAGE_IO_BUFFERED)
  struct vcast_mcdc_statement mcdc_statement;
#endif

  for (vcast_cur = 0; vcast_cur < vcast_num_bytes; ++vcast_cur) {
     vcast_bits_statement[vcast_cur] = 0;
  }

  statement_data = vcast_statement_coverage_new();
  statement_data->coverage_bits = vcast_bits_statement;
  statement_data->vcast_num_statement_statements = vcast_size;
#if defined(VCAST_COVERAGE_IO_BUFFERED)
  if (vcast_include_stmnt)
    statement_data->coverage_bits[0] |= 1;
#endif /* VCAST_COVERAGE_IO_BUFFERED */

  mcdc_data = vcast_mcdc_coverage_new();
  mcdc_data->mcdc_data = vcast_avltree_new(vcast_size);
  mcdc_data->vcast_num_mcdc_statements = vcast_size;
  for (vcast_cur = 0; vcast_cur < vcast_size; ++vcast_cur)
    mcdc_data->mcdc_data[vcast_cur] = 0;

  SM_data = vcast_STATEMENT_MCDC_coverage_new();
  SM_data->statement_coverage = statement_data;
  SM_data->mcdc_coverage = mcdc_data;

  *cov = vcast_subprogram_coverage_new();
  (*cov)->vcast_unit_id = vcast_unit_id;
  (*cov)->vcast_subprogram_id = vcast_subprogram_id;
  (*cov)->coverage_kind = VCAST_COVERAGE_STATEMENT_MCDC;
  (*cov)->coverage_ptr = SM_data;

  VCAST_REGISTER_SUBPROGRAM (*cov);

#if defined(VCAST_COVERAGE_IO_BUFFERED)
  mcdc_statement.mcdc_bits = 1;
  mcdc_statement.mcdc_bits_used = 1;
  VCAST_SAVE_MCDC_STATEMENT_MCDC_CONDITION_BUFFERED(*cov, &mcdc_statement, 0, 1);
#endif /* VCAST_COVERAGE_IO_BUFFERED */
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_MCDC */

#if defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH)
void VCAST_REGISTER_STATEMENT_BRANCH (
   struct vcast_subprogram_coverage **cov, 
   char *vcast_bits_statement,
   short vcast_unit_id,
   short vcast_subprogram_id,
   short vcast_size,
   short vcast_include_stmnt)
{
  int vcast_num_bytes, vcast_cur;
  struct vcast_statement_coverage *statement_data;
  struct vcast_branch_coverage *branch_data;
  struct vcast_STATEMENT_BRANCH_coverage *SB_data;

  vcast_num_bytes = (vcast_size%8 == 0)?vcast_size/8:vcast_size/8+1;
  for (vcast_cur = 0; vcast_cur < vcast_num_bytes; ++vcast_cur) {
     vcast_bits_statement[vcast_cur] = 0;
  }

  statement_data = vcast_statement_coverage_new();
  statement_data->coverage_bits = vcast_bits_statement;
  statement_data->vcast_num_statement_statements = vcast_size;
#if defined(VCAST_COVERAGE_IO_BUFFERED)
  if (vcast_include_stmnt)
    statement_data->coverage_bits[0] |= 1;
#endif /* VCAST_COVERAGE_IO_BUFFERED */

  branch_data = vcast_branch_coverage_new();
  branch_data->branch_bits_true = vcast_binary_coverage_new(vcast_num_bytes);
  branch_data->branch_bits_false = vcast_binary_coverage_new(vcast_num_bytes);

  for (vcast_cur = 0; vcast_cur < vcast_num_bytes; ++vcast_cur) {
     branch_data->branch_bits_true[vcast_cur] = 0;
     branch_data->branch_bits_false[vcast_cur] = 0;
  }

  branch_data->vcast_num_branch_statements = vcast_size;
#if defined(VCAST_COVERAGE_IO_BUFFERED)
  branch_data->branch_bits_true[0] |= 1;
#endif /* VCAST_COVERAGE_IO_BUFFERED */

  SB_data = vcast_STATEMENT_BRANCH_coverage_new();
  SB_data->statement_coverage = statement_data;
  SB_data->branch_coverage = branch_data;

  *cov = vcast_subprogram_coverage_new();
  (*cov)->vcast_unit_id = vcast_unit_id;
  (*cov)->vcast_subprogram_id = vcast_subprogram_id;
  (*cov)->coverage_kind = VCAST_COVERAGE_STATEMENT_BRANCH;
  (*cov)->coverage_ptr = SB_data;

  VCAST_REGISTER_SUBPROGRAM (*cov);
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_BRANCH */
#endif /* ndef VCAST_COVERAGE_IO_ANIMATION */

#ifdef VCAST_ENABLE_DATA_CLEAR_API
#if defined VCAST_COVERAGE_TYPE_STATEMENT
void VCAST_SUBPROGRAM_INIT_STATEMENT (
   struct vcast_subprogram_coverage *cov)
{
  struct vcast_statement_coverage *statement_data =
        (struct vcast_statement_coverage *)cov->coverage_ptr;
  statement_data->coverage_bits[0] |= 1;
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT */

#if defined (VCAST_COVERAGE_TYPE_BRANCH)
void VCAST_SUBPROGRAM_INIT_BRANCH (
  struct vcast_subprogram_coverage *cov)
{
  int vcast_num_bytes, vcast_cur;
  struct vcast_branch_coverage *branch_data =
        (struct vcast_branch_coverage *)cov->coverage_ptr;;
  branch_data->branch_bits_true[0] |= 1;
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT */

#if defined (VCAST_COVERAGE_TYPE_MCDC)
void VCAST_SUBPROGRAM_INIT_MCDC (
  struct vcast_subprogram_coverage *cov)
{
  struct vcast_mcdc_statement mcdc_statement;
  mcdc_statement.mcdc_bits = 1;
  mcdc_statement.mcdc_bits_used = 1;
  VCAST_SAVE_MCDC_CONDITION_BUFFERED(cov, &mcdc_statement, 0, 1);
}
#endif /* VCAST_COVERAGE_TYPE_MCDC */

#if defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
void VCAST_SUBPROGRAM_INIT_STATEMENT_MCDC (
   struct vcast_subprogram_coverage *cov,
   short vcast_include_stmnt)
{
  struct vcast_STATEMENT_MCDC_coverage *SM_data =
    (struct vcast_STATEMENT_MCDC_coverage *)cov->coverage_ptr;
  struct vcast_statement_coverage *statement_data =
    SM_data->statement_coverage; 
  struct vcast_mcdc_coverage *mcdc_data =
    SM_data->mcdc_coverage;
  struct vcast_mcdc_statement mcdc_statement;

  if (vcast_include_stmnt)
    statement_data->coverage_bits[0] |= 1;

  mcdc_statement.mcdc_bits = 1;
  mcdc_statement.mcdc_bits_used = 1;
  VCAST_SAVE_MCDC_STATEMENT_MCDC_CONDITION_BUFFERED(cov, &mcdc_statement, 0, 1);
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_MCDC */

#if defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH)
void VCAST_SUBPROGRAM_INIT_STATEMENT_BRANCH (
   struct vcast_subprogram_coverage *cov, 
   short vcast_include_stmnt)
{
  struct vcast_STATEMENT_BRANCH_coverage *SB_data =
    (struct vcast_STATEMENT_BRANCH_coverage *)cov->coverage_ptr;
  struct vcast_statement_coverage *statement_data =
    SB_data->statement_coverage;
  struct vcast_branch_coverage *branch_data =
    SB_data->branch_coverage;

  if (vcast_include_stmnt)
    statement_data->coverage_bits[0] |= 1;

  branch_data->branch_bits_true[0] |= 1;
}
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_BRANCH */
#endif /* VCAST_ENABLE_DATA_CLEAR_API */

#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
struct vcast_subprogram_coverage *vcast_dump_mcdc_coverage;
short vcast_dump_mcdc_statement;

void
VCAST_DUMP_MCDC_COVERAGE_DATA (AVLTree tree)
{
  /* Walk the tree, and output the data */
  if (tree)
  {
    VCAST_DUMP_MCDC_COVERAGE_DATA (tree->left);
    VCAST_WRITE_MCDC_DATA (
         vcast_dump_mcdc_coverage->vcast_unit_id, 
         vcast_dump_mcdc_coverage->vcast_subprogram_id, 
         vcast_dump_mcdc_statement,
         tree->element->mcdc_bits,
         tree->element->mcdc_bits_used);
    VCAST_DUMP_MCDC_COVERAGE_DATA (tree->right);
  }
}
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */

#if defined (VCAST_COVERAGE_TYPE_STATEMENT) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH)
void
VCAST_DUMP_STATEMENT_COVERAGE (struct vcast_subprogram_coverage *cur)
{
   int vcast_i, vcast_index, vcast_shift;
   struct vcast_statement_coverage *statement_coverage = 0;
   if (cur->coverage_kind == VCAST_COVERAGE_STATEMENT)
     statement_coverage = (struct vcast_statement_coverage*)cur->coverage_ptr;
   else if (cur->coverage_kind == VCAST_COVERAGE_STATEMENT_MCDC)
     statement_coverage = ((struct vcast_STATEMENT_MCDC_coverage*)cur->coverage_ptr)->statement_coverage;
   else if (cur->coverage_kind == VCAST_COVERAGE_STATEMENT_BRANCH)
     statement_coverage = ((struct vcast_STATEMENT_BRANCH_coverage*)cur->coverage_ptr)->statement_coverage;

   for (vcast_i = 0; 
        vcast_i < statement_coverage->vcast_num_statement_statements;
        ++vcast_i)
   {
     vcast_index = (vcast_i >> 3);
     vcast_shift = vcast_i%8;
     if (statement_coverage->coverage_bits[vcast_index] & (VCAST_UNSIGNED_1 << vcast_shift))
     {
       VCAST_WRITE_STATEMENT_DATA (cur->vcast_unit_id,
            cur->vcast_subprogram_id, vcast_i+1);
     }
   }
}
#endif /* defined (VCAST_COVERAGE_TYPE_STATEMENT) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH) */

#if defined (VCAST_COVERAGE_TYPE_BRANCH) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH)
void
VCAST_DUMP_BRANCH_COVERAGE (struct vcast_subprogram_coverage *cur)
{
   int vcast_i, vcast_index, vcast_shift;
   struct vcast_branch_coverage *branch_coverage = 0;

   if (cur->coverage_kind == VCAST_COVERAGE_BRANCH)
     branch_coverage = ((struct vcast_branch_coverage*)cur->coverage_ptr);
   else if (cur->coverage_kind == VCAST_COVERAGE_STATEMENT_BRANCH)
     branch_coverage = ((struct vcast_STATEMENT_BRANCH_coverage*)cur->coverage_ptr)->branch_coverage;

   for (vcast_i = 0; 
        vcast_i < branch_coverage->vcast_num_branch_statements;
        ++vcast_i)
   {
     vcast_index = (vcast_i >> 3);
     vcast_shift = vcast_i%8;
     if (branch_coverage->branch_bits_true[vcast_index] & (VCAST_UNSIGNED_1 << vcast_shift))
     {
       VCAST_WRITE_BRANCH_DATA (cur->vcast_unit_id, cur->vcast_subprogram_id,
         vcast_i, "T");
     }

     if (branch_coverage->branch_bits_false[vcast_index] & (VCAST_UNSIGNED_1 << vcast_shift))
     {
       VCAST_WRITE_BRANCH_DATA (cur->vcast_unit_id, cur->vcast_subprogram_id,
         vcast_i, "F");
     }
   }
}
#endif /* defined (VCAST_COVERAGE_TYPE_BRANCH) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH) */

#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC)

void
VCAST_DUMP_MCDC_COVERAGE (struct vcast_subprogram_coverage *cur)
{
   int vcast_i;
   struct vcast_mcdc_coverage *mcdc_coverage;

   if (cur->coverage_kind == VCAST_COVERAGE_MCDC)
     mcdc_coverage = (struct vcast_mcdc_coverage*) cur->coverage_ptr;
   else if (cur->coverage_kind == VCAST_COVERAGE_STATEMENT_MCDC)
     mcdc_coverage = ((struct vcast_STATEMENT_MCDC_coverage*) cur->coverage_ptr)->mcdc_coverage;
   
   for (vcast_i = 0; 
        vcast_i < mcdc_coverage->vcast_num_mcdc_statements; ++vcast_i)
   {
     vcast_dump_mcdc_coverage = cur;
     vcast_dump_mcdc_statement = vcast_i;
     VCAST_DUMP_MCDC_COVERAGE_DATA (mcdc_coverage->mcdc_data[vcast_i]);
   }
}
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */

void 
VCAST_DUMP_COVERAGE_DATA (void)
{
#if defined(VCAST_COVERAGE_IO_BUFFERED)
  struct vcast_subprogram_coverage *cur;
#ifdef VCAST_USE_STATIC_MEMORY
  int vcast_i;
  for (vcast_i = 0; vcast_i < vcast_subprogram_coverage_pool_pos; ++vcast_i)
  {
    cur = subprogram_coverage_pool[vcast_i];
#else /* VCAST_USE_STATIC_MEMORY */
  struct vcast_subprog_list_ptr *vcast_cur;
  for (vcast_cur = vcast_subprog_root; vcast_cur != 0; vcast_cur = vcast_cur->vcast_next)
  {
    cur = vcast_cur->vcast_data;
#endif /* VCAST_USE_STATIC_MEMORY */

    switch (cur->coverage_kind)
    {
      case VCAST_COVERAGE_STATEMENT:
#ifdef VCAST_COVERAGE_TYPE_STATEMENT
        VCAST_DUMP_STATEMENT_COVERAGE (cur);
#endif /* VCAST_COVERAGE_TYPE_STATEMENT */
        break;
      case VCAST_COVERAGE_BRANCH:
#ifdef VCAST_COVERAGE_TYPE_BRANCH
        VCAST_DUMP_BRANCH_COVERAGE (cur);
#endif /* VCAST_COVERAGE_TYPE_BRANCH */
        break;
      case VCAST_COVERAGE_MCDC:
#ifdef VCAST_COVERAGE_TYPE_MCDC
        VCAST_DUMP_MCDC_COVERAGE (cur);
#endif /* VCAST_COVERAGE_TYPE_MCDC */
        break;
      case VCAST_COVERAGE_STATEMENT_MCDC:
#ifdef VCAST_COVERAGE_TYPE_STATEMENT_MCDC
        VCAST_DUMP_STATEMENT_COVERAGE (cur);
        VCAST_DUMP_MCDC_COVERAGE (cur);
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_MCDC */
        break;
      case VCAST_COVERAGE_STATEMENT_BRANCH:
#ifdef VCAST_COVERAGE_TYPE_STATEMENT_BRANCH
        VCAST_DUMP_STATEMENT_COVERAGE (cur);
        VCAST_DUMP_BRANCH_COVERAGE (cur);
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_BRANCH */
        break;
    };
    
// #ifdef VCAST_DUMP_CALLBACK
    // VCAST_DUMP_CALLBACK ();
// #endif /* VCAST_DUMP_CALLBACK */
  }
#ifdef VCAST_DUMP_CALLBACK
    VCAST_DUMP_CALLBACK ();
#endif /* VCAST_DUMP_CALLBACK */

#ifdef VCAST_ENABLE_DATA_CLEAR_API
  VCAST_CLEAR_COVERAGE_DATA();
#endif

#ifndef VCAST_UNIT_TEST_TOOL
#ifndef VCAST_USE_STDOUT
  VCAST_CLOSE_FILE();
#endif
#endif
#endif /* VCAST_COVERAGE_IO_BUFFERED */
}

#if defined (VCAST_ENABLE_DATA_CLEAR_API)
#if defined (VCAST_COVERAGE_TYPE_STATEMENT) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH)
void
VCAST_CLEAR_STATEMENT_COVERAGE (struct vcast_subprogram_coverage *cur)
{
   int vcast_i, vcast_index;
   struct vcast_statement_coverage *statement_coverage = 0;
   int vcast_num_bytes, vcast_size;
   if (cur->coverage_kind == VCAST_COVERAGE_STATEMENT)
     statement_coverage = (struct vcast_statement_coverage*)cur->coverage_ptr;
   else if (cur->coverage_kind == VCAST_COVERAGE_STATEMENT_MCDC)
     statement_coverage = ((struct vcast_STATEMENT_MCDC_coverage*)cur->coverage_ptr)->statement_coverage;
   else if (cur->coverage_kind == VCAST_COVERAGE_STATEMENT_BRANCH)
     statement_coverage = ((struct vcast_STATEMENT_BRANCH_coverage*)cur->coverage_ptr)->statement_coverage;

   vcast_size = statement_coverage->vcast_num_statement_statements;
   vcast_num_bytes = (vcast_size%8 == 0)?vcast_size/8:vcast_size/8+1;

   for (vcast_i = 0; vcast_i < vcast_num_bytes; ++vcast_i)
   {
     statement_coverage->coverage_bits[vcast_i] = 0;
   }
}
#endif /* defined (VCAST_COVERAGE_TYPE_STATEMENT) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH) */


#if defined (VCAST_COVERAGE_TYPE_BRANCH) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH)
void
VCAST_CLEAR_BRANCH_COVERAGE (struct vcast_subprogram_coverage *cur)
{
   int vcast_i, vcast_index;
   struct vcast_branch_coverage *branch_coverage = 0;
   int vcast_num_bytes, vcast_size;

   if (cur->coverage_kind == VCAST_COVERAGE_BRANCH)
     branch_coverage = ((struct vcast_branch_coverage*)cur->coverage_ptr);
   else if (cur->coverage_kind == VCAST_COVERAGE_STATEMENT_BRANCH)
     branch_coverage = ((struct vcast_STATEMENT_BRANCH_coverage*)cur->coverage_ptr)->branch_coverage;

   vcast_size = branch_coverage->vcast_num_branch_statements;
   vcast_num_bytes = (vcast_size%8 == 0)?vcast_size/8:vcast_size/8+1;

   for (vcast_i = 0; vcast_i < vcast_num_bytes; ++vcast_i)
   {
     branch_coverage->branch_bits_true[vcast_i] = 0;
     branch_coverage->branch_bits_false[vcast_i] = 0;
   }
}
#endif /* defined (VCAST_COVERAGE_TYPE_BRANCH) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH) */

#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC)

#ifndef VCAST_USE_STATIC_MEMORY
void
VCAST_CLEAR_MCDC_COVERAGE_DATA (AVLTree tree)
{
  /* Walk the tree, and delete it */
  if (tree)
  {
    VCAST_CLEAR_MCDC_COVERAGE_DATA(tree->left);
    VCAST_CLEAR_MCDC_COVERAGE_DATA(tree->right);
    VCAST_free(tree);
  }
}
#endif

void
VCAST_CLEAR_MCDC_COVERAGE (struct vcast_subprogram_coverage *cur)
{
   int vcast_i;
   struct vcast_mcdc_coverage *mcdc_coverage;

   if (cur->coverage_kind == VCAST_COVERAGE_MCDC)
     mcdc_coverage = (struct vcast_mcdc_coverage*) cur->coverage_ptr;
   else if (cur->coverage_kind == VCAST_COVERAGE_STATEMENT_MCDC)
     mcdc_coverage = ((struct vcast_STATEMENT_MCDC_coverage*) cur->coverage_ptr)->mcdc_coverage;
   
   for (vcast_i = 0; 
        vcast_i < mcdc_coverage->vcast_num_mcdc_statements; ++vcast_i)
   {
#ifndef VCAST_USE_STATIC_MEMORY
       VCAST_CLEAR_MCDC_COVERAGE_DATA(mcdc_coverage->mcdc_data[vcast_i]);
#endif
       mcdc_coverage->mcdc_data[vcast_i] = 0;
   }
}
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */

void 
VCAST_CLEAR_COVERAGE_DATA(void)
{
#if defined(VCAST_COVERAGE_IO_BUFFERED)
  struct vcast_subprogram_coverage *cur;
#ifdef VCAST_USE_STATIC_MEMORY
  int vcast_i;
  for (vcast_i = 0; vcast_i < vcast_subprogram_coverage_pool_pos; ++vcast_i)
  {
    cur = subprogram_coverage_pool[vcast_i];
#else /* VCAST_USE_STATIC_MEMORY */
  struct vcast_subprog_list_ptr *vcast_cur;
  for (vcast_cur = vcast_subprog_root; vcast_cur != 0; vcast_cur = vcast_cur->vcast_next)
  {
    cur = vcast_cur->vcast_data;
#endif /* VCAST_USE_STATIC_MEMORY */

    switch (cur->coverage_kind)
    {
      case VCAST_COVERAGE_STATEMENT:
#ifdef VCAST_COVERAGE_TYPE_STATEMENT
        VCAST_CLEAR_STATEMENT_COVERAGE (cur);
#endif /* VCAST_COVERAGE_TYPE_STATEMENT */
        break;
      case VCAST_COVERAGE_BRANCH:
#ifdef VCAST_COVERAGE_TYPE_BRANCH
        VCAST_CLEAR_BRANCH_COVERAGE (cur);
#endif /* VCAST_COVERAGE_TYPE_BRANCH */
        break;
      case VCAST_COVERAGE_MCDC:
#ifdef VCAST_COVERAGE_TYPE_MCDC
        VCAST_CLEAR_MCDC_COVERAGE (cur);
#endif /* VCAST_COVERAGE_TYPE_MCDC */
        break;
      case VCAST_COVERAGE_STATEMENT_MCDC:
#ifdef VCAST_COVERAGE_TYPE_STATEMENT_MCDC
        VCAST_CLEAR_STATEMENT_COVERAGE (cur);
        VCAST_CLEAR_MCDC_COVERAGE (cur);
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_MCDC */
        break;
      case VCAST_COVERAGE_STATEMENT_BRANCH:
#ifdef VCAST_COVERAGE_TYPE_STATEMENT_BRANCH
        VCAST_CLEAR_STATEMENT_COVERAGE (cur);
        VCAST_CLEAR_BRANCH_COVERAGE (cur);
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_BRANCH */
        break;
    };
  }
#endif /* VCAST_COVERAGE_IO_BUFFERED */

#ifdef VCAST_USE_BUFFERED_ASCII_DATA
  vcast_ascii_coverage_data_pos = 0;
  vcast_coverage_data_buffer_full = 0;
#endif

#ifndef VCAST_UNIT_TEST_TOOL
#ifndef VCAST_USE_STDOUT
  VCAST_CLOSE_FILE();
#endif
#endif

#ifdef VCAST_USE_STATIC_MEMORY
#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
   vcast_mcdc_statement_pool_ptr = vcast_mcdc_statement_pool;
   vcast_avlnode_pool_ptr = vcast_avlnode_pool;
#endif
#endif
}
#endif /* VCAST_ENABLE_DATA_CLEAR_API */

void
VCAST_INITIALIZE (void)
{
#ifdef VCAST_USE_BUFFERED_ASCII_DATA
  vcast_ascii_coverage_data_pos = 0;
  vcast_coverage_data_buffer_full = 0;
  VCAST_MAX_CAPTURED_ASCII_DATA_EXCEEDED_ERROR = "\nVCAST_MAX_CAPTURED_ASCII_DATA_EXCEEDED";
  vcast_max_captured_ascii_data_exceeded_error_size = 39;
#endif /* VCAST_USE_BUFFERED_ASCII_DATA */
  
#ifndef VCAST_USE_STDOUT
#ifndef VCAST_UNIT_TEST_TOOL
#ifdef VCAST_USE_STDIO_OPS
  vCAST_INST_FILE = 0;
#endif /* VCAST_USE_STDIO_OPS */
#endif /* VCAST_UNIT_TEST_TOOL */
#endif /* VCAST_USE_STDOUT */
  
#ifdef VCAST_USE_STATIC_MEMORY
#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
  vcast_mcdc_statement_pool_ptr = vcast_mcdc_statement_pool;
  vcast_mcdc_statement_end = vcast_mcdc_statement_pool + VCAST_MAX_MCDC_STATEMENTS;
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */
  vcast_max_covered_subprograms_exceeded = 0;
#endif /* VCAST_USE_STATIC_MEMORY */
  
#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
  vcast_max_mcdc_statements_exceeded = 0;
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */
  
#ifdef VCAST_USE_STATIC_MEMORY
#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
  vcast_avlnode_pool_ptr = vcast_avlnode_pool;
  vcast_avlnode_pool_end = vcast_avlnode_pool + VCAST_MAX_MCDC_STATEMENTS;
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */
#endif /* VCAST_USE_STATIC_MEMORY */
  
#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
  vcast_find_val = 0;
  vcast_insert_val = 0;
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined(VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */
  
#if defined (VCAST_COVERAGE_TYPE_STATEMENT) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH)
#ifdef VCAST_USE_STATIC_MEMORY
  vcast_statement_coverage_data_pos = 0;
#endif /* VCAST_USE_STATIC_MEMORY */
#endif /* defined (VCAST_COVERAGE_TYPE_STATEMENT) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH) */
  
#if defined (VCAST_COVERAGE_TYPE_BRANCH) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH)
#ifdef VCAST_USE_STATIC_MEMORY
  vcast_branch_coverage_data_pos = 0;
#endif /* VCAST_USE_STATIC_MEMORY */
#endif /* defined (VCAST_COVERAGE_TYPE_BRANCH) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH) */
  
#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
#ifdef VCAST_USE_STATIC_MEMORY
  vcast_mcdc_coverage_data_pos = 0;
  vcast_avltree_data_pos = 0;
#endif /* VCAST_USE_STATIC_MEMORY */
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */
  
#ifdef VCAST_COVERAGE_TYPE_STATEMENT_MCDC
#ifdef VCAST_USE_STATIC_MEMORY
  vcast_STATEMENT_MCDC_coverage_data_pos = 0;
#endif /* VCAST_USE_STATIC_MEMORY */
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_MCDC */
  
#ifdef VCAST_COVERAGE_TYPE_STATEMENT_BRANCH
#ifdef VCAST_USE_STATIC_MEMORY
  vcast_STATEMENT_BRANCH_coverage_data_pos = 0;
#endif /* VCAST_USE_STATIC_MEMORY */
#endif /* VCAST_COVERAGE_TYPE_STATEMENT_BRANCH */
  
#ifdef VCAST_USE_STATIC_MEMORY
  vcast_subprogram_coverage_data_pos = 0;
#endif /* VCAST_USE_STATIC_MEMORY */
  
#if defined (VCAST_COVERAGE_TYPE_BRANCH) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH)
#ifdef VCAST_USE_STATIC_MEMORY
  vcast_binary_coverage_data_pos = 0;
#endif /* VCAST_USE_STATIC_MEMORY */
#endif /* defined (VCAST_COVERAGE_TYPE_BRANCH) || defined (VCAST_COVERAGE_TYPE_STATEMENT_BRANCH) */
  
#ifdef VCAST_USE_STATIC_MEMORY
  /* A staticall defined representation */
  /* All the coverage data necessary to dump all covered subprograms */
  vcast_subprogram_coverage_pool_pos = 0;
#else /* VCAST_USE_STATIC_MEMORY */
  vcast_subprog_root = 0;
  vcast_subprog_cur = 0;
#endif /* VCAST_USE_STATIC_MEMORY */
  
#if defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC)
  vcast_dump_mcdc_statement = 0;
#endif /* defined (VCAST_COVERAGE_TYPE_MCDC) || defined (VCAST_COVERAGE_TYPE_STATEMENT_MCDC) */
}



