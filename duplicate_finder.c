#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <openssl/sha.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096
#define FILE_TEXT_BUFFER 16384

#define HASH_SIZE 16
//PROGRAM FINDS ALL FILE WITH SAME SIZE
// ./duplicate_finder /home/Desktop - path


typedef struct{
    unsigned char hash[HASH_SIZE];
    char filepath[BUFFER_SIZE];
    long long file_size;
}File_info;

static int is_obj_a_file(const char * file_path, struct stat st, struct dirent * de)
{
    if (stat(file_path,&st) == 0)
    {
        if (S_ISREG(st.st_mode))
            return 1;
    }
    return 0;
}

static int calculate_file_hash(const char * file_path, unsigned char * hash)
{
    FILE * f = fopen(file_path, "r");

    if (!f)
    {
        perror("fopen");
        return 0;
    }
    
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    
    unsigned char buffer[BUFFER_SIZE];
    size_t byte_read;

    while((byte_read = fread(buffer,1,BUFFER_SIZE,f)) != 0)
    {
        SHA256_Update(&sha256, buffer, byte_read);
    }

    SHA256_Final(hash, &sha256);
    
    fclose(f);
    return 1;
}

static void increase_files(int * size, int * capacity, File_info ** files)
{
    if (*size >= *capacity)
    {
        *capacity *= 2;
        File_info * tmp = realloc(*files, sizeof(File_info) * (*capacity));
        if (!tmp)
        {
            perror("realloc error\n");
            return;
        }

        *files=tmp;
    
    }
    
    *(size++);    

}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Insufficient numbers of parameters\n");
        return 1;
    }

    File_info * files = NULL;
    int files_capacity = 1;
    int files_size = 0;

    DIR * dir = opendir(argv[1]);
    struct dirent *de;
    struct stat st;

    if (dir == NULL)
    {
        printf("dir %s doesn't exists\n", argv[1]);
        return 1;
    }

    while((de = readdir(dir)) != NULL)
    {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;

        char file_path[BUFFER_SIZE];

        snprintf(file_path, BUFFER_SIZE, "%s/%s", argv[1], de->d_name);    

        if (is_obj_a_file(file_path,st,de))
        {
            increase_files(files_size,files_capacity,&files);
            File_info file;


            if (!calculate_file_hash(file_path, file.hash))
                return 0;

            strcpy(file.filepath, file_path);
            file.file_size = st.st_size;

            files[files_size-1] = file;

        }

    }


    closedir(dir);
    return 0;
}