#-------------------------------------------------------------------------------
# Name:        config
# Purpose:
#
# Author:      B36573
#
# Created:     01.03.2013
# Copyright:   (c) B36573 2013
# Licence:     <your licence>
#-------------------------------------------------------------------------------
import socket
from common.log import display
import sys

WEB_IP = socket.gethostname()
WEB_PORT = 81

ENGINE_IP = socket.gethostname()
ENGINE_PORT = 49150

CAMPAIN_TIME_TRESH = 18

GIT_INFO_FILE = "git_info.txt"

DELIMITER = "-_-"
#####################
### XML TAGS ########
XML_TEST_SENDER_ROOT_TAG = "TEST_RESULT_CLIENT"
XML_TEST_CONFIG_TAG = "CONFIGURATION"
XML_TEST_SUITES_TAG = "testsuites"
XML_TEST_SUITE_TAG = "testsuite"
XML_TEST_CASE_TAG = "testcase"
XML_TEST_FAILIURE_TAG = "failure"
XML_TEST_FILE_MISSING_TAG = "missing"
XML_TEST_FILE_TESTS_TAG = "tests"
XML_TEST_FILE_FAILURES_TAG = "failures"
XML_TEST_FILE_ERRORS_TAG = "errors"

XML_TEST_SERVER_ROOT_TAG = "TEST_RESULT_SERVER"

######################
### XML_ATTRIBUTES ###
XML_PLATFORM = "platform"
XML_COMPILER = "compiler"
XML_CONFIG = "config"
XML_MODULE = "module"
XML_CLASS = "class"
XML_APPS = "apps"
XML_APP_NAME = "app_name"
XML_DATA = "data"
XML_STATUS_ATTR = "status"
XML_MSG_ATTR = "message"
XML_TYPE = "type"
XML_GIT_BRANCH = "branch"
XML_GIT_COMMIT = "commit"
######################
### XML_VALUES #######
XML_OK_VALUE = "ok"
XML_ERROR_VALUE = "error"
XML_TRUE_VALUE = "true"
XML_FALSE_VALUE = "false"
XML_STATUS_NEED_CAMP_INFO = "need_camp_info"
XML_VCAST = "vcast"
XML_EXEC_TEST = 'exec_test'
XML_KERNEL_TEST = "kernel"

try:
    from local_config import *
except:
    display.output.log("local_config.py does not exists or is incorrect! See local_config-example.py",2)
    sys.exit(1)
