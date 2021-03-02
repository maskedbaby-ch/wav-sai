#!/usr/bin/perl

use strict;
use MQXCruiseXML;

my $report_log = shift @ARGV;
my $report_xml = shift @ARGV;

open FILE, '<', $report_log || die "Cannot open $report_log";

my $all_test_sum  = 0;
my $all_test_fail = 0;
my $cur_test_sum  = 0;
my $cur_test_fail = 0;

my @test_cases = ();
my @final_report = ();

my $time_beg = 0;
my $suite_name = "";

push( @final_report, report_header() );

while(<FILE>)
{
    
    my @line = split( ";", $_ );
	
    if( $line[0] == 0 )
	{
	    $suite_name = $line[2];
		$time_beg = $line[3] unless $time_beg;
	}
	elsif( $line[0] == 12 )
	{
	
	    push( @final_report, suite_header( $suite_name, 0, $cur_test_fail, $cur_test_sum, $line[3] - $time_beg ) );
		push( @final_report, @test_cases );
	    push( @final_report, suite_footer() );

		$all_test_sum  += $cur_test_sum;
		$all_test_fail += $cur_test_fail;
		
		@test_cases = ();
		$cur_test_sum  = 0;
        $cur_test_fail = 0;
		$suite_name = "";
	}
	elsif( ( $line[0] > 0 ) && ( $line[0] < 11 ) )
	{
	    $cur_test_sum++;
		
		my $test_text = $line[2];
		
		# multiline output
		if( $line[2] =~ /LOGS/ )
		{
		    $test_text = "";
		    while(<FILE>)
			{
			    last if( /LOGE/ );
				$test_text = $test_text . $_ ;                 			
			}
		}
		
	    if( $line[4] =~ /OK/ )
		{
		    push( @test_cases, test_case_success( "buildtests", $line[1], $line[3] - $time_beg, $test_text ) );	
		}
		elsif( $line[4] =~ /FAIL/ )
		{
            $cur_test_fail++;	    
            push( @test_cases, test_case_failure( "buildtests", $line[1], $line[3] - $time_beg, "prepare_error", $test_text) );			
	    }
	}
}

push( @final_report, report_footer() );
push( @final_report, report_summary( $all_test_fail, $all_test_sum ) );

close FILE;

open FILE2, '>', $report_xml || die "Cannot open $report_xml";
foreach(@final_report)
{
    print FILE2 $_."\n";
}
close FILE2;

print "Published: $report_xml\n";
