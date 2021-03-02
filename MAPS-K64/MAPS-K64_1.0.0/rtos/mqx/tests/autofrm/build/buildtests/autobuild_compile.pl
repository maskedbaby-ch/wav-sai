#!/usr/bin/perl

use strict;
use MQXCruiseXML;
use MQXUtils;
use Time::HiRes qw/gettimeofday/;
use Win32;
use Win32::OLE;

my $config = get_cfg();

# parameters 
#
#   $project  = path to project
#   $compiler = [cwmcu63|cwcf72|cw88|cw10|iar]
#   $skipping = should script skip building the rest of the projects if
#               one of their libraries failed to compile
#
my $project;
my $compiler;
my $skipping = 1;

while( my $par = shift @ARGV )
{
    if( $par =~ /-p/ )
    {
        $project = shift @ARGV;
    }
    elsif( $par =~ /-c/ )
    {
        $compiler = shift @ARGV;
    }
    elsif( $par =~ /-noskip/ )
    {
        $skipping = 0;
    }
    else
    {
        die "Wrong command line parameters";
    }
}

# important projects must compile without errors
my $important_project = 0;

if( $skipping == 1 )
{
    if( $project =~ /psp_/ )
    {
        $important_project = 1;
    }
    elsif( $project =~ /bsp_/ )
    {
        $important_project = 1;
    }
    elsif( $project =~ /mfs_/ )
    {
        $important_project = 1;
    }
    elsif( $project =~ /usb_hdk_/ )
    {
        $important_project = 1;
    }
    elsif( $project =~ /usb_ddk_/ )
    {
        $important_project = 1;
    }
    elsif( $project =~ /shell_/ )
    {
        $important_project = 1;
    }
}

################################################################################

$project =~ /.*[\\\/]([^\\\/]+)/;
my $project_name = $1;

my $time_begin   = gettimeofday();

my $projects_cnt_all = 0;
my $errors_cnt_all   = 0;
my $projects_cnt     = 0; # number of compilations( projects*targets )
my $errors_cnt       = 0; # number of compilations which failed

my @tc = ();              # test cases, xml code

################################################################################

# get previous suites from file
my @old_content = ();
if( open FILEIN, '<', "build_logs\\".$compiler.".xml" )
{
    while(<FILEIN>)
	{
	    if( /<\?xml/ )            {}
		elsif( /<testsuites>/ )   {}
		elsif( /<\/testsuites>/ ) {}
		elsif( /testsummary  failures="(\d+)" tests="(\d+)" / )
		{
		    $errors_cnt_all   = $1;
    		$projects_cnt_all = $2;
	    }
		elsif( /<!-- begin time: (\d+\.\d+) -->/ )
		{
		    $time_begin = $1;
		}
		elsif( /<!-- failure of important project -->/ )
		{
		    exit;
		}
		else
		{
	        push( @old_content, $_ );
		}
	}
}

# print header + previous suites
open FILEOUT, '>', "build_logs\\".$compiler.".xml";
print FILEOUT join( "\n", report_header() ) . "\n";
print FILEOUT join( "", @old_content );

 my $compilerexec = $config->{uc($compiler)};

if( $compiler =~ /cwmcu63/ || $compiler =~ /cwcf72/ || $compiler =~ /cw88/ )
{
    print "Working on $compiler - $project_name\n";
	
    print "Opening compiler: $compilerexec and waiting 15 seconds\n";
	
	system("$compilerexec");
	
	sleep(15);
	
	my $ICodeWarriorApp = new Win32::OLE("CodeWarrior.CodeWarriorApp") or
	    die "Error: Couldn't access CodeWarrior object: $!\n";
	
	my $ICodeWarriorProject = $ICodeWarriorApp->OpenProject("$project", "true", 0, 0) or 
	    die "!!!! Error: Couldn't access CodeWarrior project $project \n";	
	
	my $ICodeWarriorTargetCol = $ICodeWarriorProject->Targets();
	
	my @test_cases = ();
	
	for( my $i = 0; $i < $ICodeWarriorTargetCol->Count; $i++ )
	{
	    print "Processing target: ".$ICodeWarriorTargetCol->Item($i)->Name()."\n";
	    $ICodeWarriorProject->SetCurrentTarget($ICodeWarriorTargetCol->Item($i)->Name());
		print "Cleaning...\n";
	    $ICodeWarriorProject->RemoveObjectCode(0, 1);
	    print "Building...\n";
	    my $messages = $ICodeWarriorProject->BuildAndWaitToComplete();
		
		#$messages->Errors();
	    #$messages->ErrorCount();
	    #$messages->Warnings();
	    #$messages->WarningCount();
		
		#"build_logs\\iar_".$project_name."_".$iart_fn.".log"
		
		my $elapsed_time = gettimeofday() - $time_begin;
		my $ecnt = $messages->ErrorCount();
		if( $ecnt > 0 )
		{
			my $e    = $messages->ErrorCount();
			my $w    = $messages->WarningCount();
			my $text = "";
			my $ovfl = 0;
			
		    for( my $j = 0; $j < $ecnt; $j++ )
	        {
			    my $errortoprint = $messages->Errors()->Item($j);                
				
				if( $errortoprint->MessageText() =~ /Overflow in segment/ )
				{
				    $ovfl = 1;
				    push( @tc, test_case_success("buildtests", $ICodeWarriorTargetCol->Item($i)->Name(), $elapsed_time, "Compilation succesfull, but with overflow") );    
				    last;
				}
				
				$text .= "<![CDATA[";
                $text .= "Error $j: code ". $errortoprint->ErrorNumber() ."\n"; 				
				$text .= $errortoprint->MessageText()."\n\n";
				$text .= "]]>";                				
		    }			
			
			if( $ovfl == 0 )
			{
		        push( @tc, test_case_failure("buildtests", $ICodeWarriorTargetCol->Item($i)->Name(), $elapsed_time, "build failure", "Number of Errors: $e\nNumber of Warnings: $w\n".$text  ) );
			    $errors_cnt++;
			}
		}
		elsif( $ecnt == 0 )
		{
		    push( @tc, test_case_success("buildtests", $ICodeWarriorTargetCol->Item($i)->Name(), $elapsed_time, "Compilation succesfull") );    
		}
		else
		{
		    push( @tc, test_case_failure("buildtests", $ICodeWarriorTargetCol->Item($i)->Name(), $elapsed_time, "build failure", "This shouldn't happen") );
			$errors_cnt++;		    
		}
        
		$projects_cnt++;		    
	}
	
	#close project
	$ICodeWarriorProject->Close();
    $ICodeWarriorApp->Quit(0);	
}
elsif( $compiler =~ /cw10/ )
{
    $project =~ /[\/\\]([^\/\\]+)[\/\\][^\/\\]+$/;
	$project_name = $1;
	
    print "Working on $compiler - $project_name\n";
    
	#system ("copy /Y cw10.bat  \"%PROGRAMFILES%/Freescale/CW MCU v10.1/eclipse\"");
	
	my $progfiles = `echo %PROGRAMFILES%`;
	my $workspace = `echo %USERPROFILE%\\workspace`;
			
	#delete old workspace
	#`rd /S /Q "$workspace"`;
        
	$progfiles =~ s/\n//g;
	$progfiles =~ s/\r//g;

    my $currdir = $config->{'BUILDTEST_DIR'};
    $compilerexec =~ /(.*)[\\\/]([^\\\/]+)/;
    my $cw10dir = $1; 

    chdir($cw10dir);    
	system("$compilerexec build -project \"$project\" -allConfigs >  $currdir\\build_logs\\cw10_".$project_name.".log" );	
    chdir($currdir);
	
    sleep(10);
		
	#browse compilation log for errors
	open TEMP, '<', "build_logs\\cw10_".$project_name.".log"; 
	
	my $e = 0;
	my $errors;
	
	my %err_text;
	my %err_cnt;
	my %tmp_tc;
	
	while(<TEMP>)
	{
	    if( /^Error:/ )
		{
		    $errors .= "<![CDATA[";
            $errors .= "$_\n";
			$errors .= "]]>";
			$e++;
		}
		elsif( /Build failed for project (.*) configuration (.*)/ )
		{
		    my $elapsed_time = gettimeofday() - $time_begin;
			print "Build failed.\n";
		    push( @{$tmp_tc{$2}}, test_case_failure("buildtests", $2, $elapsed_time, "Compilation failed", "<![CDATA[Number of errors: $e\n]]>$errors" ) );
			$errors_cnt++;
            $projects_cnt++;
			
			$err_cnt{$2} = $e;
			$e = 0;
			
			$err_text{$2} = $errors;
			$errors = "";
		}
		elsif( /Build succeeded for project (.*) configuration (.*)/ )
		{
		    my $elapsed_time = gettimeofday() - $time_begin;
			print "Build succeeded.\n";
		    push( @{$tmp_tc{$2}}, test_case_success("buildtests", $2, $elapsed_time, "Compilation succesfull") );
            $projects_cnt++;
            $e = 0;
			$errors = "";			
		}
	}
	
	close TEMP;
	
	foreach my $key ( keys( %tmp_tc ) )
	{
	    if( $err_text{$key} =~ /Overflow in segment/ )
	    {
		    my $elapsed_time = gettimeofday() - $time_begin;
	        push( @tc, test_case_success("buildtests", $key, $elapsed_time, "Compilation succesfull, but with overflow") );
			$errors_cnt--;
	    }
	    else
	    {	
	        push( @tc, @{$tmp_tc{$key}} );
		}
	}  
}
elsif( $compiler =~ /iar/ )
{
    print "Working on iar - $project_name\n";
    	
    my @iar_targets = ( "Debug", "Release", "Int Flash Debug", "Int Flash Release" );
				
	foreach my $iart (@iar_targets)
	{
	    
        my @tc_errors;		 # error lines
		
	    my $iart_fn = $iart;  		
		$iart_fn =~ s/ /_/g; # iar target name for use in file (spaces to underscores)
		
		my $found_text = 0;  # indicator, that we found what we want 
		                     # (up-to-date, errors counter or invalid target )
					 
		# run compilation
        system("D:\\vv_tools\\ewarm_6_10_1\\common\\bin\\IarBuild.exe \"$project\" -make \"$iart\" > build_logs\\iar_".$project_name."_".$iart_fn.".log");
	    
        # open log		
	    open FILEIN, '<', "build_logs\\iar_".$project_name."_".$iart_fn.".log";
		
        # parse log		
    	while(<FILEIN>)
	    {
		    my $elapsed_time = gettimeofday() - $time_begin;
			
            # unchanged project target, next target		
		    if( /Configuration is up-to-date/ )
			{
			    push( @tc, test_case_success("buildtests", $iart, $elapsed_time, "Project up to date") );
				
				$found_text = 1;
				last;
			}
			# error counter
	        elsif( /Total number of errors: (\d+)/ )
		    {
		        if( $1 > 0 )
			    {
			        push( @tc, test_case_failure("buildtests", $iart, $elapsed_time, "build failure", join("\n",@tc_errors)) );
					$errors_cnt++;
			    }
			    elsif( $1 == 0 )
			    {
				    push( @tc, test_case_success("buildtests", $iart, $elapsed_time, "Compilation succesfull") );
			    }
                else
				{
				    push( @tc, test_case_failure("buildtests", $iart, $elapsed_time, "build failure", "This shouldn't happen") );
					$errors_cnt++;
				}
				$found_text = 1;
				last;
		    }
			# unimplemented compilation target
			elsif( /ERROR, Configuration not found:/ )
			{
			    push( @tc, test_case_success("buildtests", $iart, $elapsed_time, "Configuration $iart is not implemented") );
				$found_text = 1;
				last;
			}
			# log error line - store for future use
			elsif( /Error\[.*\]:/ )
			{
			    push( @tc_errors, "<![CDATA[" . $_ . "]]>" );
			}
	    }
        
		# do we found desired text in log?
		if( $found_text == 0 )
		{
		     my $elapsed_time = gettimeofday() - $time_begin;
		    push( @tc, test_case_failure("buildrests", $iart, $elapsed_time, "build error", "Unhandled log text occured") );
		    $errors_cnt++;
		}
		
		close FILEIN;		
		$projects_cnt++;	
    }	
}

my $elapsed_time = gettimeofday() - $time_begin;

print FILEOUT join( "\n", suite_header( $project_name, 0, $errors_cnt, $projects_cnt, $elapsed_time ) ) . "\n";
print FILEOUT join( "\n", @tc ) . "\n";
print FILEOUT join( "\n", suite_footer() ) . "\n";

print FILEOUT join( "\n", report_footer() ) . "\n";
print FILEOUT "<!-- begin time: $time_begin -->\n";
print FILEOUT "<!-- failure of important project -->\n" if( ($important_project == 1) && ($errors_cnt > 0) );
print FILEOUT join( "\n", report_summary( $errors_cnt_all + $errors_cnt, $projects_cnt_all + $projects_cnt ) ) . "\n";

close FILEOUT;
