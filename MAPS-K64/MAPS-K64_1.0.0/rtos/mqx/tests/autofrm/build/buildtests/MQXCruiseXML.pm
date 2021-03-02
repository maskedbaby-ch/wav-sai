package MQXCruiseXML;

use strict;
use vars qw( @ISA @EXPORT @EXPORT_OK );

use MQXUtils;

require Exporter;

@ISA = qw( Exporter AutoLoader );

@EXPORT = qw( print_ps_config_xml
              print_pk_config_xml
              print_config_xml
              report_header 
              report_footer 
              report_summary 
              suite_header
              suite_footer 
              test_case_success    
              test_case_failure
              test_case_error
              print_sys_out
              parse_clearcase_output
              parse_iar_output
              parse_cwclassic_output
              parse_cw10_output );
################################################################################
# Begin Crusise Configs                                                        #
################################################################################
sub print_ps_config_xml
{
    my $path     = shift;
    my $platform = shift;
    my $gen_cw63 = shift;
    my $gen_cw72 = shift;
    my $gen_cw10 = shift;
    my $gen_iar  = shift;
    my $gen_cw88  = shift;
    
    open FILE, ">", $path or die "Can not open file \'$path\' to store configuration XML.";
    
    print FILE <<'END_OF_XML_CONFIG';
<?xml version="1.0" encoding="UTF-8"?>
<cruisecontrol>
    
    <property name="ts_class"         value="int"/>
    <property name="project_compiler" value="keeper"/>
END_OF_XML_CONFIG
    
    print FILE "    <property name=\"project_platform\" value=\"".$platform."\"/>\n";
    print FILE "    <property name=\"project_module\"   value=\"".$platform."\"/>\n";
    
    print FILE <<'END_OF_XML_CONFIG';
    <property name="project_name"     value="ar_${ts_class}_${project_module}_${project_compiler}"/>
    <property name="platform_viewdir" value="D:\cc\zcz01-1107_view_buildtest_${project_platform}" />
    
    <project name="${project_name}">
    
        <listeners>
            <currentbuildstatuslistener file="logs/${project.name}/status.txt"/>
        </listeners> 
        
        <modificationset quietperiod="30">
            <clearcase branch="main" viewpath="${platform_viewdir}\MSGSW\MQX\src" />
        </modificationset>
        
        <schedule interval="30">
            
            <composite>
                
                <!-- Synch to Clear Case -->
                <exec command="cleartool" args="update -force -log NUL ${platform_viewdir}"/>
                
                <!-- Order build -->
END_OF_XML_CONFIG
    	
    print FILE "                <exec command=\"touch\" args=\"\${platform_viewdir}\\build_cw63\" />\n" if( $gen_cw63 > 0 ); 
    print FILE "                <exec command=\"touch\" args=\"\${platform_viewdir}\\build_cw72\" />\n" if( $gen_cw72 > 0 );
    print FILE "                <exec command=\"touch\" args=\"\${platform_viewdir}\\build_cw10\" />\n" if( $gen_cw10 > 0 );
    print FILE "                <exec command=\"touch\" args=\"\${platform_viewdir}\\build_iar\" />\n"  if( $gen_iar > 0 );
    print FILE "                <exec command=\"touch\" args=\"\${platform_viewdir}\\build_cw88\" />\n" if( $gen_cw88 > 0 );
    
    print FILE <<'END_OF_XML_CONFIG';                
            </composite>            
            
        </schedule>
        
        <log>
            <merge dir="projects/${project.name}/target/test-results"/>
        </log>
        
        <publishers>
            
        </publishers>
        
    </project>
</cruisecontrol>
END_OF_XML_CONFIG

    close FILE;
}

sub print_pk_config_xml
{
    my $path = shift;
    
    my $cfg = get_cfg();
    
    open FILE, ">", $path or die "Can not open file \'$path\' to store configuration XML.";
    
    print FILE <<'END_OF_XML_CONFIG';
<?xml version="1.0" encoding="UTF-8"?>
<cruisecontrol>
    
    <property name="ts_class"         value="int"/>
    <property name="project_compiler" value="keeper"/>
    <property name="project_platform" value="platform"/>    
    <property name="project_module"   value="platform"/>
    <property name="project_name"     value="ar_${ts_class}_${project_module}_${project_compiler}"/>

END_OF_XML_CONFIG
    
    print FILE "    <property name=\"perl\"             value=\"".$cfg->{'PERL_INTERPRETER'}."\" />\n";
    print FILE "    <property name=\"keeper_viewdir\"   value=\"".$cfg->{'VIEW_DIR'}."\/".$cfg->{'PLATFORM_KEEPER_NAME'}."\" />\n";
    print FILE <<'END_OF_XML_CONFIG';
    
    <property name="scriptdir"        value="${keeper_viewdir}/MSGSW/MQX/src/tests/autofrm/build/buildtests" />
    <property name="initscript"       value="${scriptdir}/autobuild_init.pl" />    
    <property name="reportscript"     value="${scriptdir}/autobuild_repgen.pl" />
    
    <property name="logfile"          value="${cruise_dir}/projects/${project.name}/target/test-results/result.log" />
    <property name="reportfile"       value="${cruise_dir}/projects/${project.name}/target/test-results/platform.xml" />

END_OF_XML_CONFIG

    print FILE "    <property name=\"ecd_source\"       value=\"".$cfg->{'ECD_SRC'}."\" />\n";
    print FILE "    <property name=\"ecd_target\"       value=\"".$cfg->{'ECD_DST'}."\" />\n";
    print FILE <<'END_OF_XML_CONFIG';
    
    <project name="${project_name}">
    
        <listeners>
            <currentbuildstatuslistener file="logs/${project.name}/status.txt"/>
        </listeners> 
        
        <modificationset quietperiod="30">
END_OF_XML_CONFIG
        
    print FILE "            <clearcase branch=\"".$cfg->{'BRANCH'}."\" viewpath=\"\${scriptdir}\" />\n";
    print FILE <<'END_OF_XML_CONFIG';
            <timebuild time="2300" />    
        </modificationset>
        
        <schedule interval="30">
            
            <composite>
            
                <!-- Remove old test reports  -->
                <exec command="rm" args="-f ${cruise_dir}/projects/${project.name}/target/test-results/*.log"/>
                <exec command="rm" args="-f ${cruise_dir}/projects/${project.name}/target/test-results/*.xml"/>
                
                <!-- Synch to Clear Case -->
                <exec command="cleartool" args="update -force -log NUL ${keeper_viewdir}" />
                
                <!-- Copy ECD.EXE -->                
                <exec command="cp" args="&quot;${ecd_source}&quot; &quot;${ecd_target}&quot;" />
                
                <!-- Generate autobuild projects -->
END_OF_XML_CONFIG

    foreach my $pl ( @{$cfg->{'PLATFORMS'}} )
    {
        print FILE "                <exec command=\"\${perl}\" args=\"\${initscript} -l \${logfile} -p ".$pl."\" workingdir=\"\${scriptdir}\" />\n";
    }

    print FILE <<'END_OF_XML_CONFIG';
                
                <exec command="${perl}" args="${reportscript} ${logfile} ${reportfile}" workingdir="${scriptdir}" />
                
            </composite>            
            
        </schedule>
        
        <log>
            <merge dir="projects/${project.name}/target/test-results"/>
        </log>
        
        <publishers>
            
        </publishers>
        
    </project>
</cruisecontrol>

END_OF_XML_CONFIG

    close FILE;
}

sub print_config_xml
{
    my $output_path = shift;
    my $platform    = shift;
    my $compiler    = shift;
    my $projects    = shift;
    
    my $cfg = get_cfg();
    
    open FILE, ">", $output_path or die "Can not open file \'$output_path\' to store configuration XML.";
    
    print FILE <<'END_OF_XML_CONFIG';
<?xml version="1.0" encoding="UTF-8"?>
<cruisecontrol>
    <property name="ts_class" value="int"/>
END_OF_XML_CONFIG
    
    print FILE "    <property name=\"project_compiler\" value=\"$compiler\"/>\n";
    print FILE "    <property name=\"project_platform\" value=\"$platform\"/>\n";
    print FILE "    <property name=\"project_module\" value=\"$platform\"/>\n";
    print FILE "    <property name=\"project_name\" value=\"ar_\${ts_class}_\${project_module}_\${project_compiler}\"/>\n";
    print FILE "    <property name=\"perl\" value=\"".$cfg->{'PERL_INTERPRETER'}."\" />\n";
    print FILE "    <property name=\"keeper_viewdir\"   value=\"".$cfg->{'VIEW_DIR'}."\\".$cfg->{'PLATFORM_KEEPER_NAME'}."\" />\n";
    print FILE "    <property name=\"platform_viewdir\" value=\"".$cfg->{'VIEW_DIR'}."\\zcz01-1107_view_buildtest_\${project_platform}\" />\n";
    
    print FILE <<'END_OF_XML_CONFIG';
    
    <property name="compilerdir"      value="${keeper_viewdir}/MSGSW/MQX/src/tests/autofrm/build/buildtests" />    
    <property name="compilerscript"   value="${compilerdir}/autobuild_compile.pl" />
    
    <project name="${project_name}" buildafterfailed="false">
        <listeners>
            <currentbuildstatuslistener file="logs/${project.name}/status.txt"/>
        </listeners> 
        
        <modificationset quietperiod="30">
END_OF_XML_CONFIG

    print FILE "            <filesystem folder=\"\${platform_viewdir}\\build_".$compiler."\" />\n";

    print FILE <<'END_OF_XML_CONFIG';
        </modificationset>
        
        <schedule interval="30">
            <composite>
                <!-- Remove old test reports  -->
                <exec command="rm" args="-f ${cruise_dir}/projects/${project.name}/target/test-results/*.xml"/>
                <exec command="rm" args="-f ${compilerdir}/build_logs/*.*" />
                                
END_OF_XML_CONFIG

    foreach(@$projects)
    {
        #s/\\/\\\\/g;
        #s/\//\/\//g;
        next if( /build_.*_libs.mcp/ );
        print FILE '                ';
        print FILE '<exec command="${perl}" args="${compilerscript} -p ${platform_viewdir}';
        print FILE $_;
        print FILE ' -c ${project_compiler}" workingdir="${compilerdir}" />';
        print FILE "\n";    
    }
    
    print FILE <<'END_OF_XML_CONFIG';
                
                <exec command="cp" args="build_logs/* ${cruise_dir}/projects/${project.name}/target/test-results" workingdir="${compilerdir}" />
END_OF_XML_CONFIG

    if( $compiler =~ /cw10/ )
    {    
        print FILE "                <exec command=\"rm\" args=\"-rf &quot;D:\\Profiles\\zcz01vav01-svc\\workspace&quot;\" workingdir=\"\${compilerdir}\" />";            
    }
    
print FILE <<'END_OF_XML_CONFIG';

            </composite>
        </schedule>
        
        <log>
            <merge dir="projects/${project.name}/target/test-results"/>
        </log>
        
        <publishers>
            <execute command="sh ${frm_dir}/testfrm/tools/report_gen/TestReportGen.sh ${frm_dir} ${project.name} ${dev_dir} "/>
        </publishers>
        
    </project>
</cruisecontrol>
END_OF_XML_CONFIG
    
    close FILE;
}

################################################################################
# End Crusise Configs                                                          #
################################################################################

my @tmp;
my $system_out_file;

# Generate JUnit report file header
sub report_header {
    my @ret = ();
    push( @ret, "<?xml version='1.0' encoding='UTF-8'?>" );
    push( @ret, "<testsuites>" );
    return @ret;
}

# Generate JUnit report file footer
sub report_footer {
    my @ret = ();
    push( @ret, "</testsuites>" );
    return @ret;
}

# Generate JUnit report summary (used by CruiseControl)
#
# arg0 - total failures
# arg1 - total tests
#
sub report_summary {
    my @ret = ();
    push( @ret, "<!-- <testsummary  failures=\"$_[0]\" tests=\"$_[1]\" /> -->" );
    return @ret;
}

# Generate JUnit test suite header
#
# arg0 - name of testsuite
# arg1 - number of errors
# arg2 - number of failures
# arg3 - number of tests
# arg4 - time
#
sub suite_header {
    my @ret = ();
    push( @ret, "  <testsuite name=\"$_[0]\" errors=\"$_[1]\" failures=\"$_[2]\" tests=\"$_[3]\" time=\"$_[4]\">");
    return @ret;
}


# Generate JUnit test suite footer
sub suite_footer {
    my @ret = ();
    push( @ret, "  </testsuite>" );
    return @ret;
}

# Push JUnit test case success to @tmp var
#
# arg0 - classname
# arg1 - name
# arg2 - time
# arg3 - testcase text
#
sub test_case_success {
    my @ret = ();
    push(@ret, "    <testcase classname=\"$_[0]\" name=\"$_[1]\" time=\"$_[2]\">$_[3]</testcase>");
    return @ret;
}

# Push JUnit test case failure to @ret var and returns it
#
# arg0 - classname
# arg1 - name
# arg2 - time
# arg3 - failure type
# arg4 - testcase text
#
sub test_case_failure {
    my @ret = ();
    push(@ret, "    <testcase classname=\"$_[0]\" name=\"$_[1]\" time=\"$_[2]\">");
    push(@ret, "      <failure message=\"failure message\" type=\"$_[3]\">$_[4]</failure>");
    push(@ret, "    </testcase>");
    return @ret;
}

# Push JUnit test case error to @ret var and returns it
#
# arg0 - classname
# arg1 - name
# arg2 - time
# arg3 - failure type
# arg4 - testcase text
#
sub test_case_error {
    my @ret = ();
    push(@ret, "    <testcase classname=\"$_[0]\" name=\"$_[1]\" time=\"$_[2]\">");
    push(@ret, "      <error message=\"error message\" type=\"$_[3]\">$_[4]");
    push(@ret, "      </error>");
    push(@ret, "    </testcase>");
    return @ret;
}

# Generate property node
#sub print_properties {
#    if (%property)
#    {
#        print "    <properties>\n";
#            while (($name, $value) = each(%property))
#            {
#                print "      <property name=\"$name\" value=\"$value\" />\n";
#            }
#        print "    </properties>\n";
#    }
#}

# Generate system-out node
sub print_sys_out {
        print "    <system-out><![CDATA[\n";
        if (open(SYS_OUT_FILE, $system_out_file))
        {
            print <SYS_OUT_FILE>;
            close(SYS_OUT_FILE);
        }
        print "      ]]></system-out>\n";
}

sub parse_iar_output
{
    
    
    #return;
}

sub parse_cwclassic_output
{
}

sub parse_cw10_output
{
}

sub parse_clearcase_output
{
    my $filename  = shift @_;
    my $error_arr = shift @_;
    my $error_cnt = 0;
    
    open FILE, '<', $filename;
    
    while(<FILE>) { if( /# Actions taken to update the view:/ )    { <FILE>; last; } }
    
    while(<FILE>) 
    {
        if( /^NonfileError:/ )
        {
            $error_cnt++;
            push( @$error_arr, $_ );
        }
        elsif( /^#/ )
        {
            last;
        }
    }    
    return $error_cnt;    
}

1;