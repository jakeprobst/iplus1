from .iplus1 import IPlus1Database
from django.db.models.loading import get_model # fix circular dependency

class IPlus1Manager(object):
    iplus1db = IPlus1Database()

    def last_updated(self):
        pass
        #self.command = 'LASTUPDATE'
        #response = self.send()
        #return convert_to_datetime_object(response)

    def prepare_updates(self, date=None):
        Log = get_model('pytoeba', 'Log')

        if not date:
            date = self.last_updated()

        logs = list(Log.objects.filter(added_on__gte=date))

        for log in logs:
            if log.type == 'sad':
                self.iplus1db.add_sentence(log.source_lang, log.source_id, log.change_set)
            if log.type == 'sed':
                self.iplus1db.remove_sentence(log.source_lang, log.source_id)
                self.iplus1db.add_sentence(log.source_lang, log.source_id)
            if log.type == 'lad':
                self.iplus1db.add_link(log.source_lang, log.source_id, log.target_lang, log.target_id)
            if log.type == 'srd':
                self.iplus1db.remove_sentence(log.source_lang, log.source_id)
            if log.type == 'lrd':
                self.iplus1db.remove_link(log.source_lang, log.source_id, log.target_lang, log.target_id)
        # need to set the LASTUPDATE thing here    
            
    def prepare_anki_deck(self, deck, nlang, tlang):
        user = User(nlang, tlang)
        user.parse_text(deck)
        
        return self.iplus1db.parse_user(user)

    def get_iplus1(self, deck, lang1, lang2):
        Sentence = get_model('pytoeba', 'Sentence')

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
