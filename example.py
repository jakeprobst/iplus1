#! /usr/bin/env python

import pyplus1
import redis

REDIS_SERVER = 'localhost'
REDIS_PORT = 6379

redisdb = redis.Redis(REDIS_SERVER, REDIS_PORT)

NATIVE_LANG = "jpn"
TARGET_LANG = "eng"
sens = ["here is an example sentence", "and here is another sentence that I should fill with various words","and I should put a third sentence just in case","filter out sleep words"]

results = pyplus1.parse_full(NATIVE_LANG, TARGET_LANG, sens)
for r in results:
    n = redisdb.hget(NATIVE_LANG + '-full', r['nid'])
    t = redisdb.hget(TARGET_LANG + '-full', r['tid'])
    print '"%s" -> "%s"' % (n, t)
