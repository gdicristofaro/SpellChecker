# SpellChecker
Spell-checking algorithm built in C, which loads and unloads a dictionary file and checks words in a dictionary utilizing a self-developed hash map with buckets.  This project was developed for an assignment for CS50 at Harvard University.  dictionary.c was developed in the Fall of 2012.

##Usage
The project can be compiled with gcc using the makefile.  Speller can be run the following way:

```bash
./speller [dictionary file] [text document to be checked]
```

The "large" file can be used for the dictionary file.
