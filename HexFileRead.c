//
//  main.c
//  RawDiskIO
//
//  Created by WSY on 12-12-20.
//  Copyright (c) 2012 WSY. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

typedef unsigned long long UInt64;

int ProcessingArgs(int argc, const char* argv[]);
void HandleOffset(void);
int ReadLine(void);
void PrintLine(UInt64 offset, int BytesRead);
void PrintOffset(UInt64 offset);
void PrintHexValues(int BytesRead);
void PrintCharValues(int BytesRead);
void PrintUsage(const char *arg0);

int BytesPerLine = 16;
UInt64 StartOffset = 0;
UInt64 MaxByteToRead = 1024;
char Hex[256][3] = {
    "00","01","02","03","04","05","06","07","08","09","0A","0B","0C","0D","0E","0F",
    "10","11","12","13","14","15","16","17","18","19","1A","1B","1C","1D","1E","1F",
    "20","21","22","23","24","25","26","27","28","29","2A","2B","2C","2D","2E","2F",
    "30","31","32","33","34","35","36","37","38","39","3A","3B","3C","3D","3E","3F",
    "40","41","42","43","44","45","46","47","48","49","4A","4B","4C","4D","4E","4F",
    "50","51","52","53","54","55","56","57","58","59","5A","5B","5C","5D","5E","5F",
    "60","61","62","63","64","65","66","67","68","69","6A","6B","6C","6D","6E","6F",
    "70","71","72","73","74","75","76","77","78","79","7A","7B","7C","7D","7E","7F",
    "80","81","82","83","84","85","86","87","88","89","8A","8B","8C","8D","8E","8F",
    "90","91","92","93","94","95","96","97","98","99","9A","9B","9C","9D","9E","9F",
    "A0","A1","A2","A3","A4","A5","A6","A7","A8","A9","AA","AB","AC","AD","AE","AF",
    "B0","B1","B2","B3","B4","B5","B6","B7","B8","B9","BA","BB","BC","BD","BE","BF",
    "C0","C1","C2","C3","C4","C5","C6","C7","C8","C9","CA","CB","CC","CD","CE","CF",
    "D0","D1","D2","D3","D4","D5","D6","D7","D8","D9","DA","DB","DC","DD","DE","DF",
    "E0","E1","E2","E3","E4","E5","E6","E7","E8","E9","EA","EB","EC","ED","EE","EF",
    "F0","F1","F2","F3","F4","F5","F6","F7","F8","F9","FA","FB","FC","FD","FE","FF"
};
char temp[256] = { 0 };
FILE *file = NULL;

int main(int argc, const char * argv[])
{
    int BytesRead = 0;
    UInt64 offset = 0;
    
    ProcessingArgs(argc, argv);
    offset = StartOffset;
    BytesRead = BytesPerLine;

    file = fopen(argv[1], "rb");
    if (!file)
    {
        printf("Open failed!\n");
        return 1;
    }
    printf("Open succeed!\n");
    HandleOffset();
    while ((offset < (MaxByteToRead + StartOffset)) && (BytesRead == BytesPerLine))
    {
        BytesRead = ReadLine();
        PrintLine(offset, BytesRead);
        offset += BytesPerLine;
    }
    return 0;
}

int ProcessingArgs(int argc, const char* argv[])
{
    if (argc > 4)
    {
        sscanf(argv[4], "%d", &BytesPerLine);
        if (BytesPerLine > 256)
        {
            BytesPerLine = 256;
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
        PrintUsage(argv[0]);
    }
    return -1;
}

void HandleOffset()
{
    if (StartOffset)
    {
        /*
         * Here's the tricky thing.
         * Under Windows, if I don't call fread before I call the first fseek,
         * any invocation to fread after fseek will fail, if file points to a device file.
         * e.g. \\.\PhysicalDrive0
         */
        fread(temp, sizeof(char), 1, file);
        /* Tricky thing ends */
        int seek = fseek(file, StartOffset, SEEK_SET);
        if (seek)
        {
            printf("Skipping %lld bytes failed!\n", StartOffset);
            exit(2);
        }
        printf("%lld bytes skipped!\n", StartOffset);
    }
}

int ReadLine()
{
    int BytesRead = (int)fread(temp, sizeof(char), BytesPerLine, file);
#ifdef DEBUG
    if(BytesRead < BytesPerLine)
    {
        printf("DEBUG\tErrorNo: %d\n",ferror(file));
    }
#endif
    return BytesRead;
}

void PrintLine(UInt64 offset, int BytesRead)
{
    PrintOffset(offset);
    printf("\t");
    PrintHexValues(BytesRead);
    printf("\t");
    PrintCharValues(BytesRead);
    printf("\n");
}

void PrintOffset(UInt64 offset)
{
    printf("%lld", offset);
    if (offset < 1000000000000000 && StartOffset + MaxByteToRead > 10000000000000000)
    {
        printf("        ");
    }
    if (offset < 10000000 && StartOffset + MaxByteToRead > 100000000)
    {
        printf("        ");
    }
}

void PrintHexValues(int BytesRead)
{
    int i = 0;
    for (i = 0; i < BytesPerLine; i++)
    {
        if (i < BytesRead)
        {
            printf("%s ", Hex[temp[i] & 0x000000FF]);
        }
        else
        {
            printf("   ");
        }
    }
}

void PrintCharValues(int BytesRead)
{
    int i = 0;
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
}

void PrintUsage(const char *arg0)
{
    printf("Usage:\n");
    printf("\t%s fileName [<MaxByteToRead>]\n", arg0);
    printf("\t%s fileName <StartOffset> <MaxByteToRead> [<BytesPerLine>]\n", arg0);
    exit(0);
}
