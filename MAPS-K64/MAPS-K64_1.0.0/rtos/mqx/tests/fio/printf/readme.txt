About printf test:
    Printf is fio test for correct printing decimal and float numbers.
    Principle of this test is redirecting STDOUT to a pipe and then compare data from a pipe with a reference.

    This test uses reference file test_compare.h. This file was created with gcc. 

Create reference:
    test_gcc.c: Source file used for generating reference file test_compare.h and placed in a folder gcc_ref.
                This file use the same configuration header file (test.h) as our test.
                You have to redirect output from a console to a file named test_compare.h, or copy/paste output from a console and create a file called test_compare.h.
                You can use this source file with another compiler either.
    Cygwin gcc example: 
        $ cd /cygdrive/<drive>/<path to yoyr view>/MSGSW/MQX/src/tests/fio/printf/gcc_ref/
        $ rm ../test_compare.h
        $ gcc test_gcc.c
        $ ./a.exe >> ../test_compare.h
