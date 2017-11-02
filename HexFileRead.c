//
//  main.c
//  RawDiskIO
//
//  Created by WSY on 12-12-20.
//  Copyright (c) 2012 WSY. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

int BytesPerLine = 16;
unsigned long long StartOffset = 0;
unsigned long long MaxByteToRead = 1024;
char Hex[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
char temp[256] = { 0 };

int processingArgs(int argc, const char* argv[]);
void handleOffset(FILE *file);
int readLine(FILE *file);
void printLine(unsigned long long offset, int BytesRead);
void printUsage();

int main(int argc, const char * argv[])
{
    FILE *file = NULL;
    int BytesRead = 0;
    unsigned long long offset = 0;
    
    processingArgs(argc, argv);
    offset = StartOffset;
    BytesRead = BytesPerLine;

    printf("Begin.\n");
    file = fopen(argv[1], "rb");
    if (!file)
    {
        printf("Open failed!\n");
        return 1;
    }
    printf("Open succeed!\n");
    handleOffset(file);
    while ((offset < (MaxByteToRead + StartOffset)) && (BytesRead == BytesPerLine))
    {
        BytesRead = readLine(file);
        printLine(offset, BytesRead);
        offset += BytesPerLine;
    }
    return 0;
}

int processingArgs(int argc, const char* argv[])
{
    if (argc > 4)
    {
        sscanf(argv[4], "%d", &BytesPerLine);
        if (BytesPerLine > 256)
        {
            //here
        }
    }
    if (argc > 3)
    {
        sscanf(argv[2], "%lld", &StartOffset);
        sscanf(argv[3], "%lld", &MaxByteToRead);
    }
    else if (argc > 2)
    {
        sscanf(argv[2], "%lld", &MaxByteToRead);
    }
    if (argc > 1)
    {
        return 0;
    }
    else
    {
        printUsage();
    }
    return -1;
}

void handleOffset(FILE *file)
{
    if (StartOffset)
    {
		unsigned long long remainingOffset = StartOffset;
		fseek(file, 0, 0);
		while (remainingOffset>2147483647L)
		{
			int seek = fseek(file, 2047483648L, 1);
			if (seek)
			{
				printf("Skipping %lld bytes failed!\n", StartOffset);
				exit(2);
			}
			remainingOffset -= 2147483648L;
		}
        int seek = fseek(file, remainingOffset, 1);
        if (seek)
        {
            printf("Skipping %lld bytes failed!\n", StartOffset);
            exit(2);
        }
        printf("%lld bytes skipped!\n", StartOffset);
    }
}

int readLine(FILE *file)
{
    int BytesRead = 0;
    for (BytesRead = 0; BytesRead < BytesPerLine; BytesRead++)
    {
        fscanf(file, "%c", &temp[BytesRead]);
        if (feof(file))
        {
            break;
        }
    }
    return BytesRead;
}

void printLine(unsigned long long offset, int BytesRead)
{
    int i = 0;
    printf("%lld", offset);
    if (offset < 1000000000000000 && StartOffset + MaxByteToRead > 10000000000000000)
    {
        printf("        ");
    }
    if (offset < 10000000 && StartOffset + MaxByteToRead > 100000000)
    {
        printf("        ");
    }
    printf("\t");
    for (i = 0; i < BytesPerLine; i++)
    {
        if (i < BytesRead)
        {
            printf("%c%c ", Hex[(temp[i] >> 4) & 0x0F], Hex[temp[i] & 0x0F]);
        }
        else
        {
            printf("   ");
        }
    }
    printf("\t");
    for (i = 0; i < BytesPerLine; i++)
    {
        if (i < BytesRead)
        {
            if (((unsigned int)temp[i]) < 32 || ((unsigned int)temp[i]) > 127)
            {
                printf("%c", '.');
            }
            else
            {
                printf("%c", temp[i]);
            }
        }
        else
        {
            printf(" EOF\n");
            printf("Bytes Read In Last Line: %d\n", BytesRead);
            return;
        }
    }
    printf("\n");
}

void printUsage(void)
{
    printf("Usage:\n");
    printf("\tRawDiskIO fileName [<MaxByteToRead>]\n");
    printf("\tRawDiskIO fileName <StartOffset> <MaxByteToRead> [<BytesPerLine>]\n");
    exit(0);
}
