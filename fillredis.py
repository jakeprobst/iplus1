#! /usr/bin/env python

import redis
import pyplus1

REDIS_SERVER = 'localhost'
REDIS_PORT = 6379

def fill_redis():
    redisdb = redis.Redis(REDIS_SERVER, REDIS_PORT)
    senf = open("sentences.csv")
    for line in senf:
        id, lang, sen = line.strip().split('\t')
        
        token = pyplus1.lang_parse(lang, sen)
        if token:
            tokenbuf = ','.join(token)
            redisdb.hset(lang, int(id), tokenbuf)


    linkf = open("data/links.csv")
    for line in linkf:
        nlang, nid, tlang, tid = line.split('\t')
        
        if nlang < tlang: # nlang sorts before tlang
            merged = (nid) | (tid << 32)
            setname = "%s-%s" % (nlang, tlang)
        else:
            merged = (tid) | (nid << 32)
            setname = "%s-%s" % (tlang, nlang)

        redisdb.sadd(setname, merged)









if __name__ == '__main__':
    fill_redis()
