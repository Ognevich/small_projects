#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <inttypes.h>

#ifdef _WIN32
    #include <direct.h>
#else
    #include <unistd.h>
#endif


#define FILE_PATH_BUFFER 4096
#define UPLOAD_SIZE      4096
#define FILE_NAME_BUFFER 255

#define PACK_START_POS 3


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

static inline int increase_files(File_data **files, size_t *files_size, size_t *capacity)
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

static int parse_files_data(File_data **files, size_t *files_size, char ** argv, int argc)
{
    File_data * tmp = malloc(sizeof(File_data));
    if (!tmp)
        return 0;

    size_t size     = 0;
    size_t capacity = 1;

    for (int i = PACK_START_POS; i < argc; i++)
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


static char * get_current_filepath()
{
    char *cwd = getcwd(NULL, 0);
    if (!cwd)
    {
        perror("getcwd");
        return NULL;
    }
    return cwd;

}

static int check_filename_validation(char * filename)
{

    char * type = strrchr(filename, '.');
    printf("%s\n",type);
    if (strcmp(type, ".myarc") != 0)
    {
        printf("Error: unknown file extension %s\n", type);
        return 0;
    }


    return 1;
}

////////DEBUGGING///////////

static inline void print_size_info(size_t raw_size, off_t archieve_size)
{
    printf("Raw size: %zu\n", raw_size);
    printf("file size: %" PRIdMAX "\n", (intmax_t)archieve_size);
}

static void printf_files(File_data * files, size_t files_size)
{
    for (int i = 0; i < files_size; i++)
    {
        printf("filepath %s\n", files[i].filepath);
        printf("filename %s\n", files[i].filename);
        printf("file size: %" PRIdMAX "\n", (intmax_t)files[i].file_size);
        printf("\n");
    }
}

////////////////////////////

static int pack_files(File_data *files, size_t files_size, char * pack_name)
{
    char *filepath = get_current_filepath();
    if (!filepath) return 0;

    const char *archive_name = pack_name;
    char full_path[FILE_PATH_BUFFER];
    snprintf(full_path, sizeof(full_path), "%s/%s", filepath, archive_name);

    size_t raw_size = 0;
    off_t archieve_size = 0;

    FILE *archive = fopen(full_path, "wb");
    if (!archive) {
        free(filepath);
        return 0;
    }

    fwrite(&files_size,sizeof(size_t),1,archive);

    for (size_t i = 0; i < files_size; i++)
    {

        size_t name_len = strlen(files[i].filename);
        fwrite(&name_len, sizeof(size_t), 1, archive);

        fwrite(files[i].filename, 1, name_len, archive);

        FILE *current_file = fopen(files[i].filepath, "rb");
        if (!current_file) {
            perror("Can't open file");
            fclose(archive);
            free(filepath);
            return 0;
        }

        fseek(current_file, 0, SEEK_END);
        size_t file_size = ftell(current_file);
        rewind(current_file);

        raw_size += file_size;

        fwrite(&file_size, sizeof(size_t), 1, archive);

        unsigned char buffer[UPLOAD_SIZE];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), current_file)) > 0)
        {
            fwrite(buffer, 1, bytes_read, archive);
        }

        fclose(current_file);
    }

    get_file_size(full_path, &archieve_size);
    print_size_info(raw_size, archieve_size);

    fclose(archive);
    free(filepath);
    return 1;
}

static int unpack(char * path,char * location)
{

    FILE * archieve_file_stream = fopen(path, "rb");

    char current_dir = get_current_filepath();


    size_t files_count = 0;

    fread(&files_count, sizeof(size_t), 1, archieve_file_stream);
    printf("%zu", files_count);

    for ( int i = 0; i < files_count; i++)
    {
        size_t filename_size;
        fread(&filename_size, sizeof(size_t), 1, archieve_file_stream);
        
        char * filename = malloc(filename_size+1);

        fread(filename,1, filename_size, archieve_file_stream);
        filename[filename_size] = '\0';
    
        size_t filesize = 0;

        char full_new_file_path[FILE_PATH_BUFFER];
        snprintf(full_new_file_path,FILE_PATH_BUFFER,"%s/%s", current_dir, filename);

        FILE * new_file_stream = fopen(full_new_file_path,"w");

        char temp_data_buffer[UPLOAD_SIZE];



        fclose(new_file_stream);

        free(filename);
    }

    fclose(archieve_file_stream);

    return 1;
}



int main(int argc, char * argv[])
{
    if (argc < 4)
    {
        printf("Error: insufficient argument type\n");
        return 1;
    }

    if (strcmp(argv[1], "pack") == 0)
    {
        size_t files_size = 0;
        File_data * files = NULL;

        char * pack_name = argv[2];
        if (!check_filename_validation(pack_name))
            return 1;
        

        if (!parse_files_data(&files,&files_size, argv, argc))
            return 1;
            
        printf_files(files,files_size);
        pack_files(files, files_size, pack_name);

        free_files(&files);
    }
    else if (strcmp(argv[1], "unpack") == 0)
    {
        unpack("arc.myarc", get_current_filepath());
    }
    return 0;
}
