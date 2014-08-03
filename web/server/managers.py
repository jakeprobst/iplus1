from django.db.models.manager import Manager
from django.db.models.query import QuerySet
from django.db.models.loading import get_model
from .utils import get_user
from .iplus1 import IPlus1Database, User

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
        #user = User(nlang, tlang)
        #user.parse_text(deck)
        
        #return self.iplus1db.parse_user(user)
        enc_deck = []
        for d in deck:
            enc_deck.append(d.encode('utf-8'))
        print [nlang, tlang, enc_deck]
        return self.iplus1db.parse(nlang.encode('utf-8'), tlang.encode('utf-8'), enc_deck)

    def get_iplus1(self, deck, lang1, lang2):
        #Sentence = get_model('pytoeba', 'Sentence')

        results = self.prepare_anki_deck(deck, lang1, lang2)
        
        out = []
        
        for r in results:
            n = self.iplus1db.redisdb.hget(lang1 + '-full', r.nid)
            t = self.iplus1db.redisdb.hget(lang2 + '-full', r.tid)
            out.append([n,t])
        
        return out
        
        #nid = []
        #tid = []
        #for r in results:
        #    nid.append(r.nid)
        #    tid.append(r.tid)
        
        #nsen = list(Sentence.objects.filter(id__in=nid))
        #tsen = list(Sentence.objects.filter(id__in=tid))
        
        #out = []
        #for i in xrange(len(tsen)):
        #    out.append([nsen[i], tsen[i]])
        
        #return out

class SentenceQuerySet(QuerySet):
    """
    Overrides django's default queryset class to add
    custom fuctionality mirrored from instance methods
    and on the manager. This essentially means every
    time you apply any orm filter these methods will
    be available on them, which makes them chainable.
    """
    def all(self):
        """
        This makes all other filters not see deleted
        objects by default.
        """
        return self.filter(is_deleted=False)

    def deleted(self):
        """
        This filter shows only deleted objects.
        """
        return self.filter(is_deleted=True)

    def active(self):
        """
        A filter that shows only objects that have been
        owned by a native or someone with the appropriate
        permissions for is_active to be true.
        """
        return self.all().filter(is_active=True)

    def inactive(self):
        """
        Reverse fucntionality of the active filter.
        """
        return self.all().filter(is_active=False)

    def locked(self):
        """
        Filters all locked sentences.
        """
        return self.all().filter(is_editable=False)

    def unlocked(self):
        """
        Reverse filter for locked.
        """
        return self.all().filter(is_editable=True)

    def orphan(self):
        """
        Filters all unowned sentences.
        """
        return self.all().filter(owner=None)

    def needs_correction(self):
        """
        Filters all sentences that have an attached correction.
        """
        return self.all().filter(has_correction=True)

    def delete(self):
        """
        Bulk deletes all sentences in the queryset. Uses the
        same implementation as the delete sentence instance
        method.
        """
        for sent in self.all():
            sent.delete()

    def lock(self):
        """
        Bulk locks sentences in the queryset. Mirrors
        sentence.lock
        """
        for sent in self.all():
            sent.lock()

    def unlock(self):
        """
        Bulk unlocks sentences in the queryset. Mirrors
        sentece.lock.
        """
        for sent in self.all():
            sent.unlock()

    def adopt(self):
        """
        Bulk adopts sentences in the queryset. Mirrors
        sentence.adopt.
        """
        for sent in self.all():
            sent.adopt()

    def release(self):
        """
        Bulk releases ownership over sentences in the
        queryset. Mirrors sentence.release.
        """
        for sent in self.all():
            sent.release()

    def change_language(self, lang):
        """
        Bulk changes the lang field on sentences in the
        queryset. Mirrors sentence.change_language.
        """
        for sent in self.all():
            sent.change_language(lang)

    def link(self, sent):
        """
        Links all sentences in the
        queryset to a single sentence.
        Mirrors the instance method.
        """
        sents = self.all()
        sent.bulk_link(sents)

    def unlink(self, sent):
        """
        Unlinks all sentences in the
        queryset from a single sentence.
        Mirrors the instance methods.
        """
        sents = self.all()
        sent.bulk_unlink(sents)

    def translate(self, text, lang='auto'):
        """
        Adds a sentence then links it to
        all sentences in the queryset.
        Mirrors the instance methods.
        """
        sents = self.all()
        sent = self.add(text, lang)
        sent.bulk_link(sents)

    def auto_force_correction(self):
        """
        Autoforces corrections on all sentences
        in the queryset. Mirrors the instance method.
        """
        for sent in self.all():
            sent.auto_force_correction()

    def add_tag(self, text):
        """
        Adds an existing tag to all sentences
        in the queryset. Mirrors the instance
        method.
        """
        for sent in self.all():
            sent.add_tag(text)

    def add_new_tag(self, text, lang):
        """
        Creates a new tag then adds it to all
        sentences in the queryset. Mirrors the
        instance method.
        """
        sents = self.all()
        sents[0].add_new_tag(text, lang)
        for sent in sents[1:]:
            sent.add_tag(text)

    def delete_tag(self, text):
        """
        Removes a tag from all sentences
        in the queryset. Mirrors the
        instance method.
        """
        for sent in self.all():
            sent.delete_tag(text)

class SentenceManager(Manager):

    def get_query_set(self):
        return SentenceQuerySet(self.model, using=self._db)

    def all(self):
        return self.get_query_set().all()

    def deleted(self):
        return self.get_query_set().deleted()

    def active(self):
        return self.get_query_set().active()

    def inactive(self):
        return self.get_query_set().active()

    def locked(self):
        return self.get_query_set().locked()

    def unlocked(self):
        return self.get_query_set().unlocked()

    def orphan(self):
        return self.get_query_set().orphan()

    def needs_correction(self):
        return self.get_query_set().needs_correction()

    def add(self, text, lang='auto'):
        user = get_user()
        sent = self.create(
            added_by=user, owner=user, text=text, lang=lang
            )
        Log = get_model('pytoeba', 'Log')
        Log.objects.create(
            sentence=sent, type='sad', done_by=user, change_set=text,
            target_id=sent.hash_id
            )
        return sent

    def bulk_add(self, sentences):
        if isinstance(sentences[0], tuple):
            for sent_info in sentences:
                text = sent_info[0]
                lang = sent_info[1]
                self.add(text, lang)
        else:
            for sentence in sentences:
                self.add(sentence)

    def show(self, hash_id):
        return self.get(hash_id=hash_id)

    def bulk_show(self, hashes):
        return self.filter(hash_id__in=hashes)

    def edit(self, hash_id, text):
        sent = self.get(hash_id=hash_id)
        sent.edit(text)

    def bulk_edit(self, edits):
        for edit in edits:
            hash_id = edit[0]
            text = edit[1]
            self.edit(hash_id, text)

    def delete(self, hash_id):
        sent = self.get(hash_id=hash_id)
        sent.delete()

    def bulk_delete(self, hashes):
        sentences = self.filter(hash_id__in=hashes)
        for sent in sentences:
            sent.delete()

    def lock(self, sent_id):
        self.show(sent_id)
        sent.lock()

    def unlock(self, sent_id):
        self.show(sent_id)
        sent.unlock()

    def adopt(self, sent_id):
        self.show(sent_id)
        sent.adopt()

    def release(self, sent_id):
        self.show(sent_id)
        sent.release()

    def link(self, source_id, target_id):
        source = self.show(source_id)
        target = self.show(target_id)
        source.link(target)

    def bulk_link(self, source_id, target_ids):
        source = self.show(source_id)
        targets = self.bulk_show(target_ids)
        source.bulk_link(targets)

    def unlink(self, source_id, target_id):
        source = self.show(source_id)
        target = self.show(target_id)
        source.unlink(target)

    def bulk_unlink(self, source_id, target_ids):
        source = self.show(source_id)
        targets = self.bulk_show(target_ids)
        source.bulk_unlink(targets)

    def translate(self, sent_id, text, lang='auto'):
        sent = self.show(sent_id)
        sent.translate(text, lang)

    def correct(self, sent_id, text, reason=''):
        sent = self.show(sent_id)
        sent.correct(text, reason)

    def accept_correction(self, sent_id, corr_id):
        sent = self.show(sent_id)
        sent.accept_correction(corr_id)

    def reject_correction(self, sent_id, corr_id):
        sent = self.show(sent_id)
        sent.reject_correction(corr_id)

    def force_correction(self, sent_id, corr_id):
        sent = self.show(sent_id)
        sent.force_correction(corr_id)

    def auto_force_correction(self, sent_id):
        sent = self.show(sent_id)
        sent.auto_force_correction()

    def add_tag(self, sent_id, text):
        sent = self.show(sent_id)
        sent.add_tag(text)

    def add_new_tag(self, sent_id, text, lang):
        sent = self.show(sent_id)
        sent.add_new_tag(text)

    def delete_tag(self, sent_id, text):
        sent = self.show(sent_id)
        sent.delete_tag(text)


class CorrectionQuerySet(QuerySet):

    def delete(self):
        for corr in self.all():
            corr.delete()

    def accept(self):
        for corr in self.all():
            corr.accept()

    def reject(self):
        for corr in self.all():
            corr.reject()

    def force(self):
        for corr in self.all():
            corr.force()


class CorrectionManager(Manager):

    def get_query_set(self):
        return CorrectionQuerySet(self.model, using=self._db)

    def add_to_obj(self, sent, text, reason=''):
        user = get_user()
        corr = self.create(
            sentence=sent, text=text, added_by=user, reason=reason
            )
        Log = get_model('pytoeba', 'Log')
        Log.objects.create(
            sentence=sent, type='cad', done_by=user, change_set=corr.text,
            target_id=corr.hash_id
            )
        return corr

    def add(self, sent_id, text, reason=''):
        Sentence = get_model('pytoeba', 'Sentence')
        sent = Sentence.objects.show(sent_id)
        corr = self.add_to_obj(sent, text, reason)
        return corr

    def edit(self, corr_id, text):
        corr = self.get(hash_id=corr_id)
        corr.edit(text)

    def delete(self, corr_id):
        corr = self.get(hash_id=corr_id)
        corr.delete()

    def accept(self, corr_id):
        corr = self.get(hash_id=corr_id)
        corr.accept()

    def reject(self, corr_id):
        corr = self.get(hash_id=corr_id)
        corr.reject()

    def force(self, corr_id):
        corr = self.get(hash_id=corr_id)
        corr.force()

    def auto_force(self, sent_id):
        sent = Sentence.objects.show(sent_id)
        sent.auto_force_correction()


class TagManager(Manager):

    def get_localization(self, tag_id, lang):
        tag = self.get(hash_id=tag_id)
        tag.get_localization(lang)

    def get_all_localizations(self, tag_id):
        tag = self.get(hash_id=tag_id)
        tag.get_all_localizations()

    def merge(self, source_id, target_id):
        source = self.get(hash_id=source_id)
        target = self.get(hash_id=target_id)
        source.merge(target)

    def translate(self, tag_id, text, lang):
        tag = self.get(hash_id=tag_id)
        tag.translate(text, lang)

    def add_new(self, text, lang):
        user = get_user()
        tag = self.create(added_by=user)
        tag.translate(text, lang)
        return tag
