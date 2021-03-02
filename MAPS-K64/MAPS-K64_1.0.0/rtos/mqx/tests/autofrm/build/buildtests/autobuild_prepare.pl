#!/usr/bin/perl

use MQXCruiseXML;
use MQXUtils;

my $cfg = get_cfg();

my $BUILD_LOGS_DIR   = $cfg->{'BUILDTEST_DIR'} . "\\build_logs";
my $CRUISE_CONFS_DIR = $cfg->{'BUILDTEST_DIR'} . "\\cruise_confs";
my $CSPECS_DIR       = $cfg->{'BUILDTEST_DIR'} . "\\cspecs";
my $PROJECTS_DIR     = $cfg->{'BUILDTEST_DIR'} . "\\projects";

my $PL_KP_VIEW       = $cfg->{'VIEW_DIR'} . "\\" . $cfg->{'PLATFORM_KEEPER_NAME'};

my $CR_XML_CFG_DIR   = $cfg->{'CRUISE_CTRL_CFG_DIR'} . "\\int";
my $PK_PROJECT_DIR   = $cfg->{'CRUISE_CTRL_PRJ_DIR'} . "\\ar_int_platform_keeper";

mkdir $BUILD_LOGS_DIR;
mkdir $CRUISE_CONFS_DIR;
mkdir $CSPECS_DIR;
mkdir $PROJECTS_DIR;

mkdir $PK_PROJECT_DIR;
mkdir $PK_PROJECT_DIR . "\\target";
mkdir $PK_PROJECT_DIR . "\\target\\test-results";

if( not -d $PL_KP_VIEW )
{ 
    system( "cleartool mkview -snapshot $PL_KP_VIEW" );
    system( "copy prepare_files\\platform_keeper.cspec $PL_KP_VIEW\\platform_keeper.cspec" );
    system( "cd $PL_KP_VIEW & cleartool setcs platform_keeper.cspec" );
}

unlink "$CR_XML_CFG_DIR\\config_int_platform_keeper.xml";
print_pk_config_xml("$CR_XML_CFG_DIR\\config_int_platform_keeper.xml");

