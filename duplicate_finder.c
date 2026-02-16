#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#define BUFFER_SIZE 4096
//PROGRAM FINDS ALL FILE WITH SAME SIZE
// ./duplicate_finder /home/Desktop - path

int is_obj_a_file(const char * file_path, struct stat st, struct dirent * de)
{
    if (stat(file_path,&st) == 0)
    {
        if (S_ISREG(st.st_mode))
            return 1;
    }
    return 0;
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Insufficient numbers of parameters\n");
        return 1;
    }

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
            printf("%s\n", de->d_name);
        }

    }


    closedir(dir);
    return 0;
}