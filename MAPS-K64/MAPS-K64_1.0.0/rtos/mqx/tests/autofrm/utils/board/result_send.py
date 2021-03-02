#-------------------------------------------------------------------------------
# Name:        send
# Purpose:
#
# Author:      B36573
#
# Created:     04.03.2013
# Copyright:   (c) B36573 2013
# Licence:     <your licence>
#-------------------------------------------------------------------------------

if __name__ == '__main__':
    import time
    import argparse
    import sys
    import os


    os.chdir(os.path.dirname(os.path.abspath(__file__)))
    if not os.path.isfile('local_config.py'):
        import shutil
        shutil.copyfile('local_config_example.py','local_config.py')

    import config
    from engine.result_sender import ResultSender
    from engine.result_sender import ResultSenderException
    from common.log import display

    parser = argparse.ArgumentParser(description='Execution test report sender.')
    parser.add_argument('--ip', metavar='<ip address>', type=str, dest='ip', required=True,help='server ip address')
    parser.add_argument('--port', metavar='<port number>', type=int, dest='port', default=49150, help='server port (default: 49150)')
    parser.add_argument('--platform', metavar='<platform>', type=str, dest='platform', required=True, help='tested platform')
    parser.add_argument('--compiler', metavar='<compiler>', type=str, dest='compiler', required=True, help='tested compiler')
    parser.add_argument('--config', metavar='<configuration>', type=str, dest='config', required=True, help='configuration: Debug|Release')
    parser.add_argument('--target', metavar='<target>', type=str, dest='target', required=True, help='Target: INTRAM, DDR,...')
    parser.add_argument('--class', metavar='<class>', type=str, dest='classif', required=True, help='Class: int, ext...')
    parser.add_argument('--module', metavar='<module>', type=str, dest='module', required=True, help='tests module')
    parser.add_argument('--apps', metavar='<apps>', type=str, nargs='+', dest='apps', required=True, help='list of applications')
    parser.add_argument('--xml_dir', metavar='<xml_dir>', type=str, dest='xml_dir', required=True, help='path to xml files')
    parser.add_argument('-s', '--silent', action="store_true", dest='silent', required=False, help='silent mode')
    parser.add_argument('-d', '--dev_dir', metavar="dev_dir", type=str, dest='dev_dir', required=False, help='dev dir')
    args = parser.parse_args()

    if not os.path.exists(args.xml_dir):
        raise ResultSenderException("XML_DIR does not exist")

    if args.dev_dir == None:
        args.dev_dir = os.path.dirname(os.path.realpath((__file__)))


    try:
        ResultSender(args).run()
    except ResultSenderException as error:
        display.output.log("Program exits with message: " + str(error),2)
        display.output.signalExit()
        while not display.output.exited(): pass
        sys.exit(1)
