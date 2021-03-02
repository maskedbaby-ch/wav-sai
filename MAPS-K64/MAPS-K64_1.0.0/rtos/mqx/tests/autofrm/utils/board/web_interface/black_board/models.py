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

REPORTS_TO_HANDLE = ['int', 'ext']

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
        global_modules = []
        global_runs = []
        global_branches = []

        timestamp = tools.getTimestamp(timestamp)

        os.chdir(config.OUTPUT_FOLDER)
        global_runs = os.listdir(config.OUTPUT_FOLDER)
        global_runs.sort(reverse=True)

        if timestamp not in global_runs:
            return global_results, global_modules, global_runs, global_branches, timestamp, None


        global_branches = os.listdir(os.path.join(config.OUTPUT_FOLDER, timestamp))
        branch = tools.getGitBranch(branch, global_branches)
        branch_dir = os.path.join(config.OUTPUT_FOLDER, timestamp, branch)

        dirs = [ name for name in os.listdir(branch_dir) if os.path.isdir(os.path.join(branch_dir, name)) ]
        dirs = [ name for name in dirs if name.startswith(tuple(REPORTS_TO_HANDLE)) ]

        for dir_name in dirs:
            result_path = os.path.join(branch_dir,dir_name)
            if not os.path.isdir(result_path):
                continue

            modules = os.listdir(result_path)

            for module in modules:
                file = et.parse(os.path.join(result_path,module))
                config_tag = file.find(".//" + config.XML_TEST_CONFIG_TAG)
                try:
                    platform = config_tag.attrib[config.XML_PLATFORM]
                    compiler = config_tag.attrib[config.XML_COMPILER]
                    cnfg = config_tag.attrib[config.XML_CONFIG]
                except Exception:
                    continue
                results = file.findall(".//" +config.XML_TEST_SUITE_TAG)
                total = failed = error = missing = 0
                module_results = {}
                for result in results:
                    try:
                        total += int(result.attrib[config.XML_TEST_FILE_TESTS_TAG])
                        failed += int(result.attrib[config.XML_TEST_FILE_FAILURES_TAG])
                        error += int(result.attrib[config.XML_TEST_FILE_ERRORS_TAG])
                    except Exception as error:
                        missing += 1

                module_results = {"test":total,"failures":failed,"errors":error, "missing":missing}
                module_name = module.split(".")[0]
                if not module_name in global_modules:
                    global_modules.append(module_name)
                global_results[platform][compiler][cnfg][module_name] = module_results

        return global_results, global_modules, global_runs, global_branches, timestamp, branch

if __name__ == '__main__':
    ReportGetter.get(None,None)