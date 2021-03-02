#!/usr/bin/perl

use MQXCruiseXML;
use MQXUtils;

use Time::HiRes qw/gettimeofday/;

my $configuration_data = get_cfg();

my $VIEWS_PATH   = $configuration_data->{'VIEW_DIR'};
my $SCRIPT_PATH  = $configuration_data->{'VIEW_DIR'} . "\\" . $configuration_data->{'PLATFORM_KEEPER_NAME'};
my $VOB_SRC_PATH = "\\MSGSW\\MQX\\src\\";

################################################################################
#
# Purpose of this script is to prepare every
#
# -p PLATFORM
#
################################################################################

my %valid_platforms = ( 'kwikstikk40x256'    => 'mk4x',
                        'm51acdemo'          => 'mcf51ac', 
                        'm51emdemo'          => 'mcf51em',
                        'm51jmevb'           => 'mcf51jm',
                        'm5208evb'           => 'mcf520x',
                        'm52221demo'         => 'mcf5222x',
                        'm52223evb'          => 'mcf5222x',
                        'm52233demo'         => 'mcf5223x',
                        'm52235evb'          => 'mcf5223x',
                        'm52259demo'         => 'mcf5225x',
                        'm52259evb'          => 'mcf5225x',
                        'm52277evb'          => 'mcf5227x',
                        'm5235bcc'           => undef,
                        'm5282evb'           => undef,
                        'm53015evb'          => 'mcf5301x',
                        'm5329evb'           => 'mcf532x',
                        'm54455evb'          => 'mcf5445x',
                        'mpc5125ads'         => 'mpc512x',
                        'mpc8308rdb'         => 'mpc830x',
                        'pioneer2_palladium' => undef,
                        'pioneer2fpga'       => undef,
                        'stm3210e'           => undef,
                        'twrk40x256'         => 'mk4x',
                        'twrk60n512'         => 'mk6x',
                        'twrmcf51ag'         => 'mcf51ag',
                        'twrmcf51cn'         => 'mcf51cn',
                        'twrmcf51je'         => 'mcf51je',
                        'twrmcf51jf'         => 'mcf51jf',
                        'twrmcf51mm'         => 'mcf51mm',
                        'twrmcf51qm'         => 'mcf51qm',
                        'twrmcf52259'        => 'mcf5225x',
                        'twrmcf54418'        => 'mcf5441x',
                        'twrmpc5125'         => 'mpc512x' );
                        
my %platform_names = (  'kwikstikk40x256'    => 'KWIKSTIK_K40X256',
                        'm51acdemo'          => 'M51ACDEMO', 
                        'm51emdemo'          => 'M51EMDEMO',
                        'm51jmevb'           => 'M51JMEVB',
                        'm5208evb'           => 'M5208EVB',
                        'm52221demo'         => 'M52221DEMO',
                        'm52223evb'          => 'M52223EVB',
                        'm52233demo'         => 'M52223DEMO',
                        'm52235evb'          => 'M52235EVB',
                        'm52259demo'         => 'M52259DEMO',
                        'm52259evb'          => 'M52259EVB',
                        'm52277evb'          => 'M52277EVB',
                        'm5235bcc'           => undef,
                        'm5282evb'           => undef,
                        'm53015evb'          => 'M53015EVB',
                        'm5329evb'           => 'M5329EVB',
                        'm54455evb'          => 'M54455EVB',
                        'mpc5125ads'         => 'MPC5125ADS',
                        'mpc8308rdb'         => 'MPC8308',
                        'pioneer2_palladium' => undef,
                        'pioneer2fpga'       => undef,
                        'stm3210e'           => undef,
                        'twrk40x256'         => 'TWR_K40X256',
                        'twrk60n512'         => 'TWR_K60N512',
                        'twrmcf51ag'         => 'TWR_MCF51AG',
                        'twrmcf51cn'         => 'TWR_MCF51CN',
                        'twrmcf51je'         => 'TWR_MCF51JE',
                        'twrmcf51jf'         => 'TWR_MCF51JF',
                        'twrmcf51mm'         => 'TWR_MCF51MM',
                        'twrmcf51qm'         => 'TWR_MCF51QM',
                        'twrmcf52259'        => 'TWR_MCF52259',
                        'twrmcf54418'        => 'TWR_MCF54418',
                        'twrmpc5125'         => 'TWR_MPC5125' );

(shift @ARGV) =~ /^-l$/ || die "Specify logfile path with '-l PATH'";

my $log = shift @ARGV;

my $p_parm = 1;
(shift @ARGV) =~ /^-p$/ || ($p_parm = 0);

my $platform = shift @ARGV;

checkpoint( $log, 1, "00;start;$platform" );
checkpoint( $log, $p_parm, "01;cmdline_check;Parameter -p is ok", "01;cmdline_check;Specify -p switch as second arg" );
checkpoint( $log, (defined $valid_platforms{$platform}), "02;platform_check;Platform $platform is ok", "02;platform_check;Undefined platform $platform" );

################################################################################
#                       
# First transform *.in files to configspecs
#
################################################################################

my $in             = $configuration_data->{'INFILES_DIR'}."\\".$valid_platforms{ $platform }.".in";
my $sharedin       = $configuration_data->{'INFILES_DIR'}."\\shared.in";
my $component_name = $platform_names{ $platform };
my $intext;

my %COMPONENTS;
my $eval_res = 1;

# load shared infile
checkpoint( $log, (open INFILE,'<', $sharedin), "03;open_shared_infile;$sharedin file opened"
                                              , "03;open_shared_infile;Cannot open file $sharedin" );
checkpoint( $log, (read INFILE,$intext,100000), "04;read_shared_infile;$in file read"
                                              , "04;read_shared_infile;Cannot read file $in" );
close INFILE;

eval $intext or ($eval_res = 0);
checkpoint( $log, $eval_res, "05;eval_shared_infile;shared.in file evaluated", "05;eval_shared_infile;Cannot evaluate file shared.in" );
$intext = "";

# load infile for given platform
checkpoint( $log, (open INFILE,'<',$in), "06;open_platform_infile;*.in file opened", "06;open_platform_infile;Cannot open file $in" );
checkpoint( $log, (read INFILE,$intext,100000), "07;read_platform_infile;*.in file read", "07;read_platform_infile;Cannot read file $in" );
close INFILE;

eval $intext or ($eval_res = 0);
checkpoint( $log, $eval_res, "08;eval_platform_infile;*.in file evaluated", "08;eval_platform_infile;Cannot evaluate file $in" );

my @files = ();

my %prj_cw63;
my %prj_cw72;
my %prj_cw88;
my %prj_cw10;
my %prj_iar;

# store file list of platform specific sources
foreach( @{$COMPONENTS{ $component_name."_MQX_SOURCE" }} )
{
    push(@files, expand_path($SCRIPT_PATH.$VOB_SRC_PATH.$_));
}

# store file list of common sources like examples etc.
foreach( @{$COMPONENTS{ "COMMON_MQX_SOURCE" }} )
{
    push(@files, expand_path($SCRIPT_PATH.$VOB_SRC_PATH.$_));
}

# store file list of shared sources
foreach( @{$COMPONENTS{ "SHARED_MQX_SOURCE" }} )
{
    push(@files, expand_path($SCRIPT_PATH.$VOB_SRC_PATH.$_));
}

# modify files & create list of projects
foreach(@files)
{
    my $hash_ref;
    
    s/\Q$SCRIPT_PATH//i;
    # determine if it is a project and for which compiler   
    if( $_ =~ /.*[\\\/]([^\\\/]+)[\\\/][^\\\/]+\.mcp/i and $_ !~ /stationery/i )
    {
        if( $1 =~ /cwmcu63/ )
        {
            $hash_ref = \%prj_cw63;
        }
        elsif( $1 =~ /cwcf72/ )
        {
            $hash_ref = \%prj_cw72;
        }
        elsif( $1 =~ /cwmpc88/ )
        {            
            $hash_ref = \%prj_cw88;            
        }
    }
    elsif( /.*\.project/i )
    {
        $hash_ref = \%prj_cw10;
    }   
    elsif( /.*\.ewp/i )
    {
        $hash_ref = \%prj_iar;
    }
    else
    {
        # it isnt project - next line
        next;
    }
    
    # determine type of project
    if ($_ =~ /psp_/i)
    {
        push( @{$hash_ref->{'psp_proj'}}, $_ );
    }
    elsif ($_ =~ /bsp_/i)
    {
        push( @{$hash_ref->{'bsp_proj'}}, $_ );
    }
    elsif ($_ =~ /.*build.*shell_/i)
    {
        push( @{$hash_ref->{'shell_proj'}}, $_ );
    }
    elsif ($_ =~ /.*build.*usb_/i)
    {
        push( @{$hash_ref->{'usb_proj'}}, $_ );
    }
    elsif ($_ =~ /.*build.*rtcs_/i)
    {
        push( @{$hash_ref->{'rtcs_proj'}}, $_ );
    }
    elsif ($_ =~ /.*build.*mfs_/i)
    {
        push( @{$hash_ref->{'mfs_proj'}}, $_ );
    }
    elsif ($_ =~ /_libs/i)
    {
        push( @{$hash_ref->{'libs_proj'}}, $_ );
    }
    else 
    {
        push( @{$hash_ref->{'other_proj'}}, $_ );
    }
}

my @projects_cw63 = ( @{$prj_cw63{'libs_proj'}}, 
                      @{$prj_cw63{'psp_proj'}},
                      @{$prj_cw63{'bsp_proj'}},
                      @{$prj_cw63{'rtcs_proj'}},
                      @{$prj_cw63{'mfs_proj'}},
                      @{$prj_cw63{'shell_proj'}},
                      @{$prj_cw63{'usb_proj'}},
                      @{$prj_cw63{'other_proj'}} );
                      
my @projects_cw72 = ( @{$prj_cw72{'libs_proj'}}, 
                      @{$prj_cw72{'psp_proj'}},
                      @{$prj_cw72{'bsp_proj'}},
                      @{$prj_cw72{'rtcs_proj'}},
                      @{$prj_cw72{'mfs_proj'}},
                      @{$prj_cw72{'shell_proj'}},
                      @{$prj_cw72{'usb_proj'}},
                      @{$prj_cw72{'other_proj'}} );

my @projects_cw88 = ( @{$prj_cw88{'libs_proj'}}, 
                      @{$prj_cw88{'psp_proj'}},
                      @{$prj_cw88{'bsp_proj'}},
                      @{$prj_cw88{'rtcs_proj'}},
                      @{$prj_cw88{'mfs_proj'}},
                      @{$prj_cw88{'shell_proj'}},
                      @{$prj_cw88{'usb_proj'}},
                      @{$prj_cw88{'other_proj'}} );

my @projects_cw10 = ( @{$prj_cw10{'libs_proj'}}, 
                      @{$prj_cw10{'psp_proj'}},
                      @{$prj_cw10{'bsp_proj'}},
                      @{$prj_cw10{'rtcs_proj'}},
                      @{$prj_cw10{'mfs_proj'}},
                      @{$prj_cw10{'shell_proj'}},
                      @{$prj_cw10{'usb_proj'}},
                      @{$prj_cw10{'other_proj'}} );

my @projects_iar  = ( @{$prj_iar{'libs_proj'}}, 
                      @{$prj_iar{'psp_proj'}},
                      @{$prj_iar{'bsp_proj'}},
                      @{$prj_iar{'rtcs_proj'}},
                      @{$prj_iar{'mfs_proj'}},
                      @{$prj_iar{'shell_proj'}},
                      @{$prj_iar{'usb_proj'}},
                      @{$prj_iar{'other_proj'}} );                    

################################################################################
#
# Find out if *.proj or *.cspec regeneration is needed
#
################################################################################

my $regen_cspec      = 0;
my $regen_proj_cw63  = 0;
my $regen_proj_cw72  = 0;
my $regen_proj_cw88  = 0;
my $regen_proj_cw10  = 0;
my $regen_proj_iar   = 0;

my $exists_cspec     = 1;
my $exists_proj_cw63 = 1;
my $exists_proj_cw72 = 1;
my $exists_proj_cw88 = 1;
my $exists_proj_cw10 = 1;
my $exists_proj_iar  = 1;


# open existing files for reading
$exists_cspec     = 0  if( not open FILE_CSPEC, '<', "cspecs\\$platform.cspec" );
$exists_proj_cw63 = 0  if( not open FILE_CW63, '<', "projects\\$platform.cw63" );
$exists_proj_cw72 = 0  if( not open FILE_CW72, '<', "projects\\$platform.cw72" );
$exists_proj_cw88 = 0  if( not open FILE_CW88, '<', "projects\\$platform.cw88" );
$exists_proj_cw10 = 0  if( not open FILE_CW10, '<', "projects\\$platform.cw10" );
$exists_proj_iar  = 0  if( not open FILE_IAR, '<', "projects\\$platform.iar" );

#remove "verif_enabled_config.h" from cspec
{
    my @tmp;
    push( @tmp, @files );
    @files = ();
    
    foreach(@tmp)
    {
        if( $_ !~ /verif_enabled_config/ )
        {           
            push( @files, $_ );
        }
    }
}

# check changes in cspec file list
if( $exists_cspec == 1 )
{
    my @tmparr; 
    @tmparr = <FILE_CSPEC>;
    $regen_cspec = 1 if ( (shift @tmparr) !~ /^element \* CHECKEDOUT\n$/ );
    $regen_cspec = 1 if ( (shift @tmparr) !~ /^element \* main\/LATEST\n$/ );
    $regen_cspec = 1 if ( (shift @tmparr) !~ /^\n$/ );  
        
    foreach(@tmparr) { s/^load (.*)/$1/ };
    
    $regen_cspec = 1 if ( not compare_arrays( \@files, \@tmparr ) );
}
else
{
    $regen_cspec = 1;
}

# check changes in cw63 project list
if( $exists_proj_cw63 == 1 )
{
    my @tmparr;
    @tmparr = <FILE_CW63>;
    $regen_proj_cw63 = 1 if( not compare_arrays( \@projects_cw63, \@tmparr ) );
}
elsif( @projects_cw63 > 0 )
{
    $regen_proj_cw63 = 1;
}

# check changes in cw72 project list
if( $exists_proj_cw72 == 1 )
{
    my @tmparr;
    @tmparr = <FILE_CW72>;
    $regen_proj_cw72 = 1 if( not compare_arrays( \@projects_cw72, \@tmparr ) );
}
elsif( @projects_cw72 > 0 )
{
    $regen_proj_cw72 = 1;
}

# check changes in cw88 project list
if( $exists_proj_cw88 == 1 )
{
    my @tmparr;
    @tmparr = <FILE_CW88>;
    $regen_proj_cw88 = 1 if( not compare_arrays( \@projects_cw88, \@tmparr ) );
}
elsif( @projects_cw88 > 0 )
{
    $regen_proj_cw88 = 1;
}

# check changes in cw10 project list
if( $exists_proj_cw10 == 1 )
{
    my @tmparr;
    @tmparr = <FILE_CW10>;
    $regen_proj_cw10 = 1 if( not compare_arrays( \@projects_cw10, \@tmparr ) );
}
elsif( @projects_cw10 > 0 )
{
    $regen_proj_cw10 = 1;
}

# check changes in iar project list
if( $exists_proj_iar == 1 )
{
    my @tmparr;
    @tmparr = <FILE_IAR>;
    $regen_proj_iar = 1 if( not compare_arrays( \@projects_iar, \@tmparr ) );
}
elsif( @projects_iar > 0 )
{
    $regen_proj_iar = 1;
}

close FILE_CSPEC; 
close FILE_CW63; 
close FILE_CW72;
close FILE_CW88;
close FILE_CW10; 
close FILE_IAR;

if( not -d "cspecs" )
{
    mkdir "cspecs";
}

if( not -d "projects" )
{
    mkdir "projects";
}

if( not -d "cruise_confs" )
{
    mkdir "cruise_confs";
}

if( not -d "build_logs" )
{
    mkdir "build_logs";
}

################################################################################
#
# Regenerate changed config spec, project lists, cc view...
#
################################################################################

# if file list changed
if( $regen_cspec )
{
    # create new configspec content
    checkpoint( $log, (open FILE, '>', "cspecs\\$platform.cspec"), "06;open_cspec;Configspec $platform.cpec opened", "06;open_cspec;Cannot open configspec file $platform.cspec");
    print FILE "element * CHECKEDOUT\n";
    print FILE "element * main/LATEST\n";
    print FILE "\n";    
    foreach(@files)
    {
        print FILE "load $_\n";
    }   
    close FILE;
    
    # dont bother with thinking - everytime the cspec is changed recreate view
    my $current_view_path = $VIEWS_PATH."\\zcz01-1107_view_buildtest_".$platform;
    system("cleartool rmview ".$current_view_path) if( -d $current_view_path );
    system("cleartool mkview -snapshot ".$current_view_path."\n");
    system("copy cspecs\\$platform.cspec ".$current_view_path."\\$platform.cspec");
    system("cd ".$current_view_path." & cleartool setcs $platform.cspec\n");
    system("copy prepare_files\\build_all.h ".$current_view_path."\\MSGSW\\MQX\\src\\config\\common\\build_all.h" );
    system("copy prepare_files\\verif_enabled_config.h ".$current_view_path."\\MSGSW\\MQX\\src\\config\\common\\verif_enabled_config.h" );

    my @view_dir_content = ();
    my $update_file = "";
    
    opendir( DIR, $VIEWS_PATH."\\zcz01-1107_view_buildtest_".$platform );
    @view_dir_content = readdir(DIR);
    closedir(DIR);
    
    foreach(@view_dir_content)
    {
        if( /(.*)\.updt$/ )
        {
            $update_file = $_;  
        }
    }
    
    my $errors = 0;
    my @err_arr = ();
    
    $errors = parse_clearcase_output( $VIEWS_PATH."\\zcz01-1107_view_buildtest_".$platform."\\".$update_file, \@err_arr );
        
    if($errors > 0)
    {
        open FILE, '>>', $log;  
        print FILE "07;update_view;LOGS;".gettimeofday().";FAILED\n";
        foreach(@err_arr)
        {
            s/\s+/ /g;
            s/\n//;
            print FILE $_."\n";
        }
        print FILE "LOGE\n";
        close FILE;
    }
    else
    {
        checkpoint( $log, 1,"07;update_view;All files updated successfuly");
    }   
}

# keeper dirs, files and files for ordering build via touch from keeper
mkdir("D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_keeper");
mkdir("D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_keeper\\target");
mkdir("D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_keeper\\target\\test-results");
print_ps_config_xml("cruise_confs\\".$platform."_keeper.xml", $platform, scalar(@projects_cw63), scalar(@projects_cw72), scalar(@projects_cw10), scalar(@projects_iar), scalar(@projects_cw88) );
system("touch ".$VIEWS_PATH."\\zcz01-1107_view_buildtest_".$platform."\\build_cwmcu63") if( @projects_cw63 > 0 );
system("touch ".$VIEWS_PATH."\\zcz01-1107_view_buildtest_".$platform."\\build_cwcf72") if( @projects_cw72 > 0 );
system("touch ".$VIEWS_PATH."\\zcz01-1107_view_buildtest_".$platform."\\build_cw88") if( @projects_cw88 > 0 );
system("touch ".$VIEWS_PATH."\\zcz01-1107_view_buildtest_".$platform."\\build_cw10") if( @projects_cw10 > 0 );
system("touch ".$VIEWS_PATH."\\zcz01-1107_view_buildtest_".$platform."\\build_iar") if( @projects_iar > 0 );
system("copy /Y cruise_confs\\".$platform."_keeper.xml D:\\vv_tools\\af\\CruiseControl\\config_xml\\int\\config_int_".$platform."_keeper.xml");
                                                               
# if project list for cw63 changed
if( $regen_proj_cw63 )
{
    # create new project list content and copy it
    checkpoint( $log, (open FILE, '>', "projects\\$platform.cw63"), 
                "08;open_prjlist_cw63;Project list $platform.cw63 opened", 
                "08;open_prjlist_cw63;Cannot open project list $platform.cw63" ); 
    foreach(@projects_cw63) { print FILE "$_\n"; }
    close FILE; 
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\logs\\ar_int_".$platform."_cwmcu63");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_cwmcu63");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_cwmcu63\\target");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_cwmcu63\\target\\test-results");
}
# regenerate cw63 config xml
if( $regen_proj_cw63 || $exists_proj_cw63 )
{
    print_config_xml("cruise_confs\\".$platform."_cwmcu63.xml", $platform, "cwmcu63", \@projects_cw63);
    system("copy projects\\$platform.cw63 ".$VIEWS_PATH."\\zcz01-1107_view_buildtest_".$platform."\\$platform.cw63");    
    system("copy /Y cruise_confs\\".$platform."_cwmcu63.xml D:\\vv_tools\\af\\CruiseControl\\config_xml\\int\\config_int_".$platform."_cwmcu63.xml");
}

# if project list for cw72 changed
if( $regen_proj_cw72 )
{
    # create new project list content and copy it
    checkpoint( $log, (open FILE, '>', "projects\\$platform.cw72"), 
                "09;open_prjlist_cw72;Project list $platform.cw72 opened", 
                "09;open_prjlist_cw72;Cannot open project list $platform.cw72" ); 
    foreach(@projects_cw72) { print FILE "$_\n"; }
    close FILE;
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\logs\\ar_int_".$platform."_cwcf72");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_cwcf72");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_cwcf72\\target");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_cwcf72\\target\\test-results");
}
# regenerate cw72 config xml
if( $regen_proj_cw72 || $exists_proj_cw72 )
{
    print_config_xml("cruise_confs\\".$platform."_cwcf72.xml", $platform, "cwcf72", \@projects_cw72);
    system("copy projects\\$platform.cw72 ".$VIEWS_PATH."\\zcz01-1107_view_buildtest_".$platform."\\$platform.cw72");       
    system("copy /Y cruise_confs\\".$platform."_cwcf72.xml D:\\vv_tools\\af\\CruiseControl\\config_xml\\int\\config_int_".$platform."_cwcf72.xml");
}

# if project list for cw88 changed
if( $regen_proj_cw88 )
{
    # create new project list content and copy it
    checkpoint( $log, (open FILE, '>', "projects\\$platform.cw88"), 
                "09;open_prjlist_cw88;Project list $platform.cw88 opened", 
                "09;open_prjlist_cw88;Cannot open project list $platform.cw88" ); 
    foreach(@projects_cw88) { print FILE "$_\n"; }
    close FILE;
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\logs\\ar_int_".$platform."_cwc88");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_cw88");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_cw88\\target");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_cw88\\target\\test-results");
}
# regenerate cw88 config xml
if( $regen_proj_cw88 || $exists_proj_cw88 )
{
    print_config_xml("cruise_confs\\".$platform."_cw88.xml", $platform, "cw88", \@projects_cw88);
    system("copy projects\\$platform.cw88 ".$VIEWS_PATH."\\zcz01-1107_view_buildtest_".$platform."\\$platform.cw88");       
    system("copy /Y cruise_confs\\".$platform."_cw88.xml D:\\vv_tools\\af\\CruiseControl\\config_xml\\int\\config_int_".$platform."_cw88.xml");
}

# if project list for cw10 changed
if( $regen_proj_cw10 )
{
    # create new project list content and copy it
    checkpoint( $log, (open FILE, '>', "projects\\$platform.cw10"), 
                "10;open_prjlist_cw10;Project list $platform.cw10 opened", 
                "10;open_prjlist_cw10;Cannot open project list $platform.cw10" );
    foreach(@projects_cw10) { print FILE "$_\n"; }
    close FILE; 
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\logs\\ar_int_".$platform."_cw10");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_cw10");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_cw10\\target");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_cw10\\target\\test-results");
}
# regenerate cw10 config xml
if( $regen_proj_cw10 || $exists_proj_cw10 )
{
    print_config_xml("cruise_confs\\".$platform."_cw10.xml", $platform, "cw10", \@projects_cw10);
    system("copy projects\\$platform.cw10 ".$VIEWS_PATH."\\zcz01-1107_view_buildtest_".$platform."\\$platform.cw10");
    system("copy /Y cruise_confs\\".$platform."_cw10.xml D:\\vv_tools\\af\\CruiseControl\\config_xml\\int\\config_int_".$platform."_cw10.xml");
}

# if project list for iar changed
if( $regen_proj_iar )
{
    # create new project list content and copy it
    checkpoint( $log, (open FILE, '>', "projects\\$platform.iar") ,
                "11;open_prjlist_iar;Project list $platform.iar opened", 
                "11;open_prjlist_iar;Cannot open project list $platform.iar" );
    foreach(@projects_iar)  { print FILE "$_\n"; }
    close FILE; 
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\logs\\ar_int_".$platform."_iar");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_iar");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_iar\\target");
    system("mkdir D:\\vv_tools\\af\\CruiseControl\\projects\\ar_int_".$platform."_iar\\target\\test-results");
}
# regenerate iar config xml
if( $regen_proj_iar || $exists_proj_iar )
{
    print_config_xml("cruise_confs\\".$platform."_iar.xml", $platform, "iar", \@projects_iar);
    system("copy projects\\$platform.iar ".$VIEWS_PATH."\\zcz01-1107_view_buildtest_".$platform."\\$platform.iar");
    system("copy /Y cruise_confs\\".$platform."_iar.xml D:\\vv_tools\\af\\CruiseControl\\config_xml\\int\\config_int_".$platform."_iar.xml");
}

checkpoint( $log, 1, "12;end;End" );

