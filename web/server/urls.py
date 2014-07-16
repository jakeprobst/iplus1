from django.conf.urls import patterns, include, url
from .api import IPlus1Resource

iplus1_resource = IPlus1Resource()

urlpatterns = patterns('',
    url(r'^api/', include(iplus1_resource.urls)),
)
