package MQXUtils;

use strict;
use Time::HiRes qw/gettimeofday/;

use vars qw( @ISA @EXPORT @EXPORT_OK );

require Exporter;

@ISA = qw( Exporter AutoLoader );

@EXPORT = qw( include expand_path expand_path_recursive compare_arrays checkpoint get_cfg );

sub include
{
  my ($in) = @_;
  my $intext;

  open INFILE,'<',$in or die "Can not open file $in";
  read INFILE,$intext,100000 or die "Can not read file $in";
  close INFILE;

  eval $intext or die "Error processing \'$in\': $@";
}

sub expand_path
{
    my $path = shift @_;
  	my @ret  = ();
	
    # $path = xxxxxxxxxx\*
    if( $path =~ /([^\*]*[\/\\])\*$/ )
    {
		opendir( DIR, $1 ) or print "Can not open directory \'$1\'\n";
		
		readdir(DIR);
		readdir(DIR);
		
		while( my $file = readdir(DIR) )
		{
		    if( opendir( INDIR, $1.$file ) )
			{
			    closedir( INDIR );
			}
			else
			{
			    push(@ret, $1.$file );
			}		    
		    
		    #push(@ret, expand_path_recursive($1.$file) );
		}
		
		closedir( DIR );
    }
	else
	{
	    push( @ret, $path );
	}
	
	return @ret;
}

sub expand_path_recursive
{
    my $path = shift @_;
  	my @ret  = ();
	
   	if( opendir( DIR, $path."\/" ) )
	{		
	    readdir(DIR);
	    readdir(DIR);
		
		my @tmpret = readdir(DIR);
		
		closedir( DIR );
		
		while( my $file = shift @tmpret )
		{
		    push( @ret, expand_path_recursive($path."\\".$file) );
		}	
	}
	else
	{
	    push( @ret, $path );
	}
    
	return @ret;
}

sub compare_arrays
{
    # !! second array is modified !!
		
    my $arr1 = shift;
	my $arr2 = shift;

    # remove newlines	
	foreach( @$arr1 ) { s/\n//;	}		
	foreach( @$arr2 ) { s/\n//;	}		
	
	if( @$arr1 != @$arr2 )	{ return 0;	}
	
	# compare items
	foreach(@$arr1)
	{
	    my $tmp = shift @$arr2;
	    if( $_ !~ /\Q$tmp/ )
		{
		    return 0;
		}
	}
	
	return 1;
}

sub checkpoint
{
    my $log_path  = shift @_;
    my $condition = shift @_;
	my $text_ok   = shift @_;
	my $text_fail = shift @_;
	
	my $sec = gettimeofday();

	open FILE, ">>", $log_path;
	 
    if($condition)
    {
	    print FILE $text_ok.";".$sec.";OK\n";
		close FILE;
		return;
    }
	
	print FILE $text_fail.";".$sec.";FAILED\n";
	print FILE "12;end;End;".$sec.";FAILED\n";
	close FILE;
		
	exit 0;
}

sub get_cfg
{
    my %cfg;
	
	open FILE, "<", "autobuild.cfg";
	
	while(<FILE>)
	{
	    if( $_ =~ /([^\s]+)\s*=\s*"(.*)";/ )
		{
		    my $name  = $1;
			my $value = $2;
			
		    if( $name =~ /PLATFORMS/ )
			{
			    push( @{$cfg{$name}}, split(/\s*,\s*/,$value) );
			}
			else
			{
		        $cfg{$name} = $value;
			}
		}
	}
	
	close FILE;
		
    return \%cfg;
}

1
