#-------------------------------------------------------------------------------
# Name:        models
# Purpose:
#
# Author:      B36573
#
# Created:     06.03.2013
# Copyright:   (c) B36573 2013
# Licence:     <your licence>
#-------------------------------------------------------------------------------

from django.shortcuts import render_to_response
from black_board.models import ReportGetter
import models
import config

def genPics(pic_names):

    import subprocess
    from os.path import dirname, join, normpath, realpath, isfile

    pics_path = dirname(realpath(__file__))
    pics_path = normpath(join(pics_path, '..', 'templates', 'pics'))

    for pic in pic_names:
        if not isfile(join(pics_path, pic + '.png')):
            subprocess.call(['perl','png_gen.pl',pic],cwd=pics_path)


def index(request, timestamp=None, branch=None):

    results, modules, runs, branches, timestamp, branch = ReportGetter.get(timestamp, branch)
    modules.sort()
    top_header = ["Platform", "Compiler", "Config"]

    genPics(modules + top_header)
    response = render_to_response("black_board.html", {
        'headers' : modules,
        'results' : results,
        'top_header' : top_header,
        'runs' : runs,
        'timestamp' : timestamp,
        'branches' : branches,
        'branch' : branch
    })

    return response

