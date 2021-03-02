#!/usr/bin/perl

use strict;

use MQXUtils;

my $arg_mqxpath = "undefined";

while( my $arg = shift @ARGV )
{    
    if( $arg =~ /-mqx/ )
    {
        $arg_mqxpath = shift @ARGV;
    }
    elsif( $arg =~ /-c/ )
    {

    }
}

print "MQX Install directory: " . $arg_mqxpath . "\n";
my @mqxfiles = expand_path_recursive( $arg_mqxpath );

################################################################################

my $cfg = get_cfg();

my @cwmcu63_examples; my @cwmcu63_libs;
my @cwcf72_examples;  my @cwcf72_libs;
my @cwmpc92_examples; my @cwmpc92_libs;
my @cw10_examples;    my @cw10_libs;
my @iar_examples;     my @iar_libs;

foreach my $f ( @mqxfiles )
{
    ##########################################
    # cwcf72 says 54418 is unknown processor #
    next if( $f =~ /.*54418.*/ );
    ##########################################
     
    # find cwmcu63 projects
    if( $f =~ /.*cwmcu63.*\.mcp/ )
    {
        # find examples
        if( ( $f =~ /.*\\examples\\.*/ ) || ( $f =~ /.*\\demo\\.*/ ) )
        {
            push( @cwmcu63_examples, $f );
        }

        # find libs
        else
        {
            push( @cwmcu63_libs, $f )
        }
    }
    # find cwcf72 projects
    elsif( $f =~ /.*cwcf72.*\.mcp/ )
    {
        # find examples
        if( ( $f =~ /.*\\examples\\.*/ ) || ( $f =~ /.*\\demo\\.*/ ) )
        {
            push( @cwcf72_examples, $f );
        }

        # find libs
        else
        {
            push( @cwcf72_libs, $f );
        }
    }

    # find cwmpc92 projects
    elsif( $f =~ /.*cwmpc92.*\.mcp/ )
    {
        # find examples
        if( ( $f =~ /.*\\examples\\.*/ ) || ( $f =~ /.*\\demo\\.*/ ) )
        {
            push( @cwmpc92_examples, $f );
        }

        # find libs
        else
        {
            push( @cwmpc92_libs, $f )
        }
    }

    # find cw10 projects
    elsif( $f =~ /.*\.project/ )
    {
        # find examples
        if( ( $f =~ /.*\\examples\\.*/ ) || ( $f =~ /.*\\demo\\.*/ ) )
        {
            push( @cw10_examples, $f );
        }

        # find libs
        else
        {
            push( @cw10_libs, $f )
        }
    }
    
    # find iar projects
    elsif( $f =~ /.*iar.*\.ewp/ )
    {
        # find examples
        if( ( $f =~ /.*\\examples\\.*/ ) || ( $f =~ /.*\\demo\\.*/ ) )
        {
            push( @iar_examples, $f );
        }

        # find libs
        else
        {
            push( @iar_libs, $f )
        }
    }
}

print "copy prepare_files\\build_all.h ".$arg_mqxpath."\\config\\common\\build_all.h";
system( "copy prepare_files\\build_all.h ".$arg_mqxpath."\\config\\common\\build_all.h" );
print "copy prepare_files\\verif_enabled_config.h ".$arg_mqxpath."\\config\\common\\verif_enabled_config.h";
system( "copy prepare_files\\verif_enabled_config.h ".$arg_mqxpath."\\config\\common\\verif_enabled_config.h" );

print "Now these cwmcu63 libraries:\n".join( "\n", @cwmcu63_libs )."\n\n";
foreach my $p ( @cwmcu63_libs )
{
    system( $cfg->{'PERL_INTERPRETER'} . " autobuild_compile.pl -noskip -c cwmcu63 -p \"$p\"" );
}

print "Now these cwmcu63 examples:\n".join( "\n", @cwmcu63_examples )."\n\n";
foreach my $p ( @cwmcu63_examples )
{
    system( $cfg->{'PERL_INTERPRETER'} . " autobuild_compile.pl -noskip -c cwmcu63 -p \"$p\"" );
}

print "Now these cwcf72 libraries:\n".join( "\n", @cwcf72_libs )."\n\n";
foreach my $p ( @cwcf72_libs )
{
    system( $cfg->{'PERL_INTERPRETER'} . " autobuild_compile.pl -noskip -c cwcf72 -p \"$p\"" );
}

print "Now these cwcf72 examples:\n".join( "\n", @cwcf72_examples )."\n\n";
foreach my $p ( @cwcf72_examples )
{
    system( $cfg->{'PERL_INTERPRETER'} . " autobuild_compile.pl -noskip -c cwcf72 -p \"$p\"" );
}

print "Now these cw10 libraries:\n".join( "\n", @cw10_libs )."\n\n";
foreach my $p ( @cw10_libs )
{
    system( $cfg->{'PERL_INTERPRETER'} . " autobuild_compile.pl -noskip -c cw10 -p \"$p\"" );
}

print "Now these cw10 examples:\n".join( "\n", @cw10_examples )."\n\n";
foreach my $p ( @cw10_examples )
{
    system( $cfg->{'PERL_INTERPRETER'} . " autobuild_compile.pl -noskip -c cw10 -p \"$p\"" );
}

print "Now these iar libraries:\n".join( "\n", @iar_libs )."\n\n";
foreach my $p ( @iar_libs )
{
    system( $cfg->{'PERL_INTERPRETER'} . " autobuild_compile.pl -noskip -c iar -p \"$p\"" );
}

print "Now these iar examples:\n".join( "\n", @iar_examples )."\n\n";
foreach my $p ( @iar_examples )
{
    system( $cfg->{'PERL_INTERPRETER'} . " autobuild_compile.pl -noskip -c iar -p \"$p\"" );
}



################################################################################

my @output;

push( @output, "<html><body><h1>Installer build results</h1><table><tr><td>Project</td><td>Targets</td><td>Failed</td><td><b>Result</b></td></tr>" );

foreach my $comp ('cwmcu63','cwcf72','cw10','iar')
{
    push( @output, "<tr><td colspan='4'>$comp</td></tr>" );

    open FILE, '<', "build_logs\/$comp.xml";

    while(<FILE>)
    {
        if( $_ =~ "<testsuite name=\"(.*)\" errors=\"(.*)\" failures=\"(.*)\" tests=\"(.*)\" time=\".*\">" )
        {
            my $errs = $2 + $3;
            my $res  = "OK";
            $res = "ERROR" if( $errs eq 1 );
            $res = "ERRORS" if( $errs > 1 );

            push( @output, "<tr><td>$1</td><td>$4</td><td>$errs</td><td><b>$res</b></td></tr>" );
        }
    }

    close FILE;
}

push( @output, "</table></body></html>" );

open FILE, '>', "build_logs\/result.html";

print FILE join("\n", @output);

close FILE;
