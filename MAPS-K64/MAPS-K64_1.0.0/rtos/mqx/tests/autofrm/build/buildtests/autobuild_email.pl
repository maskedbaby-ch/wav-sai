#!/usr/bin/perl

use MQXCruiseXML;
use MQXUtils;

my $cfg = get_cfg();

my $platform;
my $compiler;

my $modifiers = $cfg->{'VIEW_DIR'}."\\zcz01-1107_view_buildtest_".$platform."\\build_".$compiler;

##########

my @users = ();

open FILE, '<', $modifiers || die "Cannot open list of modifiers...";
@users = <FILE>;
close FILE;

##########

open FILE, '>', "report.txt";
print FILE <<'END_OF_MAIL';
Hi,

You and your colleagues have recently done some modification to platform
${platform} and now it cannot be compiled under ${compiler}. It doesn't mean 
that they were your modifications which caused compilation errors, but just 
that you are one of the people who made some change sice last successfull build.

These people are:
END_OF_MAIL

print FILE "\n".join(',',@users)."\n\n";
print FILE "Have a nice day.";
close FILE;

##########

my $blat    = "\\\\zcz01app02\\cc_Shared\\Scripts\\Tools\\blat\\full\\blat.exe";

#foreach my $u (@users)
{
    my $u = "b35850";
    system("$blat report.txt -t ".$u."@freescale.com -s \"Failed autobuild\" -server remotesmtp.freescale.net -f rene.kolarik\@freescale.com");
}