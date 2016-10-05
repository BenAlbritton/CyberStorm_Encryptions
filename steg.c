#include <stdio.h>
#include <string.h>

#ifdef _WIN32
	#define	_O_BINARY	0x8000	/* Input and output is not translated. */
	_CRTIMP int __cdecl __MINGW_NOTHROW _setmode (int, int);
#endif

#define RIGHT_TO_LEFT 0

// Current state of looking for the sentinel sequence
char sentinel = 0;

// File pointers for the wrapper and the hidden file
FILE *fpw = 0, *fph = 0;

// Outputs the help message on how to use the program
void help()
{
	printf("\nUsage: steg -(bB) -(sr) -o<offset> [-i<interval>] -w<wrapper file> [-h<hidden file>]\n\n");
}

// Look for the sentinel sequence a byte at a time
// Returns 1 if complete sequence is found, 0 otherwise.
char end(unsigned char c)
{
	switch(sentinel)
	{
		case 0:
			if(0x00 == c)
				sentinel = 1;
			break;
		case 1:	// 00
			if(0xff == c)
				sentinel = 2;
			else if(0x00 == c)
				sentinel = 1;
			else
				sentinel = 0;
			break;
		case 2:	// 00 ff
			if(0x00 == c)
				sentinel = 3;
			else
				sentinel = 0;
			break;
		case 3:	// 00 ff 00
			if(0x00 == c)
				sentinel = 4;
			else if(0xff == c)
				sentinel = 2;
			else
				sentinel = 0;
			break;
		case 4:	// 00 ff 00 00
			if(0xff == c)
				sentinel = 5;
			else if(0x00 == c)
				sentinel = 1;
			else
				sentinel = 0;
			break;
		case 5:	// 00 ff 00 00 ff
			if(0x00 == c)
				return 1;
			else
				sentinel = 0;
			break;
	}
	return 0;
}


// Handle the cases of reaching EOF or read error while reading hidden file
int error_reading_hidden()
{
	int r = 1;
	char c;
	
	if(feof(fph))
	{
		// End of hidden file reached. Continue to output the remainder of the wrapper unmodified.
		for(;;)
		{
			if(0 == fread(&c, 1, 1, fpw))
			{
				if(feof(fpw))
				{
					// End of wrapper file reached.
					// Set return code to 0 (SUCCESS)
					r = 0;
				}
				else
				{
					fprintf(stderr, "Error reading wrapper file.\n");
				}
				
				// Close wrapper file
				fclose(fpw);
				
				break;
			}
			else
			{
				// Output wrapper byte to stdout
				putchar(c);
			}
		}
	}
	else
	{
		fprintf(stderr, "Error reading hidden file.\n");
	}
	
	// Close hidden file
	fclose(fph);
	
	return r;
}


// Handle the cases of reaching EOF or read error while reading wrapper file
int error_reading_wrapper()
{
	// Cant continue. If hidden file is open, close it.
	if(fph != 0)
	{
		fclose(fph);
	}
	
	if(feof(fpw))
	{
		fprintf(stderr, "End of wrapper file reached.\n");
	}
	else
	{
		fprintf(stderr, "Error reading wrapper file.\n");
	}
	
	// Close wrapper file
	fclose(fpw);
	
	return 1;
}

int main(int argc, char **argv)
{
	char SENTINEL[] = {0x00, 0xff, 0x00, 0x00, 0xff, 0x00};
	unsigned char c, h;
	char method, command, *s, *wrapper, *hidden;
	int i, j = 0, n, o = 0, offset, interval;
	char buffer[6];
	
	// Check if the minimum number of arguments were supplied
	if(argc	< 5)
	{
		help();
		return 1;
	}
	
	// Process 1st argument
	s = argv[++j];
	if(0 == strncmp("-b", s, 3))
	{
		// bit mode
		method = 'b';
	}
	else if(0 == strncmp("-B", s, 3))
	{
		// byte mode. There should be an additional argument.
		method = 'B';
		if(argc	< 6)
		{
			help();
			return 1;
		}
	}
	else
	{
		help();
		return 1;
	}
	
	// Process 2nd argument
	s = argv[++j];
	if(0 == strncmp("-s", s, 3))
	{
		// store mode
		command = 's';
	}
	else if(0 == strncmp("-r", s, 3))
	{
		// read mode
		command = 'r';
	}
	else
	{
		help();
		return 1;
	}
	
	// Process 3rd argument
	s = argv[++j];
	if(0 != strncmp("-o", s, 2))
	{
		help();
		return 1;
	}
	
	// Parse the offset value of the 3rd argument
	n = strlen(s);
	offset = 0;
	for(i = 2 /* skip the first two characters: '-o' */; i < n; ++i)
	{
		// Parse value as a decimal
		offset *= 10;
		c = s[i];
		if(isdigit(c))
			offset += c - '0';
		else
		{
			printf("Offset specification (-o<offset>) is invalid.");
			return 1;
		}
	}
	
	if('B' == method)
	{
		// Process 4th argument
		s = argv[++j];
		if(0 != strncmp("-i", s, 2))
		{
			help();
			return 1;
		}
		
		// Parse the interval value of the 4th argument
		n = strlen(s);
		interval = 0;
		for(i = 2 /* skip the first two characters: '-i' */; i < n; ++i)
		{
			// Parse value as a decimal
			interval *= 10;
			c = s[i];
			//asking if digit a 
			if(isdigit(c))
				interval += c - '0';
			else
			{
				printf("Interval specification (-i<interval>) is invalid.");
				return 1;
			}
		}
	}
	
	// Process 3rd or 4th argument depending on the mode
	s = argv[++j];
	if(0 != strncmp("-w", s, 2))
	{
		help();
		return 1;
	}
	
	// Skip the first two characters: '-w'
	wrapper = s + 2;
	
	if('s' == command)
	{
		// store mode
		++j;
		if(argc - 1	< j)	// check if next argument is available
		{
			help();
			return 1;
		}
		
		s = argv[j];
		if(0 != strncmp("-h", s, 2))
		{
			help();
			return 1;
		}
		
		// Skip the first two characters: '-h'
		hidden = s + 2;
	}
	
#ifdef _WIN32
	// Under windows, stdout processing has to be done in binary mode
	// so that carriage returns are not inserted before newlines
	_setmode(1, _O_BINARY);
#endif
	
	// open wrapper file
	fpw=fopen(wrapper, "rb");
	
	if('s' == command)
	{
		// open hidden file
		fph=fopen(hidden, "rb");
		
		// Read and output bytes of the wrapper file
		// upto the offset, unmodified.
		for(; o < offset; ++o)
		{
			if(0 == fread(&c, 1, 1, fpw))
			{
				return error_reading_wrapper();
			}
			else
			{
				putchar(c);
			}
		}
		
		if('b' == method)
		{
			// bit method
			
			j = 0;
			for(;;)
			{
				// fread returns the number of bytes read. If fread returns 0,
				// the next part of the condition starts reading the bytes
				// of the sentinel value.
				if(0 == fread(&h, 1, 1, fph) && (h = SENTINEL[j], j++ == 6))
				{
					return error_reading_hidden();
				}
				else
				{
					// for the hidden byte read, read and output 8 wrapper
					// bytes. The LSB of each wrapper byte is replaced with
					// a bit of the hidden byte
					for(i = 0; i < 8; ++i)
					{
						if(0 == fread(&c, 1, 1, fpw))
						{
							return error_reading_wrapper();
						}
						else
						{
							c &= 0xfe;	// Set the LSB of the wrapper byte to 0
							c |= (h & 0x80) >> 7;	// Set the LSB of the wrapper byte to the MSB of the hidden byte
							h <<= 1;	// Shift hidden byte to the left by 1
							putchar(c);
						}
					}
				}
			}
		}
		else	// 'B' == method
		{
			j = 0;
			for(o = 0;; ++o)
			{
				if(0 == fread(&c, 1, 1, fpw))
				{
					return error_reading_wrapper();
				}
				else
				{
					// at every interval, substitute the wrapper byte with the hidden byte
					// else simply output the wrapper byte unmodified
					if(0 == o % interval)
					{
						if(0 == fread(&h, 1, 1, fph) && (h = SENTINEL[j], j++ == 6))
						{
							putchar(c);
							return error_reading_hidden();
						}
						else
						{
							putchar(h);
						}
					}
					else
					{
						putchar(c);
					}
				}
			}
		}
		
		fclose(fph);
	}
	else	// 'r' == command
	{
		// skip contents of wrapper file upto offset
		if(0 != fseek(fpw, offset, SEEK_SET))
		{
			return error_reading_wrapper();
		}
		
		if('b' == method)
		{
			// this loop processes the first 5 bytes of the hidden file
			// extracted from the wrapper
			for(i = 0; i < 5; ++i)
			{
				h = 0;
				// To bulld a hidden byte, 8 wrapper bytes are processed
				for(j = 0; j < 8; ++j)
				{
					if(0 == fread(&c, 1, 1, fpw))
					{
						return error_reading_wrapper();
					}
					else
					{
						// The LSB is extracted from the wrapper byte c
						// Then shifted to the final position depending on
						// the value of RIGHT_TO_LEFT
						h |= (c & 0x01) << (RIGHT_TO_LEFT ? j : 7 - j);
					}
				}
				end(h);	// Check for sentinel
				
				// Buffer is used because the sentinel should be excluded.
				buffer[o++] = h;
			}
			for(;;)
			{
				h = 0;
				// To bulld a hidden byte, 8 wrapper bytes are processed
				for(j = 0; j < 8; ++j)
				{
					if(0 == fread(&c, 1, 1, fpw))
					{
						return error_reading_wrapper();
					}
					else
					{
						h |= (c & 0x01) << (RIGHT_TO_LEFT ? j : 7 - j);
					}
				}
				buffer[o++] = h;
				if(end(h))	// Check for sentinel
				{
					fclose(fpw);
					return 0;
				}
				o %= 6;	// Buffer is used as a circular one
				putchar(buffer[o]);
			}
		}
		else	// 'B' == method
		{
			// Process the first 5 bytes of the hidden file extracted from the wrapper
			for(i = 0; i < 5; ++i)
			{
				if(0 == fread(&c, 1, 1, fpw))
				{
					return error_reading_wrapper();
				}
				end(c);
				buffer[o++] = c;
				
				// Skip interval - 1 number of bytes
				if(0 != fseek(fpw, interval - 1, SEEK_CUR))
				{
					return error_reading_wrapper();
				}
			}
			for(;;)
			{
				if(0 == fread(&c, 1, 1, fpw))
				{
					return error_reading_wrapper();
				}
				buffer[o++] = c;
				if(end(c))
				{
					fclose(fpw);
					return 0;
				}
				o %= 6;
				putchar(buffer[o]);
				if(0 != fseek(fpw, interval - 1, SEEK_CUR))
				{
					return error_reading_wrapper();
				}
			}
		}
	}
	
	// Close wrapper
	fclose(fpw);
	
	return 0;
}
