#-------------------------------------------------------------------------------
# Name:        receiver
# Purpose:
#
# Author:      B36573
#
# Created:     04.03.2013
# Copyright:   (c) B36573 2013
# Licence:     <your licence>
#-------------------------------------------------------------------------------

from common.log import display
from common.communication import commserver
from xml.etree import cElementTree as et
import config, os

class ReceiverException(commserver.CommServerException): pass

class Receiver(object):

    def __init__(self):

        self.socket_server = None
        self.handler = None

    def stop(self):

        self.socket_server.shutdown()
        display.output.log("Receiver ended",0)

    def start(self,ip,port):

        try:
            self.socket_server, self.handler = commserver.server(ip,port)
        except commserver.CommServerException as error:
            raise ReceiverException(str(error))

        self.handler.handle = requestHandler
        display.output.log("Receiver started",0)


def requestHandler(req):

    from common.log import display
    from common import tools

    data = req.receive()
##        display.output.log("Received data in reqHandler: \n" + data,0)

    try:
        frame = et.fromstring(data)
    except Exception:
        display.output.log("Communication Error! Received frame cannot be decoded!",2)
        self.close()
        return

    report_type = frame.get(config.XML_TYPE)

    info = frame.find(config.XML_TEST_CONFIG_TAG)
    platform = info.get(config.XML_PLATFORM)
    compiler = info.get(config.XML_COMPILER)
    module = info.get(config.XML_MODULE)
    cnfg = info.get(config.XML_CONFIG)
    clas = info.get(config.XML_CLASS,"int")
    branch = info.get(config.XML_GIT_BRANCH)
    commit = info.get(config.XML_GIT_COMMIT)

    # output root dir
    output_root = config.OUTPUT_FOLDER
    # timespamp dir
    time_dir_name = tools.getTimestamp(None)
    # branch specific dir
    branch_dir_name = '_'.join([branch, commit])
    # project specific dir
    proj_dir_name = config.DELIMITER.join([ clas, platform, compiler, cnfg])
    # concat path elements
    full_path = os.path.join(output_root,time_dir_name,branch_dir_name,proj_dir_name)

    respond_frame = et.Element(config.XML_TEST_SERVER_ROOT_TAG)
    respond_frame.attrib[config.XML_STATUS_ATTR] = config.XML_ERROR_VALUE
    respond_frame.attrib[config.XML_MSG_ATTR] = ""

    if report_type == config.XML_EXEC_TEST:
        execTestHandler(frame, respond_frame, full_path, module)

    elif report_type == config.XML_VCAST:
        vcastHandler(frame, respond_frame, full_path, module)

    else:
        unknownHandler(respond_frame)

    req.send(et.tostring(respond_frame,"utf-8"))
##        display.output.log("Server send:\n" + et.tostring(respond_frame,"utf-8"))
    req.close()


def vcastHandler(frame, respond_frame, path, module):

    import config, zipfile, base64

    try:
        path = config.DELIMITER.join([path, module])
        if not os.path.exists(path):
            os.makedirs(path)
        data = frame.find(config.XML_DATA).text
        data = base64.b64decode(data)

        file_name = os.path.join(path, "reports.zip")
        open(file_name, "wb").write(data)
        zip_file = zipfile.ZipFile(file_name)
        zip_file.extractall(path)
        zip_file.close()

        respond_frame.attrib[config.XML_STATUS_ATTR] = config.XML_OK_VALUE

    except Exception as error:
        respond_frame.attrib[config.XML_MSG_ATTR] = str(error)


def execTestHandler(frame, respond_frame, path, module):

    import config

    try:
        if not os.path.exists(path):
            os.makedirs(path)

        file_name = module + ".xml"
        file_name = os.path.join(path,file_name)

        et.ElementTree(frame).write(file_name, "UTF-8", "xml")
        respond_frame.attrib[config.XML_STATUS_ATTR] = config.XML_OK_VALUE

    except Exception as error:
        respond_frame.attrib[config.XML_MSG_ATTR] = str(error)


def unknownHandler(respond_frame):

    import config

    respond_frame.attrib[config.XML_MSG_ATTR] = "Unknown report type"

