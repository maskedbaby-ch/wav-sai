#-------------------------------------------------------------------------------
# Name:        start
# Purpose:
#
# Author:      B36573
#
# Created:     01.03.2013
# Copyright:   (c) B36573 2013
# Licence:     <your licence>
#-------------------------------------------------------------------------------

if __name__ == '__main__':

    import config, sys, os
    from engine import reporter
    from common.log import display
    from common import tools

    app = reporter.Reporter()
    app.startWebInterface(config.WEB_IP,config.WEB_PORT)
    display.output.log("Web Interface started",0)

    app.startEngine(config.ENGINE_IP, config.ENGINE_PORT)
    display.output.log("Execution engine started",0)

    app.executeForever()

    display.output.log("Reporter exits",0)

    app.stopWebInterface()
    display.output.log("Web Interface stopped",0)

    app.stopEngine()
    display.output.log("Execution engine stopped",0)

    display.output.signalExit()
    while not display.output.exited(): pass
    sys.exit()



