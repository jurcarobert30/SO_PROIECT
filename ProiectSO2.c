#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

void print_error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void print_usage() {
    fprintf(stderr, "Usage: ./program <director_intrare>\n");
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

void process_entry(char *base_path, char *entry_name, int output_fd) {
    char full_path[MAX_BUFFER_SIZE];
    struct stat entry_stat;

    snprintf(full_path, sizeof(full_path), "%s/%s", base_path, entry_name);

    if (lstat(full_path, &entry_stat) == -1) {
        perror("Eroare info fisier/director");
        return;
    }

    if (S_ISREG(entry_stat.st_mode)) {
        if (strstr(entry_name, ".bmp") != NULL) {
            // .bmp file
            char time_str[MAX_BUFFER_SIZE];
            struct tm *modification_time = localtime(&entry_stat.st_mtime);
            strftime(time_str, sizeof(time_str), "%d.%m.%Y", modification_time);

            char user_permissions[10];
            get_permissions(entry_stat.st_mode, user_permissions);

            dprintf(output_fd, "nume fisier: %s\n", entry_name);
            dprintf(output_fd, "inaltime: %ld\n", entry_stat.st_size);
            dprintf(output_fd, "lungime: %ld\n", entry_stat.st_size);
            dprintf(output_fd, "dimensiune: %ld\n", entry_stat.st_size);
            dprintf(output_fd, "identificatorul utilizatorului: %d\n", entry_stat.st_uid);
            dprintf(output_fd, "timpul ultimei modificari: %s\n", time_str);
            dprintf(output_fd, "contorul de legaturi: %ld\n", entry_stat.st_nlink);
            dprintf(output_fd, "drepturi de acces user: %s\n", user_permissions);
            dprintf(output_fd, "drepturi de acces grup: %s\n", user_permissions + 3);
            dprintf(output_fd, "drepturi de acces altii: %s\n", user_permissions + 6);
        } else {
            // non-.bmp file
            char user_permissions[10];
            get_permissions(entry_stat.st_mode, user_permissions);

            dprintf(output_fd, "nume fisier: %s\n", entry_name);
            dprintf(output_fd, "identificatorul utilizatorului: %d\n", entry_stat.st_uid);
            dprintf(output_fd, "drepturi de acces user: %s\n", user_permissions);
            dprintf(output_fd, "drepturi de acces grup: %s\n", user_permissions + 3);
            dprintf(output_fd, "drepturi de acces altii: %s\n", user_permissions + 6);
        }
    } else if (S_ISDIR(entry_stat.st_mode)) {
        // directory
        char user_permissions[10];
        get_permissions(entry_stat.st_mode, user_permissions);

        dprintf(output_fd, "nume director: %s\n", entry_name);
        dprintf(output_fd, "identificatorul utilizatorului: %d\n", entry_stat.st_uid);
        dprintf(output_fd, "drepturi de acces user: %s\n", user_permissions);
        dprintf(output_fd, "drepturi de acces grup: %s\n", user_permissions + 3);
        dprintf(output_fd, "drepturi de acces altii: %s\n", user_permissions + 6);
    } else if (S_ISLNK(entry_stat.st_mode)) {
        // symbolic link
        char user_permissions[10];
        get_permissions(entry_stat.st_mode, user_permissions);

        char link_target[MAX_BUFFER_SIZE];
        ssize_t link_size = readlink(full_path, link_target, sizeof(link_target));
        link_target[link_size] = '\0';

        dprintf(output_fd, "nume legatura: %s\n", entry_name);
        dprintf(output_fd, "dimensiune: %ld\n", entry_stat.st_size);
        dprintf(output_fd, "dimensiune fisier: %ld\n", link_size);
        dprintf(output_fd, "drepturi de acces user: %s\n", user_permissions);
        dprintf(output_fd, "drepturi de acces grup: %s\n", user_permissions + 3);
        dprintf(output_fd, "drepturi de acces altii: %s\n", user_permissions + 6);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_usage();
    }

    char *input_directory = argv[1];
    char output_file[] = "statistica.txt";

    int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (output_fd == -1) {
        print_error("Eroare dechidere fisier iesire");
    }

    DIR *dir = opendir(input_directory);
    if (dir == NULL) {
        print_error("Eroare deschidere director");
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            process_entry(input_directory, entry->d_name, output_fd);
        }
    }

    closedir(dir);
    close(output_fd);

    return 0;
}
