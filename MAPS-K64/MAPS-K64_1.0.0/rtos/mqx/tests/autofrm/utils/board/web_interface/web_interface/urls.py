from django.conf.urls import patterns, url

# Uncomment the next two lines to enable the admin:
# from django.contrib import admin
# admin.autodiscover()

urlpatterns = patterns('',
    # Examples:
    # url(r'^$', 'web_interface.views.home', name='home'),
    # url(r'^web_interface/', include('web_interface.foo.urls')),

    # Uncomment the admin/doc line below to enable admin documentation:
    # url(r'^admin/doc/', include('django.contrib.admindocs.urls')),

    # Uncomment the next line to enable the admin:
    # url(r'^admin/', include(admin.site.urls)),
    url(r'^$', 'sumary.views.index'),
    url(r'^board$', 'black_board.views.index'),
    url(r'^board/(?P<timestamp>[A-Za-z0-9]+)$', 'black_board.views.index'),
    url(r'^board/(?P<timestamp>[A-Za-z0-9]+)/(?P<branch>[a-zA-Z0-9:\\ _.-\\&\\+\\$\\-]+)$', 'black_board.views.index'),    
    url(r'^board/(?P<branch>[a-zA-Z0-9:\\ _.-\\&\\+\\$\\-]+)$', 'black_board.views.index'),    
    url(r'^vcast$', 'vcast.views.index'),
    url(r'^vcast/download/(?P<path>[a-zA-Z0-9:\\ _.-\\&\\+\\$\\-]+)$', 'vcast.views.download'),
    url(r'^vcast/(?P<timestamp>[A-Za-z0-9]+)$', 'vcast.views.index'),
    url(r'^vcast/(?P<timestamp>[A-Za-z0-9]+)/(?P<branch>[a-zA-Z0-9:\\ _.-\\&\\+\\$\\-]+)$', 'vcast.views.index'),
    url(r'^vcast/(?P<branch>[a-zA-Z0-9:\\ _.-\\&\\+\\$\\-]+)$', 'vcast.views.index')

)
