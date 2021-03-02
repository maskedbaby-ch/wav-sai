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
from common.log import display
from common.communication import commclient
from xml.etree import cElementTree as et
######################################################################################
################################### Classes ##########################################
######################################################################################

class SenderException(commclient.CommClientException): pass

class Sender(object):

    def __init__(self, ip, port, silent, dev_dir=None):
        import threading

        self.server_ip = ip
        self.server_port = port
        self.silent = silent
        self.dev_dir = dev_dir
        self.exit_flag = threading.Event()

        self.user_interface_thread = threading.Thread(target=self._userInterface)
        self.user_interface_thread.daemon = True
        self.user_interface_thread.start()

        if not (self.silent):
            display.output.log("Using " + self.server_ip + ":" + str(self.server_port),0)

    def _userInterface(self):
        """
            This function is responsible for user interface
            It read keyboard keys and performs actions

            Input:
            Nothing

            Return:
            Nothing
        """
        from common import keyboard

        while True:
            if not (self.silent):
                display.output.log("Press ESC (or ctrl + c) key to exit.")
            key = keyboard.waitForKey()
            if key == 27 or key == 3 or key == 24 or key == 26: #ECS key has 27 value
                if not (self.silent):
                    display.output.log("ESC was pressed. Exiting...")
                self.exit_flag.set()
                break

    def _getGitInfo(self, path=None):
        import config, os

        try:
            with open(config.GIT_INFO_FILE, "r") as infofile:
                line = infofile.readline()
                branch, commit = line.split()
                return branch, commit
        except Exception as error:
            from common import tools

            if path == None:
                path = self.dev_dir
            branch, commit = tools.getGitInfo(path)
            # with open(config.GIT_INFO_FILE, "w") as infofile:
            #     infofile.write(branch + " " + commit )
            return branch, commit


    def _prepareFrame(self):


        raise SenderException("_prepareFrame function is not defined")

    def _communication(self, frame):

        import config

        self.indent(frame)
        data_to_send = et.tostring(frame)
        if not (self.silent):
            display.output.log("Sending data: \n" + data_to_send,0)
        status, message = self._sendAndReceiveWithServerStatuCheck(data_to_send)
        if not (self.silent):
            display.output.log("Status from server: " + status,0)
            display.output.log("Message from server: " + message,0)
        if status != config.XML_OK_VALUE:
            raise SenderException(message)

    def _finish(self):
        """
            This functions finishes execution of Sender

            Input:
            Nothing

            Returns:
            Nothing
        """
        import sys

        display.output.signalExit()
        while not display.output.exited(): pass


    def _sendAndReceiveWithServerStatuCheck(self,send_data):
        """
            This function sends and receives data to server
            It also checks status of request.
            In case of restart it waits and resends data.

            Input
            send_data   - data to send

            Return:
            data received
        """
        import time
        from common.communication import commclient
        import config

        try:
            receive_data = commclient.sendAndReceive(self.server_ip,self.server_port,send_data)
        except commclient.CommClientException as error:
            raise SenderException(str(error))
        try:
            received_frame = et.fromstring(receive_data).findall(".")[0]
        except Exception as error:
            raise SenderException(str(error))

        status = received_frame.get(config.XML_STATUS_ATTR, config.XML_ERROR_VALUE)
        message = received_frame.get(config.XML_MSG_ATTR, "")

        return status, message

    def indent(self, elem, level=0):
        i = "\n" + level*"  "
        if len(elem):
            if not elem.text or not elem.text.strip():
                elem.text = i + "  "
            for e in elem:
                self.indent(e, level+1)
                if not e.tail or not e.tail.strip():
                    e.tail = i + "  "
            if not e.tail or not e.tail.strip():
                e.tail = i
        else:
            if level and (not elem.tail or not elem.tail.strip()):
                elem.tail = i
