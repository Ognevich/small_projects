#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define FILE_PATH_BUFFER 4096
#define FILE_NAME_BUFFER 255

typedef struct 
{
    char filename[FILE_NAME_BUFFER];
    char filepath[FILE_PATH_BUFFER];

    uint32_t file_size;
}File_data;

static int parse_files_data(File_data **files, int *files_size)
{
    *files = malloc(sizeof(File_data));
    if (!*files)
        return 0;

    *files_size = 1;
    return 1;
}


int main(int argc, char * argv[])
{
    if (argc < 3)
    {
        printf("Error: insufficient argument type\n");
        return 1;
    }

    int files_size = 0;
    File_data * files = NULL;
    if (!parse_files_data(&files,&files_size))
    {
        return 1;
    }


    return 0;
}