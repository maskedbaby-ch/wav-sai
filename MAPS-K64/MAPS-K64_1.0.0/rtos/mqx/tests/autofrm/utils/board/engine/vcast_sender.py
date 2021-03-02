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

class VcastSenderException(SenderException): pass

class VcastSender(Sender):

    def __init__(self, args):

        super(VcastSender, self).__init__(args.ip, args.port, args.silent, args.dev_dir)


    def run(self, list_file):
        try:
            project_paths = open(list_file).read().splitlines()
        except Exception as error:
            raise VcastSenderException("Error during project list file reading\n" + str(error))

        for project_path in project_paths:
            frame = self._prepareFrame(project_path)
            self._communication(frame)

        self._finish()

    def readConfig(self, project_path):

        import os

        result = {}
        config_file = os.path.join(project_path, 'config.cfg')
        config_lines = open(config_file).read().splitlines()
        for line in config_lines:
            arg, value = line.split(':')
            result[arg] = value
        return result

    def readData(self, project_path):

        import os, base64

        data_file = os.path.join(project_path, 'reports.zip')
        data = open(data_file, 'rb').read()
        data = base64.b64encode(data)

        return data

    def _prepareFrame(self, project_path):

        import config, os
        import pickle

        configuration = self.readConfig(project_path)
        branch, commit = self._getGitInfo()
        frame = et.Element(config.XML_TEST_SENDER_ROOT_TAG)
        frame.set(config.XML_TYPE, config.XML_VCAST)

        cnfg = et.Element(config.XML_TEST_CONFIG_TAG)
        cnfg.set(config.XML_PLATFORM, configuration['board'])
        cnfg.set(config.XML_COMPILER, configuration['tool'])
        cnfg.set(config.XML_MODULE, configuration['alt_config'])
        cnfg.set(config.XML_CONFIG, configuration['config'])
        cnfg.set(config.XML_CLASS, config.XML_VCAST)
        cnfg.set(config.XML_GIT_BRANCH, branch)
        cnfg.set(config.XML_GIT_COMMIT, commit)
        frame.append(cnfg)

        data = self.readData(project_path)
        data_element = et.Element(config.XML_DATA)
        data_element.text=data
        frame.append(data_element)

        return frame

