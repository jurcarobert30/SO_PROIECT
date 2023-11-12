#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>  

#define MAX_BUFFER_SIZE 1024

void print_error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void print_usage() {
    fprintf(stderr, "Usage: ./program <fisier_intrare>\n");
    exit(EXIT_FAILURE);
}

void get_permissions(mode_t mode, char *permissions) {
    permissions[0] = (mode & S_IRUSR) ? 'R' : '-';
    permissions[1] = (mode & S_IWUSR) ? 'W' : '-';
    permissions[2] = (mode & S_IXUSR) ? 'X' : '-';
    permissions[3] = (mode & S_IRGRP) ? 'R' : '-';
    permissions[4] = (mode & S_IWGRP) ? 'W' : '-';
    permissions[5] = (mode & S_IXGRP) ? 'X' : '-';
    permissions[6] = (mode & S_IROTH) ? 'R' : '-';
    permissions[7] = (mode & S_IWOTH) ? 'W' : '-';
    permissions[8] = (mode & S_IXOTH) ? 'X' : '-';
    permissions[9] = '\0';
}

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        print_usage();
    }

    char *input_file = argv[1];
    char output_file[] = "statistica.txt";

    
    int input_fd = open(input_file, O_RDONLY);
    if (input_fd == -1) {
        print_error("Eroare deschidere fisier intrare");
    }

   
    struct stat file_stat;
    if (fstat(input_fd, &file_stat) == -1) {
        print_error("Eroare info fisier");
    }

    
    int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (output_fd == -1) {
        print_error("Eroare dechidere fisier iesire");
    }

    
    char time_str[MAX_BUFFER_SIZE];
    struct tm *modification_time = localtime(&file_stat.st_mtime);
    strftime(time_str, sizeof(time_str), "%d.%m.%Y", modification_time);


    char user_permissions[10];
    get_permissions(file_stat.st_mode, user_permissions);

   
    dprintf(output_fd, "nume fisier: %s\n", input_file);
    dprintf(output_fd, "inaltime: %ld\n", file_stat.st_size);  
    dprintf(output_fd, "lungime: %ld\n", file_stat.st_size);
    dprintf(output_fd, "dimensiune: %ld\n", file_stat.st_size);
    dprintf(output_fd, "identificatorul utilizatorului: %d\n", file_stat.st_uid);
    dprintf(output_fd, "timpul ultimei modificari: %s\n", time_str);
    dprintf(output_fd, "contorul de legaturi: %ld\n", file_stat.st_nlink);
    dprintf(output_fd, "drepturi de acces user: %s\n", user_permissions);
    dprintf(output_fd, "drepturi de acces grup: %s\n", user_permissions + 3);
    dprintf(output_fd, "drepturi de acces altii: %s\n", user_permissions + 6);

    
    close(input_fd);
    close(output_fd);

    return 0;
}
