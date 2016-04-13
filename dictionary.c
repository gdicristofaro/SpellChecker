/****************************************************************************
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Implements a dictionary's functionality.
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "dictionary.h"

// hashSize determines the size of the hashtable
// this hash function inspired by the string hashcode function in jdk takes advantage
// of sizes typically being multiples of 32 I believe.
// hash size should be a multiple of 32 for that reason.
#define hashSize 32768

unsigned int dictSize = 0;

//create node holder for reading in dictionary
struct node* arr;

//buffer for strings
char* buffer;


//linked list adapted from http://www.cprogramming.com/tutorial/c/lesson15.html
struct node {
    char* string;
    struct node* next;
} __attribute__((__packed__));
    
bool stringCmp(char* dictionaryWord, const char* string, int stringlength)
{
    //maybe there are some extra characters in dictionaryWord...that would be a problem
    if (dictionaryWord[stringlength] != '\0')
        return false;

    //check all letters that are consistent to dictionaryWord and string (that exist in stringlength
    for (int i = 0; i < stringlength; i++)
    {
        // the goal of this operation is to compare the dictionary char and string char
        // the ^ operator should take care of equality concerning the necessary bits by setting them to 0 if equal
        // the 0x5F should deal with the case issue (i.e. a and A are both seen as 65 with this)
        // however, it should keep the apostrophe lower so that '(39  to 7) does not equal g (103 to 71 and not 7 if
        // we took out the 64)
        // if it isn't 0, then they aren't equal
        //used as source: http://www.cs.umd.edu/class/sum2003/cmsc311/Notes/BitOp/bitwise.html
        if ((string[i] ^ dictionaryWord[i]) & 0x5F)
            return false;
    }
    
    //we made it through the tests and we are all set
    return true;
}


/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    //figure out string length and hash (at the same time)
    int length = 0;
    unsigned int curHash = 0;
    
    while(word[length] != '\0')
    {
        // adapted from http://hg.openjdk.java.net/jdk7/build-gate/jdk/file/tip/src/share/classes/java/lang/String.java (the hashCode part)
        // adjust for upper/lowercase using & 0x1F - this should only take the bits under 32 which is the only piece we need to determine the letter
        // this will mean that ' will have the same value as a letter, but I think it is a small issue given the efficiency
        curHash = curHash * 31 + (word[length] & 0x1F);
        length++;
    }
    
    //do bounds checking on the hash (make sure it isn't larger than hashSize and consequently, the arr)
    int thehash = (int) (curHash % hashSize);
    
    //if hash doesn't exist, then it isn't a word
    if (arr[thehash].string == NULL)
        return false;
    
    //if hash does exist, then check it to see if it is ok
    if (stringCmp(arr[thehash].string, word, length))
        return true;
    
    //so the first node didn't work, let's try another one in the linked list  
    struct node current = arr[thehash];
    while ( current.next != NULL)
    {
        current = *(current.next);
        if (stringCmp(current.string, word, length))
            return true;
    }
    
    //that failed too, so it isn't in dictionary
    return false;
}


// TODO REMOVE: for diagnostic purposes only
/*
int totalwords = 0;
int duplicatehashes = 0;
int maxdepth = 0;

void printStats() {    
    printf("\n\nDIAGNOSTICS: \n\ttotal words loaded: %d\n\tduplicatehashes: %d\n\tmax depth: %d\n\n", totalwords, duplicatehashes, maxdepth);
}
*/

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    arr = malloc(sizeof(struct node) * hashSize);
    if (arr == NULL)
        return false;

    //set it to null so we can be clear what is going on inside
    //pointers to pointers credited here: http://www.eskimo.com/~scs/cclass/int/sx8.html
    for (int i = 0; i < hashSize; i++)
        arr[i].string = NULL;
    
    FILE *input = fopen( dictionary, "r" );
    if (input == NULL)
        return false;
    
    //this will handle the hash of each word
    unsigned int currentHash = 0;
    
    //fseek to determine length - http://www.linuxquestions.org/questions/programming-9/how-to-get-size-of-file-in-c-183360/
    fseek(input, 0, SEEK_END);
    
    //set the buffer up - I am malloc'ing the size of the dictionary file plus a byte
    //I am going to put all of the words into buffer and then chop up buffer
    buffer = malloc(ftell(input) + 1);
    if (buffer == NULL)
        return false;
    
    //go back to the right place in the file
    fseek(input, 0, SEEK_SET);
    
    // curLength will tell us where the character should go
    // curStringStart will tell us where the next string will start
    int curLength = 0;
    int curStringStart = 0;
    
    //read in input
    while (!feof(input)) 
    {
        char nextChar = fgetc(input);
        
        
        if (nextChar == '\n')
        {
            //TODO remove below
            //totalwords++;
        
        	//end that piece of the string
            buffer[curLength] = '\0';

            //get hash
            int hash = (int) (currentHash % hashSize);
        
            //put word in the hash if there isn't one there already
	        if (arr[hash].string == NULL) {
                //set string of current node to the piece of buffer that we want
	            arr[hash].string = &buffer[curStringStart];
                arr[hash].next = NULL;
            }
            //if there is a word in the hash already, append to linked list
	        else
            {
                //TODO remove 2 below
                //duplicatehashes++;
                //int traversecounter = 2;
                
                struct node* traverse = &arr[hash];
                while (traverse->next != NULL)
                {
                    //TODO remove below
                    //traversecounter++;
                    
                    traverse = traverse->next;
                }
                
                //TODO remove below
                //if (traversecounter > maxdepth) maxdepth = traversecounter;

                traverse->next = malloc(sizeof(struct node));
                    //set this node to the string
                    traverse->next->string = &buffer[curStringStart];
                    traverse->next->next = NULL;                    
	        }
	        
	        //we just added a word
	        dictSize++;            
            
            //increment where we are, reset
            curLength++;
            currentHash = 0;
            
            //start of the next string will be the current length after we increment
            curStringStart = curLength;
            continue;
        }
  	
  		//grab the hash
        //hash function explained in line 81  		
        currentHash = currentHash * 31 + (nextChar & 0x1F);
        buffer[curLength] = nextChar;
        curLength++;
    }
    
    //close the file
    fclose(input);
    
    //TODO remove below
    //printStats();

    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return dictSize;
}

//cleans out a node in linked list
void cleanNode(struct node* thenode)
{
    //recurse if there is more
    if (thenode->next != NULL)
        cleanNode(thenode->next);    
    
    //free the node
    free(thenode);
    return;
}


/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void)
{
    free(buffer);

    //for all nodes in array, erase...
    for (int i = 0; i < hashSize; i++)
    {
        //clean nodes if not null
        if (arr[i].next != NULL)
            cleanNode(arr[i].next);
    }
    //free the node holder
    free(arr);
        
    return true;
}
