'''
Created on 1.3.2013

@author: B36573
'''
from django.shortcuts import render_to_response

def index(request):
    
    response = render_to_response("index.html")
    return response

