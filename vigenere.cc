/*/////////////////////////////////////////////////////
 * Programmer: Benjamin Albritton
 * Program: Vigenere
 * version: Final Copy //(for me to keep my sanity)
 *////////////////////////////////////////////////////////

bool debug = false;  		//turn on for debug mode
#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>
using namespace std;

//Global Variables
string key;
string mode;
string input;
string NewKey;
string e = "-e"; 			//mode for encode
string d = "-d"; 			//mode for decode
//bool
bool dashE = false;  		//mode for encode
bool dashD = false; 	    //mode for decode
bool lower = false;         //keeps track of upper and lower case

//prototypes
void setMode();  			//set mode uses the bool of dashE or DashD
void setKey();   			//sets up key for use in coding
void run(string);			//runs the string through code
void code(char, int); 		//algorythm for ciphering
void reset();

//Main: accepts parameters -e, -d, and key
int main(int argc,char *argv[])
{
	mode = argv[1]; 		//takes argument through cli for mode
	key = argv[2];  		//takes argument through cli for key
	do{             		//loops until ' '
	getline(cin, input); //takes input a line at a time
	if(debug)cout <<"MODE: "<<mode<< " KEY: "<< NewKey<< " INPUT: " << input <<endl;
	setMode();
	setKey();
	run(input);//
} while (true); //(!input.length() ==0);
	return 0;
}
//setMode(): returns nothing, simply decides whether to encrypt/decrypt
void setMode()
{
    if(mode.compare(e)==0)
    {
		if(debug)cout << "SETMODE: (encryption)"<< endl;
		dashE = true;
	}
	else if(mode.compare(d)==0)
	{
		if(debug)cout << "MODE: (decrytion)"<< endl;
		dashD = true;
	}
}
//setKey(): returns nothing
//changes key to upper case and removes characters that are not part of the alphabet
void setKey()
{
	string word;
	for(unsigned int tmp = 0; tmp < key.length(); tmp++)
	{
			key[tmp] = toupper(key[tmp]);
	}
   // key.erase(remove(key.begin(), key.end(), ' '), key.end());
    key.erase(remove(key.begin(), key.end(), ' '), key.end());
    for(unsigned int tmp = 0; tmp < key.length(); tmp++)
	{
		if(isalpha(key[tmp]))
		{
		    key[tmp] = toupper(key[tmp]);
			word += key[tmp];
		}
	}NewKey = word;
	if(debug) cout << "SET KEY: (" << NewKey << ")"<< endl;
}
//run(string): returns nothing
//manipulates the key in conjunction with input
//and runs the cipher based on there position within
//there respected arrays
void run(string str)
{
	unsigned int count = 0;
	for(unsigned int x = 0; x < str.length(); x++)
	{
		int keyPos = NewKey[count] - 65;  //since key is capital, and ranges from A-Z, we can -65 to get pos
		if(debug)cout << "key pos: " << keyPos << " count" << count<< endl;
		code(str[x], keyPos);
		if((count == (NewKey.length() -1)) &&( isalpha(str[x]))) //moves the key pos along with the for loop
			count = 0;											 //while allowing it to wrap within the for loop
		else if (isalpha(str[x]))
			count++;
	}
	cout << endl;
}
//code(char, int): returns nothing
//uses the pos of the character within the alphabet and the pos of key to
//encode or decode based on mode
void code(char c, int posOfKey)
{
	if( isalpha(c) )
	{
		if(islower(c)) //changes character to upper case but remembers if it was lowercase
		{
			c = toupper(c);
			lower = true;
		}
		if(dashD)  //decode if true
		{
			c = (((c-65)-posOfKey)) + 65;
			if(c < 65) c += 26;
		}
		else if(dashE) //encode if true
			c = (((c-65)+posOfKey) % 26) + 65;
		if(lower)  //changes back to lowercase
		{
			c = tolower(c);
			lower = false;
		}
	}
	cout << c;	//prints out character c
}

