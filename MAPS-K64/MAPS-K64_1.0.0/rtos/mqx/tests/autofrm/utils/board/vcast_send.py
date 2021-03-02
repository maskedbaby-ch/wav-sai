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
    from engine.vcast_sender import VcastSender
    from engine.vcast_sender import VcastSenderException
    from common.log import display

    parser = argparse.ArgumentParser(description='VectorCAST code coverage reports sender.')
    parser.add_argument('-i', '--ip', metavar='ip_address', type=str, dest='ip', required=True, help='server ip address')
    parser.add_argument('-p', '--port', metavar='port_number', type=int, dest='port', required=False, default=49150, help='server port (default: 49150)')
    parser.add_argument('-b', '--board', metavar="board_enable", type=str, dest='board', required=False, default='on', help='enable board sener')
    parser.add_argument('-s', '--silent', action="store_true", dest='silent', required=False, help='silent mode')
    parser.add_argument('-d', '--dev_dir', metavar="dev_dir", type=str, dest='dev_dir', required=False, help='dev dir')
    args = parser.parse_args()

    if args.board == None or args.board.lower() != 'on':
        sys.exit(0)

    if args.dev_dir == None:
        args.dev_dir = os.path.dirname(os.path.realpath((__file__)))

    try:
        list_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'vcast_project.lst')
        if not os.path.isfile(list_file):
            raise VcastSenderException('File list \'' + list_file + '\' not found')

        VcastSender(args).run(list_file)

        if os.path.isfile(list_file + '.bak'): os.unlink(list_file + '.bak')
        os.rename(list_file, list_file+ '.bak')
    except VcastSenderException as error:
        display.output.log("Program exits with message: " + str(error),2)
        display.output.signalExit()
        while not display.output.exited(): pass
        sys.exit(1)
