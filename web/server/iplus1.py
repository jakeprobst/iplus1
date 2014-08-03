import redis
import pyplus1

REDIS_SERVER = 'localhost'
REDIS_PORT = 6379
MAX_RESULTS = 100
# connection pooling for redis
POOL = redis.ConnectionPool(host=REDIS_SERVER, port=REDIS_PORT)

# pytoeba is still on python2, gotta inherit from object
class Result(object):
    def __init__(self, nid, tid, word):
        self.nid = nid
        self.tid = tid
        self.word = word

    def __repr__(self):
        return "Result(%d %d %s)" % (self.nid, self.tid, self.word)

class User(object):
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
        for line in text:
            words = pyplus1.lang_parse(self.tlang.encode('utf-8'), line.encode('utf-8'))
            for w in words:
                self.add_word(w)
            
class IPlus1Database(object):
    def __init__(self):
        self.redisdb = redis.Redis(connection_pool=POOL)

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
        
    def parse(self, nlang, tlang, deck):
        result = pyplus1.parse_full(nlang, tlang, deck)
        out = []
        for r in result:
            out.append(Result(r['nid'], r['tid'], ''))
        
        return out
        
        
        
        
