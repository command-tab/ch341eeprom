#include <stdio.h>

int main(int argc, char **argv)
{
    static char *hex = "\00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff";

    for (int j = 0; j < 256/16; j++)
	{
        for (int i = 0; i < 16; i++)
		{
            fputc(hex[j%16], stdout);
		}
	}
	
	return 0;
}

