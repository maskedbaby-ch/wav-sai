MCC Test Step:
1. Configure your project
For a5 as primary core, a5 as sender,m4 as responder,so need to add "MCC_TEST_APP=1" into a5 project->Options->C/C++ Compiler->Preprocessor-->Defined symbols:(one per line).Similarly, need to add "MCC_TEST_APP=0" into m4 project's Options.
For m4 as primary core, m4 as sender, a5 as responder, similarly, need to add "MCC_TEST_APP=1" into m4 project's Options and add "MCC_TEST_APP=0" into a5 project's Options.
2. Run case
Open two IDE for test,one for a5 project and another for m4 project.For a5 as primary core, need to run a5 projec firstly, then run m4 project.For m4 as primary core, run m4 project firstly and then run a5 project.
3.Test result
pingpong expected results : Fialed count =0
pingpong1 expected results: Failed count =0(but owing to have a bug that can not be fixed,so Failed count will be 1)
