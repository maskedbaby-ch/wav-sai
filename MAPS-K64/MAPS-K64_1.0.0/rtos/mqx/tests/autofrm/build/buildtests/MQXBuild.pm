package MQXBuild;

use strict;
use vars qw( @ISA @EXPORT @EXPORT_OK );

require Exporter;

@ISA = qw( Exporter AutoLoader );

@EXPORT = qw( build_iar
              build_cw10
			  build_cwclassic );

BEGIN
{
}

################################################################################
# Builds with IAR                                                              #
################################################################################

sub build_iar
{
    my $build_list = shift;
    my @iar_targets = ( "Debug", "Release", "Int Flash Debug",  "Int Flash Release" );

    foreach my $projecttoopen (@$build_list)
    {       
        my $allerrors = 0;
        my $at_leas_one_target_valid = 0;
        
        foreach my $iar_target (@iar_targets)
        {
            printboth ("Building $projecttoopen\n");

            system("$CompillerExec \"$projecttoopen\" -make  \"$iar_target\" > temp.txt");
            
            #browse compilation log for errors
            open TEMP,"<temp.txt"; 
            @temp = <TEMP>;
            close TEMP;
            
            @error_line = grep /Total number of errors:/, @temp;
            
            if (grep /Configuration is up-to-date/,@temp)
            {
                $at_leas_one_target_valid = 1;
                printlog  ("$iar_target - Up to date\n");   
            }
            
            if ($error_line[0] =~ /Total number of errors: (\d*)/)
            {
              $allerrors = $allerrors + $1;
              $at_leas_one_target_valid = 1;
              printlog  ("$iar_target - number of errors: $1\n");                 
            }

            if ($allerrors > 0)
            {
                $projecttoopen =~ /.?([\w\d]*)\.ewp/;
                printerr  ("$iar_target - number of errors: $allerrors\n");
                system("copy \/Y temp.txt \"$logdir\\$1\_$iar_target.txt\"");
            }
        }
      
        #if there was at least one target valid
        if($at_leas_one_target_valid == 0)
        {
            printboth  ("THERE IS NO TARGET VALID IN THE PROJECT!\n");
            $allerrors = $allerrors + 1;
        }
      
        if ($allerrors > 0)
        {
          $text = sprintf "%-60s\t\tERROR - $allerrors\n",$PROJ_LIST[$ii];
          printrep($text);
          $sumerrors = $sumerrors + $allerrors;
          $allerrors = 0
        }
        else
        {
          $text = sprintf "%-60s\t\tOK\n",$PROJ_LIST[$ii];
          printrep($text);
        }
        printboth ("----------------------------------\n");
    }

    return 
}

################################################################################
# Builds with CW 10                                                            #
################################################################################

# sub build_cw10
# {
    # my $build_list = shift;

    # system ("copy /Y cw10.bat  \"%PROGRAMFILES%/Freescale/CW MCU v10.1/eclipse\"");

    # $workspace = `echo %USERPROFILE%\\workspace`;
    
    # print "\n";
    
    # foreach $projecttoopen (@BUILD_LIST)
    # {
        # print "-- deleting old workspace: $workspace\n";
        elete old workspace
        # `rd /S /Q "$workspace"`;
            
        # printboth ("Project: @$build_list[$ii]\n"); 

        # $programFiles = `echo %PROGRAMFILES%`;
        # $programFiles =~ s/\n//g;
        # $programFiles =~ s/\r//g;
            
        # chdir $programFiles."\\Freescale\\CW MCU v10.1\\eclipse\\";

        # printlog ("building\n");      
        # system("$CompillerExec build -project \"$projecttoopen\" -allConfigs > \"$CUR_DIR\\temp.txt\"" );
        # sleep(10);
        # chdir $CUR_DIR;
        
        rowse compilation log for errors
        # open TEMP,"<temp.txt"; @temp = <TEMP>;  close TEMP;
        
        # if (grep /Build succeeded for project/,@temp)
        # {
            # printlog  ("Build succeeded.\n");   
        # }
        # else
        # {
            # $allerrors = $allerrors + 1;
        # }
        
        # if (grep /Build failed for project/, @temp)
        # {
          # $allerrors = $allerrors + 1;
          # printlog  ("Build failed.\n");                  
        # }
        # if ($allerrors > 0)
        # {
            # $projecttoopen =~ /.?([\w\d]*)\\\.project/;
            # printerr  ("Number of errors: $allerrors\n");
            # system("copy \/Y temp.txt \"$logdir\\$1\.txt\"");
            
        # }     
        # if ($allerrors > 0)
        # {
          # $text = sprintf "%-60s\t\tERROR - $allerrors\n",$PROJ_LIST[$ii];
          # printrep($text);
          # $sumerrors = $sumerrors + $allerrors;
          # $allerrors = 0
        # }
        # else
        # {
          # $text = sprintf "%-60s\t\tOK\n",$PROJ_LIST[$ii];
          # printrep($text);
        # }
        # $ii = $ii + 1;
    # }
    
    # printboth ("----------------------------------\n");   
# }

################################################################################
# Builds with CW Classic                                                       #
################################################################################
# sub build_cwclassic
# {
    # my $build_list = shift;

    # print "Opening Compiller: $CompillerExec and waiting 15 seconds\n";

    # system("$CompillerExec");
    # $CompillerExec =~ /^(.*)\.bat$/;
    # $codeWarriorName = $1;
    # sleep(15);
    
    # $ICodeWarriorApp = new Win32::OLE("CodeWarrior.CodeWarriorApp") or die "Error: Couldn't access CodeWarrior object: $!\n";
    
    Traverse all projects and build them
    # foreach $projecttoopen ( @$build_list )
    # {
      # $allerrors = 0;  
      # if (!($projecttoopen =~ /.*\\$codeWarriorName\\*[^\\]*$/))
      # {
          # printboth("Cannon build in $codeWarriorName : $projecttoopen\n");
          # $text = sprintf "%-60s\tSKIPPED\n",$PROJ_LIST[$ii];
          # printrep($text);
          # printboth ("----------------------------------\n");
          # next;
      # }
    
      Open Project
      # printboth ("Project: $PROJ_LIST[$ii]\n");
      # $ICodeWarriorProject = $ICodeWarriorApp->OpenProject("$projecttoopen", true, 0, 0) or 
           # printboth ("!!!! Error: Couldn't access CodeWarrior project $projecttoopen \n----------------------------------\n") and
           # next;
    
      # $ICodeWarriorTargetCol = $ICodeWarriorProject->Targets();
      # $Count = $ICodeWarriorTargetCol->Count;
      # for ($index = 0; $index < $Count; $index++)
      # {  
        # $ICodeWarriorTarget = $ICodeWarriorTargetCol->Item($index);
        # $ICodeWarriorProject->SetCurrentTarget($ICodeWarriorTarget->Name());
    
        # if ($filter ne "")
        # {
          # if ($ICodeWarriorTarget->Name() =~ /.*$filter.*/)
          # {
        # printlog ("Building Target:".$ICodeWarriorTarget->Name()."\n");  
          # }
          # else
          # {
        # printlog ("Skipping Target:".$ICodeWarriorTarget->Name()."\n");
        # next;
          # }
        # }
    
        Clean
        # printlog ("cleaning\n");
        # $ICodeWarriorProject->RemoveObjectCode(0, 1);
        
        Build
        # printlog ("building\n");
        # $messages = $ICodeWarriorProject->BuildAndWaitToComplete();
        
        Create report
        # $errors = $messages->Errors();
        # $numerrors = $messages->ErrorCount();
        # $warnings = $messages->Warnings();
        # $numwarnings = $messages->WarningCount();

        # printlog  ("Number of Errors: $numerrors\n");
        # if ($numerrors > 0)
        # {
          # printerr  ("Number of Errors: $numerrors\n");
          # $allerrors = $allerrors + $numerrors;
        # }
        # for ($i = 0; $i < $numerrors; $i++)
        # {
          # $errortoprint = $errors->Item($i);
          # $errornum = $errortoprint->ErrorNumber();
          # $stringtoprint = $errortoprint->MessageText();
          # printboth("$errornum: $stringtoprint\n");
        # }

        # printlog ("\nNumber of Warnings: $numwarnings\n");
        # if ($numwarnings > 0)
        # {
          # printboth  ("Number of Warnings: $numwarnings\n");
        # }    

        # for ($i = 0; $i < $numwarnings; $i++)
        # {
          # $warningtoprint = $warnings->Item($i);
          # $warningnum = $warningtoprint->ErrorNumber();
          # $stringtoprint = $warningtoprint->MessageText();
          # printboth("$warningnum: $stringtoprint\n");
        # }
        # printboth ("----------------------------------\n");
      # }  # endo of browse all targets
      
      # if ($allerrors > 0)
      # {
        # $text = sprintf "%-60s\t\tERROR - $allerrors\n",$PROJ_LIST[$ii];
        # printrep($text);
        # $sumerrors = $sumerrors + $allerrors;
        # $allerrors = 0
      # }
      # else
      # {
        # $text = sprintf "%-60s\t\tOK\n",$PROJ_LIST[$ii];        
        # printrep($text);
      # }
      # $ii = $ii + 1;
      lose project
      # $ICodeWarriorProject->Close();
      # printboth ("----------------------------------\n");
    # }
# $ICodeWarriorApp->Quit(0);
# }

1
