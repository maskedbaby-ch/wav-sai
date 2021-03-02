######################################################################################
######################################################################################
######################################################################################
# @file vcast_report.py
# @version 1.0.0

# \brief VectorCAST report generator
# \project Plugins
# \author Damian Nowok
# \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

# @lastmodusr b22656
# @lastmoddate Sep-20-2012

# Freescale Semiconductor Inc.
# (c) Copyright 2012 Freescale Semiconductor Inc.
# ALL RIGHTS RESERVED.

######################################################################################
################################### Functions ########################################
######################################################################################
def run(search_dir, board_dir):

    projects = findVCASTprojects(search_dir)
    generateAllReports(projects, board_dir)


def generateAllReports(projects, board_dir):

    import os

    for project in projects:
        project_dir = os.path.dirname(project)
        project_name = os.path.splitext(os.path.basename(project))[0]
        generateALLReportsForProject(project_dir,project_name, board_dir)
        if board_dir is not None:
            createBoardRecord(project_dir, board_dir)



def generateALLReportsForProject(project_dir, project_name, board_dir):

    import subprocess
    import os
    import config

    report_dir = os.path.join(project_dir, config.REPORT_DIR)
    if not os.path.exists(report_dir):
        os.makedirs(report_dir)

    for report_file in config.REPORT_FILES:
        report_type = os.path.splitext(report_file)[0]
        generateReportForProject(report_type, report_file, project_dir, project_name)
        zipReports(project_dir)

def createBoardRecord(project_dir, board_dir):

    import os

    vcast_list = os.path.join(board_dir, 'vcast_project.lst')
    if os.path.isfile(vcast_list):
        f=open(vcast_list, 'a')
    else:
        f=open(vcast_list, 'w')

    f.write(project_dir + "\n")
    f.close()

def generateReportForProject(report_type, report_file,project_dir, project_name):

    import config, os, subprocess

    command = config.VCAST_COMMAND + project_name + ' ' + config.REPORT_COMMAND + ' ' + report_type + ' ' + \
        os.path.join(project_dir, config.REPORT_DIR, report_file)
    print command
    vcast_error = subprocess.call(command,shell=True,cwd=project_dir)
    if vcast_error != 0:
        print "Command \"" + command + "\" caused an error"


def zipReports(project_dir):

    import zipfile, os
    import config

    reports = os.listdir(os.path.join(project_dir, config.REPORT_DIR))
    reports = [r for r in reports if r in config.REPORT_FILES]
    zip_file = zipfile.ZipFile(os.path.join(project_dir, config.REPORT_DIR) +'.zip', 'w', zipfile.ZIP_DEFLATED)
    for report in reports:
        full_name = os.path.join(project_dir, config.REPORT_DIR, report)
        zip_file.write(full_name,report)
    zip_file.close()


def findVCASTprojects(path):

    import os

    projects = []
    for root, dirs, files in os.walk(path):
        if not root.endswith('vcast'):
            continue
        for f in files:
            name, ext = os.path.splitext(f)
            if ext == '.vcp' and name in dirs:
                projects.append(root.replace('\\','/') + '/' + f)

    return projects


######################################################################################
################################### Main ############################################
######################################################################################
if __name__ == '__main__':

    import argparse
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='VectorCAST code coverage analysis tool - Report generator.')
    parser.add_argument('-s','--search_dir', metavar='<coverage data dir>', type=str, dest='search_dir', required=True, help='Where to look for VCAST project.')
    parser.add_argument('-b','--board_dir', metavar='<board dir>', type=str, dest='board_dir', required=False, help='dir to DashBoard.')
    args = parser.parse_args()

    run(args.search_dir, args.board_dir)

