#-------------------------------------------------------------------------------
# Name:        module1
# Purpose:
#
# Author:      B36573
#
# Created:     06.03.2013
# Copyright:   (c) B36573 2013
# Licence:     <your licence>
#-------------------------------------------------------------------------------

from django import template
import datetime

register = template.Library()

@register.filter
def key(d, key_name):
    try:
        value = d[key_name]
    except KeyError:
        from django.conf import settings

        value = settings.TEMPLATE_STRING_IF_INVALID

    return value

@register.filter
def full_date(timestamp):
    date = datetime.datetime.strptime(timestamp, '%Y%m%d')
    return date.strftime ('%d.%m.%Y')

@register.filter
def split(string, char):
    return str(string).split(char)

@register.filter
def encodeBase64(data_str):
    import base64
    return base64.urlsafe_b64encode(str(data_str))

@register.filter
def decodeBase64(data_str):
    import base64
    return base64.urlsafe_b64decode(str(data_str))

def getGitInfo(str):
    import re

    pattern = '[a-f0-9]+'
    item = 0
    for item in re.finditer(pattern,str):
        pass

    sha = item.group(0)
    branch = str[:item.start()-1]
    return [branch, sha]

@register.filter
def getSHAFromGitInfo(str):
    branch,sha = getGitInfo(str);
    return sha

@register.filter
def getBranchFromGitInfo(str):
    branch,sha = getGitInfo(str)
    return branch
