######################################################################################
######################################################################################
######################################################################################
# @file vcast_instrument.py
# @version 1.0.0

# \brief VectorCAST - Code coverage analysis - Result gathering
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
import shutil

def getCoverageData(path):

    import re

    file_list = os.listdir(path)
    coverage_regex = re.compile('vcast_coverage')

    file_list = [os.path.join(path,f) for f in file_list if coverage_regex.match(f)]
    consolidated_coverage_data=""

    for file in file_list:
        f = open(file, 'r')
        coverage_data=f.readline()
        coverage_data=f.readline()

        while ".." not in coverage_data:
            coverage_data=f.readline()
            read_data = string.split(coverage_data,'|')[1][:-1]
            if len(read_data)==63:
                read_data = read_data + ' '
            read_data = string.replace(read_data,'.','\n')
            consolidated_coverage_data=consolidated_coverage_data + read_data
        f.close()

    return consolidated_coverage_data

######################################################################################
################################### Main #############################################
######################################################################################
if __name__ == '__main__':

    # Parse command line arguments
    parser = argparse.ArgumentParser(description='VectorCAST code coverage analysis tool - Report.')
    parser.add_argument('--coverage_data_dir', metavar='<coverage data dir>', type=str, dest='coverage_data_dir', required=True, help='VectorCAST coverage data files location.')
    parser.add_argument('--vc_project_name', metavar='<vc project name>', type=str, dest='vc_project_name', required=True, help='project name.')
    parser.add_argument('--vc_test_name', metavar='<vc test name>', type=str, dest='vc_test_name', required=True, help='test name.')
    parser.add_argument('--vc_work_dir', metavar='<vc work dir>', type=str, dest='vc_work_dir', required=True, help='VectorCAST work dir.')
    parser.add_argument('-s', '--silent', action="store_true", dest='silent', required=False, help='silent mode.')
    args = parser.parse_args()

    print "####################################### VectorCAST #######################################"
    print "# Project name: %s" %(args.vc_project_name)
    print "# Work dir: %s" %(args.vc_work_dir)
    print "# Coverage data dir: %s" %(args.coverage_data_dir)
    print "######################################### Parser #########################################"

    os.chdir(args.vc_work_dir)

    coverage_data = getCoverageData(args.coverage_data_dir)

    coverage_data_dest = args.vc_work_dir + '\\results\\' + args.vc_test_name + '.dat'
    if not os.path.exists(os.path.dirname(coverage_data_dest)):
        os.makedirs(os.path.dirname(coverage_data_dest))

    file_updated=0
    try:
        if os.path.exists(coverage_data_dest): file_updated=1
        # shutil.copy2(args.coverage_data_file, coverage_data_dest)
        open(coverage_data_dest, 'w').write(coverage_data)
    except Exception as error:
        print "Error during result data copying\n" + str(error)
        exit(1)

    if file_updated==1:
        print "Updating old results..."
        os.system('"%VECTORCAST_DIR%\CLICAST" /E:' + args.vc_project_name + ' cover RESult Update ' + args.vc_test_name + ' ' + coverage_data_dest)
    else:
        print "Adding vcast coverage data ... "
        os.system('"%VECTORCAST_DIR%\CLICAST" /E:' + args.vc_project_name + ' cover RESult Add ' + coverage_data_dest + ' ' + args.vc_test_name)


