#include <stdio.h>
#include "crc.h"
#include <arpa/inet.h> //For ntohl
#include <string.h>    //For memcmp
#include <stdlib.h>    //For exit

int is_png(char *file);

int main(int argc, char *argv[])
{

    if (argc != 2)
    {

        printf("Error --  Must have only 1 argument\n");
    }
    else
    {

        FILE *pngfile;
        char *filename = argv[1];
        char *justpng;
        if (strchr(filename, '/') != NULL)
        {
            justpng = strrchr(argv[1], '/');
            justpng = justpng + 1;
        }
        else
        {
            justpng = argv[1];
        }
        unsigned int width;
        unsigned int height;
        //char width_string[10];
        //char height_string[10];

        if (is_png(filename) == 1)
        { // Note to self: Double check the functionality of argv function

            pngfile = fopen(filename, "rb");

            //Set file pointer to 16 byte position in png file
            fseek(pngfile, 16, SEEK_SET);

            //Read 4 bytes from 16 byte position

            fread(&width, 4, 1, pngfile);
            width = ntohl(width);

            //Read 4 bytes from 20 byte position
            fread(&height, 4, 1, pngfile);
            height = ntohl(height);

            unsigned int IHDR_CRC;
            unsigned int IHDR_Length;
            unsigned char IHDR_Type[4];
            unsigned char IHDR_Data[13];
            unsigned int IDAT_CRC;
            unsigned int IDAT_Length;
            unsigned char IDAT_Type[4];
            unsigned char IDAT_Data[256 * 128 * 16];
            unsigned int IEND_CRC;
            unsigned int IEND_Length;
            unsigned char IEND_Type[4];

            unsigned int Computed_CRC;
            unsigned char buffer[256 * 128 * 16];

            fseek(pngfile, 8, SEEK_SET);

            fread(&IHDR_Length, 4, 1, pngfile);
            IHDR_Length = ntohl(IHDR_Length);

            fread(&IHDR_Type, 4, 1, pngfile);

            fread(&IHDR_Data, IHDR_Length, 1, pngfile);

            fread(&IHDR_CRC, 4, 1, pngfile);
            IHDR_CRC = ntohl(IHDR_CRC);

            memset(buffer, 0, 256 * 128);

            memcpy(buffer, IHDR_Type, 4);
            memcpy(buffer + 4, IHDR_Data, 13);

            Computed_CRC = crc(buffer, 4 + IHDR_Length);

            // Compare CRC values for IHDR:
            if (Computed_CRC != IHDR_CRC)
            {

                //Print: filename: width x height (both in decimal)
                printf("%s: %d x %d\n", justpng, width, height);

                //IHDR chunk CRC error: computed Computed_CRC, expected IHDR_CRC
                printf("IHDR chunk CRC error: computed %x, expected %x\n", Computed_CRC, IHDR_CRC);

                fclose(pngfile);

                exit(0);
            }

            fread(&IDAT_Length, 4, 1, pngfile);
            IDAT_Length = ntohl(IDAT_Length);

            fread(&IDAT_Type, 4, 1, pngfile);

            fread(&IDAT_Data, IDAT_Length, 1, pngfile);

            fread(&IDAT_CRC, 4, 1, pngfile);
            IDAT_CRC = ntohl(IDAT_CRC);

            memset(buffer, 0, 256 * 128 * 16);

            memcpy(buffer, IDAT_Type, 4);
            memcpy(buffer + 4, IDAT_Data, IDAT_Length);

            Computed_CRC = crc(buffer, 4 + IDAT_Length);

            //Compare CRC values for IDAT:
            if (Computed_CRC != IDAT_CRC)
            {

                //Print: filename: width x height (both in decimal)
                printf("%s: %d x %d\n", justpng, width, height);

                //IDAT chunk CRC error: computed var, expected IDAT_CRC
                printf("IDAT chunk CRC error: computed %x, expected %x\n", Computed_CRC, IDAT_CRC);

                fclose(pngfile);
                exit(0);
            }

            fread(&IEND_Length, 4, 1, pngfile);
            IEND_Length = ntohl(IEND_Length);

            fread(&IEND_Type, 4, 1, pngfile);

            fread(&IEND_CRC, 4, 1, pngfile);
            IEND_CRC = ntohl(IEND_CRC);

            memset(buffer, 0, 256 * 128);

            memcpy(buffer, IEND_Type, 4);

            Computed_CRC = crc(buffer, 4 + IEND_Length);

            //Compare CRC values for IEND
            if (Computed_CRC != IEND_CRC)
            {

                //Print: filename: width x height (both in decimal)
                printf("%s: %d x %d\n", justpng, width, height);

                //IEND chunk CRC error: computed var, expected IEND_CRC
                printf("IEND chunk CRC error: computed %x, expected %x\n", Computed_CRC, IEND_CRC);

                fclose(pngfile);
                exit(0);
            }

            //Print: filename: width x height (both in decimal)
            printf("%s: %d x %d\n", justpng, width, height);
            fclose(pngfile);
        }
        else /*If not a png file*/
        {

            //Print: Filename: Not a PNG file

            printf("%s: Not a PNG file\n", justpng);
        }
    }

    //printf("This is pnginfo file\n");

    return 0;
}

int is_png(char *file)
{

    FILE *file_png = fopen(file, "rb");
    // points to the start of the three bytes which confirm png file
    fseek(file_png, 0, SEEK_SET);

    unsigned char head[8];
    // hex data for png file
    unsigned char png_head[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};

    if (fread(head, sizeof(head), 1, file_png))
    {
        // compares the read binary file and the valid hex data for a png file
        if (memcmp(head, png_head, 8) == 0)
        {
            fclose(file_png);
            return 1;
        }
        else
        {
            fclose(file_png);
            return 0;
        }
    }
    fclose(file_png);
    return 0;
}