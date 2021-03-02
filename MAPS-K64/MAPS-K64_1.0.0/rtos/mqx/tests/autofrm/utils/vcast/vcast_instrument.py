######################################################################################
######################################################################################
######################################################################################
# @file vcast_instrument.py
# @version 1.0.0

# \brief VectorCAST - Code coverage analysis - Instrumentation
# \project VectorCAST
# \author Vaclav Hamersky
# \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

# @lastmodusr B21889
# @lastmoddate September-11-2012

# Freescale Semiconductor Inc.
# (c) Copyright 2012 Freescale Semiconductor Inc.
# ALL RIGHTS RESERVED.
######################################################################################
################################## Imports ###########################################
######################################################################################

import time
import argparse
import sys
import os
import string
import config

######################################################################################
############### Parsing input parameters #############################################
######################################################################################
def ParseInput():

    print "# Parsing input parameters...",
    parser = argparse.ArgumentParser(description='VectorCAST code coverage analysis tool - Instrumentation.')
    parser.add_argument('--instr_type', metavar='<inst type>', type=str, dest='instr_type', required=False, default='Statement+Branch', help='type of instrumentation: Statement|default: Statement+Branch.')
    parser.add_argument('--toolchain', metavar='<toolchain name>', type=str, dest='toolchain', required=True, help='compiler name.')
    parser.add_argument('--toolchain_dir', metavar='<path to compiler>', type=str, dest='toolchain_dir', required=True, help='path to compiler.')
    parser.add_argument('--compile_options', metavar='<compile options>', type=str, nargs='+', dest='compile_options', required=True, help='compiler options and flags.')
    parser.add_argument('--compile_defines', metavar='<compile defines>', type=str, nargs='+', dest='compile_defines', required=True, help='defines for compilers without -D flag.')
    parser.add_argument('--source_files', metavar='<source files>', type=str, dest='source_files', required=True, help='source files list.')
    parser.add_argument('--library', metavar='<current library>', type=str, dest='library', required=True, help='source files list.')
    parser.add_argument('--make_file_path', metavar='<make file path>', type=str, dest='make_file_path', required=True, help='used makefile.')
    parser.add_argument('--vc_project_name', metavar='<vc project name>', type=str, dest='vc_project_name', required=True, help='project name.')
    parser.add_argument('--vc_work_dir', metavar='<vc work dir>', type=str, dest='vc_work_dir', required=True, help='VectorCAST work dir.')
    parser.add_argument('--dev_dir', metavar='<mqx root dir>', type=str, dest='dev_dir', required=True, help='MQX root dir.')
    parser.add_argument('--keep_sources_inplace', metavar='<keep sources inplace>', type=str, dest='keep_sources_inplace', required=False, default='Y', help='Instrument source files in place [Y|N].')
    parser.add_argument('--filter_library', metavar='<library to instrument>', type=str, dest='filter_library', required=False, default='', help='Library to instrument')
    parser.add_argument('--filter_out', metavar='<condition not to instrument file>', type=str, dest='filter_out', required=False, default='', help='Condition not to instrument file')
    parser.add_argument('--filter_in', metavar='<condition to instrument file>', type=str, dest='filter_in', required=False, default='', help='Condition to instrument file')

    args = parser.parse_args()
    print "OK"
    return args

######################################################################################
############ Generate configuration file #############################################
######################################################################################
def GenerateConfig(args):

    from django.conf import settings
    from django.template import loader, Context

    print "# Creating configuration file...",

    config_file = os.path.join(args.vc_work_dir,config.VC_CFG_FILE)
    include_file = os.path.join(args.vc_work_dir,config.INCLUDE_PATHS_LIST)

    # if config file already exists, nothing to do here
    if os.path.isfile(config_file):
        return 0

    try:
        include_paths = open(include_file).read().splitlines()
    except Exception as error:
        print "\nError occured during file read\n" + str(error)
        return 1

    data = {
        'toolchain' : args.toolchain,
        'toolchain_dir' : args.toolchain_dir,
        'include_paths' : include_paths,
        'ccflags' : args.compile_options,
        'ccdefines' : args.compile_defines,
        'vc_work_dir' : args.vc_work_dir,
        'vc_project_name' : args.vc_project_name
    }

    try:
        settings.configure(DEBUG=True, TEMPLATE_DEBUG=True,
            TEMPLATE_DIRS=(os.path.join(os.path.dirname(os.path.realpath(__file__)), 'templates'),))
        ctx = Context(data)
        t = loader.get_template(config.TEMPLATE_FILE)
        output = t.render(ctx)
    except Exception as error:
        print "\nError occured during template handling\n" + str(error)
        return 2

    try:
        open(config_file,'w').write(output)
    except Exception as error:
        print "\nError occured during config file generation\n" + str(error)
        return 3

    print "OK"
    return 0

######################################################################################
############ Filter sources for instrumentation#######################################
######################################################################################
def FilterSources(args):

    import re

    print "# Filtering source files for instrumentation...",

    sources = args.source_files.split()
    library = args.library

    if args.filter_library != '':
        if library != args.filter_library:
            print "\n# Library " + library + " is not ment to be instrumented"
            return 1, []

    # filter out prohibided files
    sources = [s for s in sources if os.path.basename(s) not in config.FORBIDEN_FILES]

    if args.filter_in != '':
        filter_in = '.*' + args.filter_in + '.*'
        filter_in_regex = re.compile(filter_in)
        sources = [s for s in sources if filter_in_regex.match(s)]

    if args.filter_out != '':
        filter_out = '.*' + args.filter_out + '.*'
        filter_out_regex = re.compile(filter_out)
        sources = [s for s in sources if not filter_out_regex.match(s)]

    if sources.count == 0:
        print "\n# No source files matches the filter:"
        print "# IN filter: " + filter_in
        print "# OUT filter: " + filter_out
        return 2, []

    print "OK"
    return 0, sources

######################################################################################
####################### Copy files for instrumentation################################
######################################################################################
def CopyFiles(args, files_to_instrument):

    import shutil

    print "# Copying files for instrumentation...",
    try:
        for f in files_to_instrument:
            to = f.replace(args.dev_dir, args.vc_work_dir + '/sources')
            if not os.path.exists(os.path.dirname(to)):
                os.makedirs(os.path.dirname(to))
            shutil.copy2(f,to)
    except Exception as error:
        print "# Error occured during file copying\n# " + str(error)
        return 1

    print "OK"
    return 0

######################################################################################
####################### Change .mak file #############################################
######################################################################################
def ChangeMakFile(args, files_to_instrument):

    print "# Changing mak file...",

    in_mak = args.make_file_path
    out_mak = in_mak + ".bak"

    files = [f.replace(args.dev_dir,'') for f in files_to_instrument]
    line_to_add = "\nCCOV_SOURCE=$(call cygpath, " + args.vc_work_dir + "/sources)\n\n"

    try:
        out_file = open(out_mak, 'w')
        path_added = 0
        with open(in_mak) as in_file:
            for line in in_file:
                if not path_added and line=='\n':
                    out_file.writelines(line_to_add)
                    path_added = 1
                    continue
                if line.startswith('SRC_FILES'):
                    line = line.rstrip('\n')
                    if line.endswith(tuple(files)):
                        line = line.replace('DEV_DIR', 'CCOV_SOURCE')
                    line = line + '\n'
                out_file.writelines(line)
        in_file.close()
        out_file.close()

        os.remove(in_mak)
        os.rename(out_mak, in_mak)
    except Exception as error:
        print "\nError occured during mak modification\n" + str(error)
        return 1

    print "OK"
    return 0

######################################################################################
####################### Execute VectorCAST ###########################################
######################################################################################
def ExecuteVectorCAST(args, files_to_instrument):

    print "# Executing VectorCAST..."

    try:
        batch_name = os.path.join(args.vc_work_dir,"run_" + args.library + ".bat")
        if os.path.exists(batch_name): os.remove(batch_name)
        batch = open(batch_name, "w")
    except Exception as error:
        print "# Error occured during batch file generation" + str(error)
        exit(1)

    project_name = args.vc_project_name
    in_place = args.keep_sources_inplace

    os.chdir(args.vc_work_dir)

    batch.write("@echo off\n")
    batch.write("REM Creating VCAST project\n")
    batch.write('"%VECTORCAST_DIR%\CLICAST" cover environment create ' + project_name + '\n')

    print "# Creating vcast project ... "
    os.system('"%VECTORCAST_DIR%\CLICAST" cover environment create ' + project_name)

    batch.write("REM Setting IN_PLACE option to " + in_place + '\n')
    batch.write('"%VECTORCAST_DIR%\CLICAST" /E:' + project_name + ' cover options in_place ' + in_place + '\n')

    print "# Setting IN_PLACE option to " + in_place
    os.system('"%VECTORCAST_DIR%\CLICAST" /E:' + project_name + ' cover options in_place ' + in_place)

    #os.system('"%VECTORCAST_DIR%\CLICAST" /E:' + project_name + ' cover options set_instrumentation_directory ' + args.vc_work_dir + '/sources/')

    batch.write("REM Setting PREPROCESSED_FILE option to Y\n")
    batch.write('"%VECTORCAST_DIR%\CLICAST" /E:' + project_name + ' cover options preprocessed_file Y\n')

    print "# Setting PREPROCESSED_FILE option to Y"
    os.system('"%VECTORCAST_DIR%\CLICAST" /E:' + project_name + ' cover options preprocessed_file Y ')

    for f in files_to_instrument:
        f = f.replace(args.dev_dir,args.vc_work_dir + "/sources")

        batch.write("REM Adding source file: " + f + "\n")
        batch.write('"%VECTORCAST_DIR%\CLICAST" /E:' + project_name + ' cover source add ' + f + '\n')
        print "# Adding source file: " + f
        os.system('"%VECTORCAST_DIR%\CLICAST" /E:' + project_name + ' cover source add ' + f)

        batch.write("REM Instrumenting: " + f + "\n")
        batch.write('"%VECTORCAST_DIR%\CLICAST" /E:' + project_name + ' -u ' + f + ' cover instrument ' + args.instr_type + '\n')
        print "# Instrumenting: " + f
        os.system('"%VECTORCAST_DIR%\CLICAST" /E:' + project_name + ' -u ' + f + ' cover instrument ' + args.instr_type)

    batch.close()
    return 0


######################################################################################
################################### Main #############################################
######################################################################################

if __name__ == '__main__':


    print "####################################### VectorCAST #######################################"

    # Parsing input parameters
    args = ParseInput()


    print "# Project name: %s" %(args.vc_project_name)
    print "# Work dir: %s" %(args.vc_work_dir)

    if not os.path.isfile(args.make_file_path):
        print '[!] Invalid path to test make: %s'%(args.make_file_path)
        exit(1)

    if not os.path.exists(args.vc_work_dir):
        print "[!] VectorCAST work dir doesn't exist"
        exit(2)

    # Filtering source files for instrumentation
    ret_value, files_to_instrument = FilterSources(args)
    if ret_value != 0:
        exit(0)

    # Creating configuration file
    ret_value = GenerateConfig(args)
    if ret_value != 0:
        exit(ret_value)

    # Copying files for instrumentation
    ret_value = CopyFiles(args, files_to_instrument)
    if ret_value != 0:
        exit(ret_value)

    # Changing mak file
    ret_value = ChangeMakFile(args, files_to_instrument)
    if ret_value != 0:
        exit(ret_value)

    # Executing VectorCAST
    ret_value = ExecuteVectorCAST(args, files_to_instrument)

    print "########################################## Done ##########################################"
