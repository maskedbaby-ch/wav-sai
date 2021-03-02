######################################################################################
######################################################################################
######################################################################################
# @file rrclient.py
# @version 1.1.0

# \brief Remote Run client main file.
# \project Remote Run
# \author Damian Nowok
# \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

# @lastmodusr b22656
# @lastmoddate Feb-27-2012

# Freescale Semiconductor Inc.
# (c) Copyright 2012 Freescale Semiconductor Inc.
# ALL RIGHTS RESERVED.
######################################################################################
######################################################################################
######################################################################################
from xml.etree import cElementTree as et
from engine.sender import Sender
from engine.sender import SenderException
######################################################################################
################################### Classes ##########################################
######################################################################################

class ResultSenderException(SenderException): pass

class ResultSender(Sender):

    def __init__(self,args):
        import threading

        self.platform = args.platform
        self.module = args.module
        self.configuration = args.config
        self.compiler = args.compiler
        self.apps = args.apps
        self.xml_dir = args.xml_dir
        self.target = args.target
        self.classif = args.classif

        super(ResultSender, self).__init__(args.ip, args.port, args.silent, args.dev_dir)

    def run(self):
        frame = self._prepareFrame()
        self._communication(frame)
        self._finish()


    def _prepareFrame(self):


        import config, os, re

        frame = et.Element(config.XML_TEST_SENDER_ROOT_TAG)
        frame.set(config.XML_TYPE, config.XML_EXEC_TEST)

        branch, commit = self._getGitInfo()

        cnfg = et.Element(config.XML_TEST_CONFIG_TAG)
        cnfg.set(config.XML_PLATFORM, self.platform)
        cnfg.set(config.XML_COMPILER, self.compiler)
        cnfg.set(config.XML_MODULE, self.module)
        cnfg.set(config.XML_CONFIG, self.configuration)
        cnfg.set(config.XML_CLASS, self.classif)
        cnfg.set(config.XML_GIT_BRANCH, branch)
        cnfg.set(config.XML_GIT_COMMIT, commit)
        frame.append(cnfg)

        os.chdir(self.xml_dir)
        for app in self.apps:
            file_name = '_'.join([self.module, app, self.platform, self.compiler, self.configuration])
            configs = [ c.split('.')[0] for c in os.listdir(self.xml_dir) if c.endswith('.xml') and re.match(file_name,c) ]
            configs = [ c.replace(file_name + '_','') for c in configs ]

            if len(configs)>0:
                for c in configs:
                    suites = et.Element(config.XML_TEST_SUITES_TAG, app_name=app + '_' + c)
                    xml_file = file_name + '_' + c + '.xml'
                    tree = et.parse(xml_file)
                    nodes  = tree.findall(config.XML_TEST_SUITE_TAG)
                    for node in nodes:
                        suites.append(node)
                    frame.append(suites)
            else:
                suites = et.Element(config.XML_TEST_SUITES_TAG, app_name=app)
                suite = et.SubElement(suites,config.XML_TEST_SUITE_TAG)
                case = et.SubElement(suite, config.XML_TEST_CASE_TAG)
                failure = et.SubElement(case,config.XML_TEST_FAILIURE_TAG).text="Result file not found"
                frame.append(suites)
        return frame
