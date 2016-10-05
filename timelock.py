#!/usr/bin/python
#TIMELOCK V2.0
#TEAM - KIRBY
#Writen by Benjamin Albritton and Mitchel Brandon
#Version 2.0
#Change timeSinceDate to get a different epoch time
import time; 
import hashlib;

#This is our Epoch time, change this for diffent epoch
timeSinceDate = int(time.mktime(time.strptime('2001-02-03 04:05:06', '%Y-%m-%d %H:%M:%S'))) #- time.timezone

#this is current time for testing
currentTime = int(time.mktime(time.strptime('2010-06-13 12:55:34', '%Y-%m-%d %H:%M:%S')))# - time.timezone

ActualEPOCH = int(time.time())#- time.timezone


###COMENT ONE OR THE UDDER
#NewEpoch = ActualEPOCH - timeSinceDate #our new epoch takes the unix epoch and subtracts time since date
NewEpoch = currentTime - timeSinceDate #for debugging
###

NewEpoch = NewEpoch - (NewEpoch%60) #this is the new epoch and it adds 60 secs after every minute

NewEpoch = str(NewEpoch)  #convert to string for md5 struff 
m = hashlib.md5()
m.update(NewEpoch)
NewEpoch = m.hexdigest() #single hash
m = hashlib.md5()
m.update(NewEpoch)
doubleHased = m.hexdigest() #double hashery
#print 
digitFinder = '' 
letterFinder = ''
#extracts numbers in reverse order
for numbers in reversed(doubleHased):
	if numbers.isdigit():
		digitFinder += numbers  #string appending
#extracts letters
for letters in doubleHased:
	if letters.isalpha():
		letterFinder += letters
#Get the number of characters and digits in the hash, to ensure at least 2 in each
charCheck = len(letterFinder)
numCheck = len(digitFinder)
#Base number of char/digits to print
numNum = 2
numChar = 2
#since extra characters are printed AFTER 0-1 numbers are printed, we use this variable to keep track of # to print
extraChars = 0
#Set the number of characters or numbers to print if either is below 2
if charCheck < 2:
        numNum += (2 - charCheck)#This was the last edit, if it breaks make it numNum += charCheck
        numChar = charCheck
if numCheck < 2:
        #numChar += numCheck
        extraChars = 2 - numCheck
        numNum = numCheck
answer = ''
#Add characters, then numbers, then possibly more characters to output
for i in range(0, numChar ):
        answer += letterFinder[i]
for i in range(0, numNum ):
        answer += digitFinder[i]
if extraChars != 0:
        for i in range(0, extraChars ):
                answer += letterFinder[i+2]
#print the result of the hashes to output
print answer

