from pytoeba.models import Sentence, Log
from random import random
import redis
import pyplus1

REDIS_SERVER = 'localhost'
REDIS_PORT = 6379
MAX_RESULTS = 100


class Result:
    def __init__(self, nid, tid, word):
        self.nid = nid
        self.tid = tid
        self.word = word

    def __repr__(self):
        return "Result(%d %d %s)" % (self.nid, self.tid, self.word)

class User:
    def __init__(self, nlang, tlang):
        self.words = set()
        self.nlang = nlang
        self.tlang = tlang
        if nlang < tlang:
            self.langstr = nlang + '-' + tlang
        else:
            self.langstr = tlang + '-' + nlang
            

    def add_word(self, word):
        self.words.add(word)

    def has_word(self, word):
        return word in self.words

    def parse_text(self, text):
        for line in text.split('\n'):
            words = pyplus1.lang_parse(self.tlang, line.encode('utf-8'))
            for w in words:
                self.add_word(w)
            
class IPlus1Database:
    def __init__(self):
        self.redisdb = redis.Redis(REDIS_SERVER, REDIS_PORT)

    def add_sentence(self, lang, id, sentence):
        words = pyplus1.lang_parse(lang, sentence.encode('utf-8'))
        
        out = '.'.join(words)
        self.redisdb.hset(lang, id, out)
    
    def remove_sentence(self, lang, id):
        self.redisdb.hdel(lang, id)
    
    def add_link(self, nlang, nid, tlang, tid):
        if nlang < tlang:
            fullid = nid | (tid << 32)
            fulllang = nlang + "-" + tlang
        else:
            fullid = tid | (nid << 32)
            fulllang = tlang + "-" + nlang
        self.redisdb.sset(fulllang, fullid)

    def remove_link(self, nlang, nid, tlang, tid):
        if nlang < tlang:
            fullid = nid | (tid << 32)
            fulllang = nlang + "-" + tlang
        else:
            fullid = tid | (nid << 32)
            fulllang = tlang + "-" + nlang
        self.redisdb.srem(fulllang, fullid)
        
    def parse_user(self, user):
        results = []
        for id in self.redisdb.sscan_iter(user.langstr):
            id = int(id)
            if len(results) > MAX_RESULTS:
                break
            
            if user.nlang < user.tlang:
                nid = id & 0xFFFFFFFF
                tid = id >> 32
            else:
                nid = id >> 32
                tid = id & 0xFFFFFFFF
            
            outwords = []
            words = self.redisdb.hget(user.tlang, tid)
            for w in words.split(','):
                if not user.has_word(w):
                    outwords.append(w)
            
            if len(outwords) == 1:
                results.append(Result(nid, tid, outwords[0]))

        return results

class IPlus1Manager(object):
    iplus1db = IPlus1Database()

    def last_updated(self):
        pass
        #self.command = 'LASTUPDATE'
        #response = self.send()
        #return convert_to_datetime_object(response)

    def prepare_updates(self, date=None):
        if not date:
            date = self.last_updated()

        logs = list(Log.objects.filter(added_on__gte=date))

        for log in logs:
            if log.type in ['sad', 'sed']:
                self.iplus1db.add_sentence(log.langs, log.source_id, log.change_set)
            if log.type == 'lad':
                langs = log.langs.split()
                self.iplus1db.add_link(langs[0], logs.source_id, langs[1], logs.target_id)
            if log.type == 'srd':
                # this isn't in the protocol too
                self.iplus1db.remove_sentence(log.langs, log.source_id)
            if log.type == 'lrd':
                # uh this too? updates is officially broken
                langs = log.langs.split()
                self.iplus1db.remove_link(langs[0], logs.source_id, langs[1], logs.target_id)

        # need to set the LASTUPDATE thing here
    
            
    def prepare_anki_deck(self, deck, nlang, tlang):
        user = User(nlang, tlang)
        user.parse_text(deck)
        
        return self.iplus1db.parse_user(user)

    def get_iplus1(self, deck, lang1, lang2):
        results = self.prepare_anki_deck(deck, lang1, lang2)
        nid = []
        tid = []
        for r in results:
            nid.append(r.nid)
            tid.append(r.tid)
        
        nsen = list(Sentence.objects.filter(id__in=nid))
        tsen = list(Sentence.objects.filter(id__in=tid))
        
        out = []
        for i in xrange(len(tsen)):
            out.append([nsen[i], tsen[i]])
        
        return out
        
        
        
        
        
        
