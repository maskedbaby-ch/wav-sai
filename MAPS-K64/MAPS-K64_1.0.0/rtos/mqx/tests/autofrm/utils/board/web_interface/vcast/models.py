#-------------------------------------------------------------------------------
# Name:        models
# Purpose:
#
# Author:      B36573
#
# Created:     06.03.2013
# Copyright:   (c) B36573 2013
# Licence:     <your licence>
#-------------------------------------------------------------------------------

import os, cPickle, config
from xml.etree import cElementTree as et

REPORTS_TO_HANDLE = ['vcast']

class AutoVivification(dict):
    """Implementation of perl's autovivification feature."""
    def __getitem__(self, item):
        try:
            return dict.__getitem__(self, item)
        except KeyError:
            value = self[item] = type(self)()
            return value

class ReportGetter:

    @staticmethod
    def get(timestamp, branch):

        from common import tools

        global_results = AutoVivification()
        global_runs = []
        global_branches = []

        timestamp = tools.getTimestamp(timestamp)

        global_runs = os.listdir(config.OUTPUT_FOLDER)
        global_runs.sort(reverse=True)

        if timestamp not in global_runs:
            return global_results, global_runs, timestamp, global_branches, None

        global_branches = os.listdir(os.path.join(config.OUTPUT_FOLDER, timestamp))


        for branch in reversed(global_branches):
            branch_dir = os.path.join(config.OUTPUT_FOLDER, timestamp, branch)

            dirs = [ name for name in os.listdir(branch_dir) if os.path.isdir(os.path.join(branch_dir, name)) ]
            dirs = [ name for name in dirs if name.startswith(tuple(REPORTS_TO_HANDLE)) ]

            if dirs == []:
                global_branches.remove(branch)

        if global_branches == []:
            return global_results, global_runs, timestamp, global_branches, None

        branch = tools.getGitBranch(branch, global_branches)
        branch_dir = os.path.join(config.OUTPUT_FOLDER, timestamp, branch)

        dirs = [ name for name in os.listdir(branch_dir) if os.path.isdir(os.path.join(branch_dir, name)) ]
        dirs = [ name for name in dirs if name.startswith(tuple(REPORTS_TO_HANDLE)) ]

        for dir_name in dirs:
            result_path = os.path.join(branch_dir,dir_name)
            ccov, platform, compiler, cnfg, alt_cnfg = dir_name.split(config.DELIMITER)
            global_results[platform][compiler][cnfg][alt_cnfg] = result_path.replace('\\','/')

        return global_results, global_runs, timestamp, global_branches, branch
