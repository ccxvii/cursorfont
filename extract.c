#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char line[256];
char *charname;
int bbx[4];
char bitmap[256][256];

char fill[32 * 32];
char mask[32 * 32];

#define get1(buf,x) ((buf[x >> 3] >> ( 7 - (x & 7) ) ) & 1 )

char *
readline(void)
{
	int n;
	if (fgets(line, sizeof line, stdin))
	{
		n = strlen(line);
		line[n-1] = 0; /* remove newline */
		return line;
	}
	return NULL;
}

int
unhex(int c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return 0;
}

void
unhexrow(char *raw, char *hex)
{
	int a, b;
	while (hex[0] && hex[1])
	{
		a = hex[0];
		b = hex[1];
		hex += 2;
		*raw++ = unhex(a) * 16 + unhex(b);
	}
	*raw = 0;
}

void
readbitmap(void)
{
	int y;
	for (y = 0; y < bbx[1]; y++)
	{
		readline();
		unhexrow(bitmap[y], line);
	}
}

void
copybitmap(char *out)
{
	int row, col;
	int dx = 16 + bbx[2];
	int dy = 16 - bbx[1] - bbx[3];

	memset(out, 0, 32 * 32);

	for (row=0; row<bbx[1]; row++)
	{
		for (col=0; col<bbx[0]; col++)
		{
			if (get1(bitmap[row], col))
				out[(row + dy) * 32 + col + dx] = 1;
		}
	}
}

void
emitchar(void)
{
	char filename[256];
	char *p;
	FILE *fp;
	int row, col;

	p = strstr(charname, "_mask");
	if (p)
		*p = 0;

	sprintf(filename, "raw/%s.pam", charname);
	fp = fopen(filename, "wb");
	fprintf(fp, "P7\nWIDTH 32\nHEIGHT 32\nDEPTH 4\nMAXVAL 255\nTUPLTYPE RGB_ALPHA\nENDHDR\n");

	for (row=0; row<32; ++row)
	{
		for (col=0; col<32; col++)
		{
			if (fill[row*32+col])
			{
				fputc(0, fp);
				fputc(0, fp);
				fputc(0, fp);
			}
			else
			{
				fputc(255, fp);
				fputc(255, fp);
				fputc(255, fp);
			}
			if (mask[row*32+col])
				fputc(255, fp);
			else
				fputc(0, fp);
		}
	}

	fclose(fp);
}

int
main(int argc, char **argv)
{
	while(readline())
	{
		if (strstr(line, "STARTCHAR") == line)
			charname = strdup(line + 10);
		if (strstr(line, "BBX") == line)
			sscanf(line + 4, "%d %d %d %d", bbx+0, bbx+1, bbx+2, bbx+3);
		if (strstr(line, "BITMAP") == line)
			readbitmap();
		if (strstr(line, "ENDCHAR") == line)
		{
			if (strstr(charname, "_mask"))
			{
				copybitmap(mask);
				emitchar();
			}
			else
			{
				copybitmap(fill);
			}
		}
	}
}

