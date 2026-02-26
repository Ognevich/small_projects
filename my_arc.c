#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define FILE_PATH_BUFFER 4096
#define FILE_NAME_BUFFER 255

typedef struct 
{
    char filename[FILE_NAME_BUFFER];
    char filepath[FILE_PATH_BUFFER];

    uint32_t file_size;
}File_data;


static int is_file(const char * filename)
{
    struct stat st;

    if (stat(filename,&st) == 0)
    {
        if (S_ISREG(st.st_mode))
            return 1;
    }
    return 0;
}

static int extract_file_name(const char *filepath, char *filename) 
{
    const char *slash = strrchr(filepath, '/');

    if (slash != NULL)
        strcpy(filename, slash + 1);
    else
        strcpy(filename, filepath);

    if (!is_file(filepath))  
    {
        printf("Object %s is not a file\n", filepath);
        return 0;
    }

    return 1;
}

static int parse_files_data(File_data **files, int *files_size, char ** argv, int argc)
{
    File_data * tmp = malloc(sizeof(File_data));
    if (!tmp)
        return 0;
    *files_size = 0;
    int capacity = 1;

    for (int i = 2; i < argc; i++)
    {
        if (capacity <= *files_size)
        {
            capacity *= 2;
            tmp = realloc(tmp, sizeof(File_data ) * 2);
            if (!tmp)
                return 0;
        }

        char filepath[FILE_PATH_BUFFER];        
        char filename[FILE_NAME_BUFFER];
        
        strcpy(filepath, argv[i]);
        if (!extract_file_name(filepath, filename))
        {
            return 0;
        }
        
        *files_size++;
    }

    *files = tmp;
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
    if (!parse_files_data(&files,&files_size, argv, argc))
    {
        return 1;
    }

    free(files);
    files = NULL;

    return 0;
}