// File - catpng.c
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>  /* for printf().  man 3 printf */
#include <stdlib.h> /* for exit().    man 3 exit   */
#include <string.h> /* for strcat().  man strcat   */
#include "zutil.h"
#include "lab_png.h"
#include "crc.h"

#define BUF_LEN (256 * 16 * 16 * 16)
#define BUF_LEN2 (256 * 16 * 16 * 16)
#define MASSIVE_LEN (256 * 16 * 16 * 16)

// U8 gp_buf_def[BUF_LEN2]; /* output buffer for mem_def() */
// U8 gp_buf_inf[BUF_LEN2]; /* output buffer for mem_inf() */

int main(int argc, char *argv[])
{
    if (argc > 50)
    {
        printf("File limit is 50\n");
        return 1;
    }

    U32 length = 0;
    U32 length_IDAT = 0;
    U8 type[4];
    U32 crc_val = 0; /* CRC value                                     */
    int ret = 0;     /* return value for various routines             */
    U64 len_def = 0; /* compressed data length                        */
    U64 len_inf = 0; /* uncompressed data length                      */
    // printf("This is catpng file\n");
    chunk_p IDHR_chunk[50];
    chunk_p IDAT_chunk[50];
    chunk_p IEND_chunk[50];
    data_IHDR_p data_IHDR[50];
    U32 data_lengths_IDAT[50];

    simple_PNG_p PNG[50];

    int i;

    U8 *p_inflate_buffer = malloc(MASSIVE_LEN);
    U8 *p_buffer = malloc(BUF_LEN2);

    // printf("argc: %d\n", argc);

    FILE *png_out = fopen("all.png", "wb");
    if (png_out == NULL)
    {
        printf("all.png was not created \n");
        return 1;
    }
    for (i = 1; i < argc; i++)
    {
        // printf("argv[%d]=%s\n", i, argv[i]);

        PNG[i].p_IHDR = &IDHR_chunk[i];
        PNG[i].p_IDAT = &IDAT_chunk[i];
        PNG[i].p_IEND = &IEND_chunk[i];

        FILE *png_file = fopen(argv[i], "rb");
        if (png_file == NULL)
        {
            printf("Failed to Open file %s\n", argv[i]);
            return 1;
        }
        // printf("File: %s\n", argv[i]);
        if (i == 1)
        {
            char head[8];
            fread(head, 8, 1, png_file);
            fwrite(head, 8, 1, png_out);
        }
        fseek(png_file, 8, SEEK_SET);

        length = 0;
        fread(&length, 4, 1, png_file);
        length = ntohl(length);
        IDHR_chunk[i].length = length;

        memset(IDHR_chunk[i].type, 0, sizeof(IDHR_chunk[i].type));
        memset(type, 0, sizeof(type));
        fread(type, 4, 1, png_file);
        memcpy(IDHR_chunk[i].type, type, 4);

        // IDHR_chunk[i]->p_data = data;

        U32 width;
        fread(&width, 4, 1, png_file);
        width = ntohl(width);
        data_IHDR[i].width = width;
        // printf("Width: %d\n", width);

        U32 height;
        fread(&height, 4, 1, png_file);
        height = ntohl(height);
        data_IHDR[i].height = height;
        // printf("Height: %d\n", height);

        U8 bit_depth;
        fread(&bit_depth, 1, 1, png_file);
        data_IHDR[i].bit_depth = bit_depth;

        U8 colour_type;
        fread(&colour_type, 1, 1, png_file);
        data_IHDR[i].colour_type = colour_type;

        U8 compression;
        fread(&compression, 1, 1, png_file);
        data_IHDR[i].compression = compression;

        U8 filter;
        fread(&filter, 1, 1, png_file);
        data_IHDR[i].filter = filter;

        U8 interlace;
        fread(&interlace, 1, 1, png_file);
        data_IHDR[i].interlace = interlace;

        crc_val = 0;
        fread(&crc_val, 4, 1, png_file);
        IDHR_chunk[i].crc = crc_val;

        length_IDAT = 0;
        fread(&length_IDAT, 4, 1, png_file);
        length_IDAT = ntohl(length_IDAT);
        IDAT_chunk[i].length = length_IDAT;
        // printf("IDAT Length: %d\n", IDAT_chunk[i].length);

        memset(IDAT_chunk[i].type, 1, sizeof(IDAT_chunk[i].type));
        memset(type, 0, sizeof(type));
        fread(type, 4, 1, png_file);
        memcpy(IDAT_chunk[i].type, type, 4);
        // printf("IDAT: %4.4s\n", IDAT_chunk[i].type);

        memset(p_buffer, 0, sizeof(p_buffer));
        ret = fread(p_buffer, 1, IDAT_chunk[i].length, png_file);
        // printf("IDAT read elements: %d\n", ret);
        IDAT_chunk[i].p_data = malloc(IDAT_chunk[i].length);
        memset(IDAT_chunk[i].p_data, 0, IDAT_chunk[i].length);
        memcpy(IDAT_chunk[i].p_data, p_buffer, IDAT_chunk[i].length);
        // printf("IDAT Data: %x \n", IDAT_chunk[i].p_data);

        crc_val = 0;
        fread(&crc_val, 4, 1, png_file);
        IDAT_chunk[i].crc = crc_val;
        // printf("IDAT crc: %x\n", crc);

        length = 0;
        fread(&length, 4, 1, png_file);
        IEND_chunk[i].length = length;
        // printf("IEND Length: %d\n", IEND_chunk[i].length);

        memset(type, 0, sizeof(type));
        fread(type, 4, 1, png_file);
        memcpy(IEND_chunk[i].type, type, 4);
        // printf("IEND: %4.4s\n", IEND_chunk[i].type);

        crc_val = 0;
        fread(&crc_val, 4, 1, png_file);
        IEND_chunk[i].crc = crc_val;
        // printf("IEND crc: %x\n", crc);

        fclose(png_file);
    }

    U32 width_total = data_IHDR[1].width;
    U32 height_total = 0;
    for (int i = 1; i < argc; i++)
    {
        height_total += data_IHDR[i].height;
    }

    // printf("All Height: %d\n", height_total);
    // printf("All Width: %d\n", width_total);

    U64 total_inflated_length = 0;

    memset(p_buffer, 0, sizeof(p_buffer));
    memset(p_inflate_buffer, 0, sizeof(p_inflate_buffer));

    for (int i = 1; i < argc; i++)
    {
        // printf("File: %s\n", argv[i]);
        memset(p_buffer, 0, BUF_LEN2);
        ret = mem_inf(p_buffer, &len_inf, IDAT_chunk[i].p_data, IDAT_chunk[i].length);
        if (ret == 0)
        { /* success */
            // printf("original len = %d, len_def = %lu, len_inf = %u\n", BUF_LEN2, IDAT_chunk[i].length, len_inf);
        }
        else
        { /* failure */
            fprintf(stderr, "mem_def failed. ret = %d.\n", ret);
        }
        memcpy(p_inflate_buffer + total_inflated_length, p_buffer, len_inf);
        total_inflated_length += len_inf;
    }

    // printf("Total inflated length: %lu\n", total_inflated_length);

    U64 total_deflated_length = 0;

    memset(p_buffer, 0, sizeof(p_buffer));
    ret = mem_def(p_buffer, &total_deflated_length, p_inflate_buffer, total_inflated_length, Z_DEFAULT_COMPRESSION);
    if (ret == 0)
    { /* success */
        // printf("original len = %lu, len_def = %lu\n", total_inflated_length, total_deflated_length);
    }
    else
    { /* failure */
        fprintf(stderr, "mem_def failed. ret = %d.\n", ret);
        return ret;
    }

    fclose(png_out);

    png_out = fopen("all.png", "rb+");
    if (png_out == NULL)
    {
        printf("all.png was unable to open \n");
        return 1;
    }

    fseek(png_out, 8, SEEK_SET);
    IDHR_chunk[1].length = htonl(IDHR_chunk[1].length);
    fwrite(&IDHR_chunk[1].length, 4, 1, png_out);
    fwrite(IDHR_chunk[1].type, 4, 1, png_out);
    width_total = htonl(width_total);
    fwrite(&width_total, 4, 1, png_out);
    height_total = htonl(height_total);
    fwrite(&height_total, 4, 1, png_out);
    fwrite(&data_IHDR[1].bit_depth, 1, 1, png_out);
    fwrite(&data_IHDR[1].colour_type, 1, 1, png_out);
    fwrite(&data_IHDR[1].compression, 1, 1, png_out);
    fwrite(&data_IHDR[1].filter, 1, 1, png_out);
    fwrite(&data_IHDR[1].interlace, 1, 1, png_out);
    fseek(png_out, 12, SEEK_SET);
    U8 IHDR_buffer[17];
    memset(IHDR_buffer, 0, sizeof(IHDR_buffer));
    fread(IHDR_buffer, 17, 1, png_out);
    U64 crc_IHDR = crc(IHDR_buffer, 17);
    crc_IHDR = htonl(crc_IHDR);
    fseek(png_out, 0, SEEK_END);
    fwrite(&crc_IHDR, 4, 1, png_out);

    fseek(png_out, 0, SEEK_END);
    total_deflated_length = htonl(total_deflated_length);
    fwrite(&total_deflated_length, 4, 1, png_out);
    fwrite(IDAT_chunk[1].type, 4, 1, png_out);
    total_deflated_length = ntohl(total_deflated_length);
    // printf("total_deflated_length: %lu\n", total_deflated_length);
    fwrite(p_buffer, total_deflated_length, 1, png_out);
    fseek(png_out, 37, SEEK_SET);
    char *IDAT_buffer = malloc(total_deflated_length + 4);
    memset(IDAT_buffer, 0, sizeof(IDAT_buffer));
    fread(IDAT_buffer, 4 + total_deflated_length, 1, png_out);
    U64 crc_IDAT = crc(IDAT_buffer, 4 + total_deflated_length);
    fseek(png_out, 0, SEEK_END);
    crc_IDAT = htonl(crc_IDAT);
    fwrite(&crc_IDAT, 4, 1, png_out);
    free(IDAT_buffer);

    fwrite(&IEND_chunk[1].length, 4, 1, png_out);
    fwrite(IEND_chunk[1].type, 4, 1, png_out);
    fwrite(&IEND_chunk[1].crc, 4, 1, png_out);

    for (int i = 1; i < argc; i++)
    {
        free(IDAT_chunk[i].p_data);
    }
    fclose(png_out);
    free(p_buffer);
    free(p_inflate_buffer);
    return 0;
}