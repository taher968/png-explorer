// File - findpng.c
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>  /* for printf().  man 3 printf */
#include <stdlib.h> /* for exit().    man 3 exit   */
#include <string.h> /* for strcat().  man strcat   */

int is_png(char *file)
{
    FILE *file_png = fopen(file, "rb");
    // points to the start of the eight bytes which confirm png file
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

void list_png_files(char *directory, int *count)
{
    DIR *p_dir;
    struct dirent *p_dirent;
    char str[64];

    // opening the passed directory
    p_dir = opendir(directory);

    if (p_dir == NULL)
    {
        sprintf(str, "opendir(%s)", directory);
        perror(str);
        exit(2);
    }
    // reads the directory contents one at a time
    while ((p_dirent = readdir(p_dir)) != NULL)
    {
        // if directory file type
        if (p_dirent->d_type == DT_DIR)
        {
            // ignores current directory and parent directory specifiers
            if (strcmp(p_dirent->d_name, ".") != 0 && strcmp(p_dirent->d_name, "..") != 0)
            {
                // appends name to hold entire traversed directory and makes recursive call
                char name[2048];
                snprintf(name, sizeof(name), "%s/%s", directory, p_dirent->d_name);
                list_png_files(name, count);
            }
        }
        else
        {
            // if file type
            char file_name[94098];
            snprintf(file_name, sizeof(file_name), "%s/%s", directory, p_dirent->d_name);
            // checks if file has valid png heading
            int result = is_png(file_name);
            if (result == 1)
            {
                *count = *count + 1;
                printf("%s/%s\n", directory, p_dirent->d_name);
            }
        }
    }
    closedir(p_dir);
}

int main(int argc, char *argv[])
{

    if (argc == 1)
    {
        fprintf(stderr, "Usage: %s <directory name>\n", argv[0]);
        exit(1);
    }

    int *count = malloc(sizeof(int));
    *count = 0;

    // recursively checks the directories for files
    list_png_files(argv[1], count);

    if (*count == 0)
    {
        printf("findpng: No PNG file found\n");
    }

    free(count);

    return 0;
}