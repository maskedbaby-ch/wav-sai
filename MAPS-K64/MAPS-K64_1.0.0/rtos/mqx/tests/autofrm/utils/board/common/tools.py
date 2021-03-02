######################################################################################
######################################################################################
######################################################################################
# @file tools.py
# @version 1.0.0

# \brief General useful tools
# \project Common
# \author Damian Nowok
# \author Freescale Semiconductor, Roznov pod Radhostem, Czech Republic

# @lastmodusr b22656
# @lastmoddate Jun-14-2012

# Freescale Semiconductor Inc.
# (c) Copyright 2012 Freescale Semiconductor Inc.
# ALL RIGHTS RESERVED.
######################################################################################
######################################################################################
######################################################################################
from common.exception import rootexception

######################################################################################
################################### Classes ##########################################
######################################################################################
class ToolsException(rootexception.RootException): pass

######################################################################################
################################### Functions ########################################
######################################################################################
def getGitBranch(branch, branches):

    if branch not in branches:
        branch = None

    if branch == None:
        for b in branches:
            if b.startswith('master'):
                branch = b
                break
        else:
            branch = branches[0]
    return branch

def getTimestamp(timestamp):

    import datetime, config

    if timestamp == None:
        date = datetime.datetime.now()
        if date.hour < config.CAMPAIN_TIME_TRESH:
            date -= datetime.timedelta(days=1)
        timestamp = date.strftime( '%Y%m%d' )
    return timestamp


def getGitInfo(path):

    import subprocess

    process = subprocess.Popen(["git", "branch", "-v"],
                stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                shell=True, cwd=path)

    for line in process.stdout:
        if line[0] != '*':
            continue
        parts = line.split()
        branch = parts[1]
        commit = parts[2]
        return branch, commit


def patternSort(x,y,pattern):
    """
        Sort function. Sorts elements according to order pattern

        Input:
        x,y     - elementes to to be sorted
        pattern - Pattern

        Return:
        1,0,-1 depending on order in pattern
    """

    x=x.lower()
    y=y.lower()

    if not x in pattern: return 0
    if not y in pattern: return -1

    if pattern.index(x) > pattern.index(y): return 1
    if pattern.index(x) == pattern.index(y): return 0
    if pattern.index(x) < pattern.index(y): return -1

def dateTimeSort(x,y):
    """
        Sort function. Sorts elements according to datetime

        Input:
        x,y     - datetime objects to to be sorted

        Return:
        1,0,-1  - None always is last, then the oldest, then newer, then newset
    """

    if x == None and y == None: return 0
    if x == None: return -1
    if y == None: return 1

    if x > y: return 1
    if x == y: return 0
    if x < y: return -1

def waitForKey():
    """
        This function waits for keyboard button press
        Returns pressed button

        Input:
        Nothing

        Return:
        Key value
    """
    import msvcrt

    while msvcrt.kbhit(): pass
    return ord(msvcrt.getwch())

def isErrorInString(string,regular_expressions):
    """
        Returns true if one of regular expressions match

        Input:
        string                  - String where to be searched
        regular_expressions     - List of compiled regular expressions

        Returns:
        True/False
    """
    for rule in regular_expressions:
        if rule.search(string) != None: return True
    return False

def save(variable,path):
    """
        Saves variable to file

        Input:
        variable    - variable to save
        path        - Where to save variable

        Returns:
        Nothing
    """
    import pickle
    import os

    dir_name = os.path.dirname(path)

    if not os.path.isdir(dir_name):
        try:
            os.makedirs(dir_name)
        except OSError as error:
            raise ToolsException(str(error))

    try:
        file_ref = open(path,"w")
    except IOError as error:
        raise ToolsException(str(error))

    pickle.dump(variable,file_ref)
    file_ref.close()

def load(path):
    """
        Loads variable from file

        Input:
        path        - From where load variable

        Returns:
        variable
    """
    import pickle

    try:
        file_ref = open(path,"r")
    except IOError as error:
        raise ToolsException(str(error))

    try:
        variable = pickle.load(file_ref)
    except pickle.struct.error as error:
        raise ToolsException(str(error))
    except TypeError as error:
        raise ToolsException(str(error))
    except EOFError as error:
        raise ToolsException(str(error))
    except IOError as error:
        raise ToolsException(str(error))
    except KeyError as error:
        raise ToolsException(str(error))
    except IndexError as error:
        raise ToolsException(str(error))
    except ImportError as error:
        raise ToolsException(str(error))

    file_ref.close()

    return variable

def delete(path):
    """
        This function deletes path

        Input:
        Path

        Returns:
        Nothing
    """
    import os
    import shutil

    if os.path.isdir(path):
        try:
            shutil.rmtree(path)
        except shutil.Error as error:
            raise ToolsException(str(error))
    elif os.path.isfile(path):
        try:
            os.unlink(path)
        except OSError as error:
            raise ToolsException(str(error))

def copy(src,dst):
    """
        This function copies file to folder

        Input:
        src         - File to copy
        dst         - Where to copy
    """
    import os,shutil

    if not os.path.isdir(dst):
        try:
            os.makedirs(dst)
        except WindowsError as error:
            raise ToolsException(str(error))

    try:
        shutil.copy(src,dst)
    except shutil.Error:
        raise ToolsException(str(error))

def modificationDateSort(x,y):
    """
        This is sort function that should be used to sort path
        according to their modification date

        Input:
        x,y     - Paths to compare

        Return:
        1,0,-1
    """
    import os

    if not os.path.exists(x): raise ToolsException("Path dose not exist.")
    if not os.path.exists(y): raise ToolsException("Path dose not exist.")

    info_x = os.stat(x)
    info_y = os.stat(y)

    if info_x.st_mtime > info_y.st_mtime: return 1
    if info_x.st_mtime < info_y.st_mtime: return -1
    if info_x.st_mtime == info_y.st_mtime: return 0

def get_drives():
    """
        This function gets all drives letters

        Input:
        Nothing

        Returns:
        list of drives
    """
    import string
    from ctypes import windll

    drives = []
    bitmask = windll.kernel32.GetLogicalDrives()
    for letter in string.uppercase:
        if bitmask & 1:
            drives.append(letter + ":")
        bitmask >>= 1

    return drives

def compile_regexpr(list):
    """
        This function compiles regular expressions

        Input:
        list    - List of regular expressions

        Returns:
        List of compiled regular expressions
    """
    import re

    reg_expr_compiled = []
    for rule in list:
        reg_expr_compiled.append(re.compile(rule))

    return reg_expr_compiled

def remove_pyc(path):
    """
        This function remove *.pyc files
        from folder

        Input:
        path        - path to folder

        Returns:
        Nothing
    """
    import os,stat

    for root,dirs,files in os.walk(path):
        for file_name in files:
            if os.path.splitext(file_name)[1] == ".pyc":
                os.chmod(os.path.join(root,file_name), stat.S_IWRITE)
                try:
                    os.unlink(os.path.join(root,file_name))
                except:
                    pass

