#! /usr/bin/env python

import os


def main():
    if not os.path.exists("sentences.csv"):
        os.system("wget http://tatoeba.org/files/downloads/sentences.csv")
    if not os.path.exists("links.csv"):
        os.system("wget http://tatoeba.org/files/downloads/links.csv")

    outfiles = {}

    senfile = open("sentences.csv")
    while True:
        line = senfile.readline()
        if line == '':
            break
        id, lang, sen = line.split('\t')
        
        if not outfiles.has_key(lang):
            outfiles[lang] = file("data/" + lang + ".csv", "w")

        outfiles[lang].write(line)














if __name__ == '__main__':
    main()
