#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <inttypes.h>

#define FILE_PATH_BUFFER 4096
#define FILE_NAME_BUFFER 255

typedef struct 
{
    char filepath[FILE_PATH_BUFFER];
    char filename[FILE_NAME_BUFFER];
    
    off_t file_size;
}File_data;


static int is_file(const char *filepath)
{
    struct stat st;

    if (stat(filepath, &st) == 0)
    {
        if (S_ISREG(st.st_mode))
        {
            return 1;
        }
        else
        {
            printf("object %s is not a file\n", filepath);
            return 0;
        }
    }
    else
    {
        printf("object %s doesn't exist\n", filepath);
        return 0;
    }
}

static void extract_filename(char * filepath, char * filename)
{
    const char *slash = strrchr(filepath, '/');

    const char * name = slash ? slash + 1 : filepath;

    strncpy (filename, name,FILE_NAME_BUFFER-1);
    filename[FILE_NAME_BUFFER-1]='\0';

}

static void get_file_size(const char * filepath, off_t * file_size)
{
    struct stat st;
    if (stat(filepath, &st) != 0)
        return;
    *file_size = st.st_size;
}
       

static inline void free_files(File_data **files)
{
    if (files && *files)
    {
        free(*files);
        *files = NULL;
    }
}

static inline int increase_files(File_data **files, int *files_size, int *capacity)
{
    if (*files_size >= *capacity)
    {
        *capacity *= 2;

        File_data *tmp = realloc(*files, sizeof(File_data) * (*capacity));
        if (!tmp)
            return 0;

        *files = tmp;
    }

    return 1;
}

static int execute_file_adding(File_data * file_data, char ** argv, int index)
{
        strncpy(file_data->filepath, argv[index], FILE_PATH_BUFFER-1);
        file_data->filepath[FILE_PATH_BUFFER-1] = '\0';

        if (!is_file(file_data->filepath)) {
            return 0;
        }
        
        extract_filename(file_data->filepath,file_data->filename);
        get_file_size(file_data->filepath, &file_data->file_size);

        return 1;
}

static int parse_files_data(File_data **files, int *files_size, char ** argv, int argc)
{
    File_data * tmp = malloc(sizeof(File_data));
    if (!tmp)
        return 0;

    int size     = 0;
    int capacity = 1;

    for (int i = 2; i < argc; i++)
    {    
        if (!increase_files(&tmp, &size, &capacity))
        {
            free(tmp);
            return 0;
        }

        if (!execute_file_adding(&tmp[size],argv,i))
        {
            free(tmp);
            return 0;
        }

        size++;
    }

    *files_size = size;
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
        return 1;
    

    for (int i = 0; i < files_size; i++)
    {
        printf("filepath %s\n", files[i].filepath);
        printf("filename %s\n", files[i].filename);
        printf("file size: %" PRIdMAX "\n", (intmax_t)files[i].file_size);
        printf("\n");
    }



    free_files(&files);
    return 0;
}