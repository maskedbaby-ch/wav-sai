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
from vcast.models import ReportGetter
import models, datetime
import config

def index(request, timestamp=None, branch=None):

    results, runs, timestamp, branches, branch = ReportGetter.get(timestamp, branch)
    top_header = ["Platform", "Compiler", "Config", "Alt_config", "Aggregate.html" ,
        'Basis.txt', 'Csv_metrics.csv', 'MAnagement.html', 'Mcdc.html',
        'MCDC_Equivalence.html', 'MCDC_Test.txt', 'Ram_usage.csv', 'Unit_metrics.html',
        'reports.zip']

    response = render_to_response("vcast.html", {
        'top_header' : top_header,
        'results' : results,
        'runs' : runs,
        'timestamp' : timestamp,
        'branches' : branches,
        'branch' : branch
    })

    return response

def download(request, path):

    """
        This function downloads file

        Input:
        request         - Http request from client
        project         - Current project ID
        path            - Path to file

        Return:
        Rendered http page
    """
    import models, os, mimetypes
    from django.http import HttpResponse
    from django.core.servers.basehttp import FileWrapper
    from black_board.templatetags import extras

    path = extras.decodeBase64(path) if path != "None" else path
    file_ref = file(path, 'rb')

    content_type = mimetypes.guess_type(path)
    if content_type[0] == None:
        content_type = 'application/octet-stream'

    wrapper = FileWrapper(file_ref,blksize=65536)
    response = HttpResponse(wrapper,content_type=content_type)
    response['Content-Disposition'] = 'inline; filename="' + os.path.basename(path) + '"'
    response['Content-Length'] = os.path.getsize(path)

    return response