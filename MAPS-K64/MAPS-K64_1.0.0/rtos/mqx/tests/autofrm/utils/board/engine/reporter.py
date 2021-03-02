#-------------------------------------------------------------------------------
# Name:        Reporter
# Purpose:
#
# Author:      B36573
#
# Created:     01.03.2013
# Copyright:   (c) B36573 2013
# Licence:     <your licence>
#-------------------------------------------------------------------------------

from common.log import display

class Reporter(object):

    def __init__(self):

        import threading

        self.web_interface = None
        self.exit_flag = threading.Event()

    def startWebInterface(self, ip, port):

        import os
        import subprocess, threading

        display.output.log("Starting Web interface",0)
        self.web_interface = subprocess.Popen(
                                        ["python","manage.py","runserver",ip+":"+str(port)],
                                        creationflags=subprocess.CREATE_NEW_PROCESS_GROUP,
                                        stdout=subprocess.PIPE,
                                        stderr=subprocess.STDOUT,
                                        shell=True,
                                        universal_newlines=True,
                                        cwd=os.path.join(os.getcwd(),"web_interface")
                                        )

        log_monitor = threading.Thread(target=self._webInterfaceLog)
        log_monitor.daemon = False
        log_monitor.start()

    def stopWebInterface(self):

        import signal

        display.output.log("Stopping Web interface.",0)
        self.web_interface.send_signal(signal.CTRL_BREAK_EVENT)

    def startEngine(self, ip, port):

        from engine.receiver import Receiver

        display.output.log("Starting engine",0)
        self.receiver = Receiver()
        self.receiver.start(ip,port)

    def stopEngine(self):

        display.output.log("Stopping engine",0)
        self.receiver.stop()

    def executeForever(self):

        import threading

        user_interface = threading.Thread(target=self._userInterface)
        user_interface.daemon = True
        user_interface.start()

        self.exit_flag.wait()

    def _webInterfaceLog(self):

        while True:
            if self.exit_flag.is_set(): break
            line = self.web_interface.stdout.readline().strip()
            if line != "" and line != "Quit the server with CTRL-BREAK.":
                display.output.log(line,4)

    def _userInterface(self):

        from common import tools

        self._displayHelp()

        while True:
            key = tools.waitForKey()
            if key == 27 or key == 3 or key == 24 or key == 26: #ECS key has 27 value
                display.output.log("Exiting...")
                self.exit_flag.set()
                break
            else:
                self._displayHelp()

    def _displayHelp(self):

        display.output.log("Press ESC (or ctrl + c) key to exit.")