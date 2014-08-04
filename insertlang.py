#! /usr/bin/env python
import redis

REDIS_SERVER = 'localhost'
REDIS_PORT = 6379
POOL = redis.ConnectionPool(host=REDIS_SERVER, port=REDIS_PORT)


LANGS = ['dan', 'deu', 'eng','fin','fra','ita','jpn','nld','nob','por','ron','rus','spa','swe']


def insert_lang_into_redis(path):
    rdb = redis.Redis(connection_pool=POOL)
    
    
    
    
    
    
    f = file(path, "r")
    for l in f:
        try:
            id, lang, text = l.strip().split('\t',3)
            if lang in LANGS:
                print id, lang, text
                rdb.hset(lang + "-full", id, text)
        except ValueError:
            print l





if __name__ == '__main__':
    insert_lang_into_redis('sentences.csv')
