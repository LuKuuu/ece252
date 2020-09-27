#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include "lab_png.h"

char *join(const char *dir_name, const char *file_name)
{
    char *result = malloc(strlen(dir_name) + strlen(file_name) + 2); // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, dir_name);
    strcat(result, "/");
    strcat(result, file_name);
    return result;
}

//return 0 if retular file
//return 1 if directory
//return -1 if error or other type file
int checktype(char *file_name)
{
    struct stat buf;
    if (lstat(file_name, &buf) < 0)
    {
        perror("lstat error");
        return -1;
    }

    if (S_ISREG(buf.st_mode))
    {
        return 0;
    }
    else if (S_ISDIR(buf.st_mode))
    {
        return 1;
    }
    return -1;
}

void list(char *dir_name)
{
    DIR *p_dir;
    struct dirent *p_dirent;

    if ((p_dir = opendir(dir_name)) == NULL)
    {
        return;
    }

    while ((p_dirent = readdir(p_dir)) != NULL)
    {
        char *file_name = p_dirent->d_name; /* relative path name! */
        if (file_name == NULL)
        {
            return;
        }
        else
        {
            if ((strcmp(".", file_name) == 0) || (strcmp("..", file_name) == 0))
            {
                continue;
            }

            char *file_path = join(dir_name, file_name);
            int file_type = checktype(file_path);
            if (file_type == 1)
            {
                list(file_path);
            }
            else if (file_type == 0)
            {
                FILE *file;
                U8 signature_buffer[PNG_SIG_SIZE];
                file = fopen(file_path, "rb");
                fread(signature_buffer, PNG_SIG_SIZE, 1, file);
                if (is_png(signature_buffer, PNG_SIG_SIZE))
                {
                    printf("%s\n", file_path);
                }

                fclose(file);
            }

            if (file_path != NULL)
            {
                free(file_path);
                file_path = NULL;
            }
        }
    }

    if (closedir(p_dir) != 0)
    {
        perror("closedir");
        return;
    }
}

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        list(argv[1]);
    }
    else
    {
        printf("Invalid input");
    }

    return 0;
}
