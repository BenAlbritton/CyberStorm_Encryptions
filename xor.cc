
#include <cstdlib>
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>


using namespace std;

//this will be our xor encryption/decryption function

string inlasning ()
{
  string text;
  string temp; // Added this line
  
  ifstream file;
  file.open ("key");

  while (!file.eof())
  {
    getline (file, temp);
    text.append (temp); // Added this line
  }

       
  
  file.close();

  return text;
}

void XOR(char data[], string key)
{

	string xorstring = data;

	for (int i = 0; i < key.length()-1; i++) { 

		xorstring[i] = data[i] ^ key[i];
cout<< xorstring[i];

	}

}

int main()
{

	


string key =inlasning();


	char ed[2000];
cin.getline(ed,sizeof(ed));
	 XOR(ed,key);


	return 0;
}
