from django.conf.urls import patterns, include, url
from .views import SentenceAdd
from .api import IPlus1Resource

iplus1_resource = IPlus1Resource()

urlpatterns = patterns('',
    url(r'^sentence/add/$', SentenceAdd.as_view(), name='sentence_add'),
    (r'^api/', include(iplus1_resource.urls)),
)
