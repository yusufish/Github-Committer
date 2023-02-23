#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <dirent.h>
#define FILESIZE 100
#define BUFSIZE 90
#define init(x) x[0] = '\0'
char* list[2];

const char* ext_control (const char* filename) {
    
    const char* dot = strrchr(filename, '.');
    if (dot == NULL || dot == filename) return NULL;
    
    return dot+1;
}

int main() {

    printf("****************************************************************************************************************\n");
    printf("* This tool makes easier committing many files belongs to cloned repo at once with a defined template.         *\n");
    printf("* To use this tool efficiently, you should have token to access API to push and other user info.               *\n");
    printf("* To use token, it is sufficient that 405768455.txt file includes token is in a hidden folder named 14045294.  *\n");
    printf("* 405768455 is hashed form of 'token' and 14045294 is hashed form of 'info' with radix 31.                     *\n");
    printf("* If there is no such a folder in the current directory, you will be asked to enter the token for once.        *\n");
    printf("****************************************************************************************************************\n");

    DIR* dir;
    struct dirent* entry;
    struct stat statbuf;
    if ((dir = opendir(".")) == NULL) {
        printf("Folder could not been opened\n");
        return 0;
    }
    chdir(".");

    const char* paths[FILESIZE];
    int counter = 0;

    while ((entry = readdir(dir)) != NULL) {
        lstat(entry->d_name, &statbuf);
        if (S_ISDIR(statbuf.st_mode)) continue;
        if (ext_control(entry->d_name) == NULL) continue;
        paths[counter] = entry->d_name;
        ++counter;
    }

    char* home;
    if ((home = getenv("HOME")) == NULL)
        home = getpwuid(getuid())->pw_dir;

    char git_path[BUFSIZE];
    init(git_path);
    strcat(git_path, home);
    strcat(git_path, "/.gitconfig");

    FILE* fp = fopen(git_path,"r");
    if (fp == NULL) {
        char username[BUFSIZE], usermail[BUFSIZE], cmd1[BUFSIZE], cmd2[BUFSIZE];
        init(username);
        init(usermail);
        init(cmd1);
        init(cmd2);
        printf("It seems that you haven't entered your user information.\n");
        printf("Please enter your username: ");
        scanf("%s", username);
        strcat(cmd1, "git config --global user.name \"");
        strcat(cmd1, username);
        strcat(cmd1, "\"");
        printf("Please enter your email: ");
        scanf("%s", usermail);
        strcat(cmd2, "git config --global user.email \"");
        strcat(cmd2, usermail);
        strcat(cmd2, "\"");
        system(cmd1);
        system(cmd2);
    }

    else {
        char line[BUFSIZE];
        int cnt = 0;
        while(fgets(line, 90, fp)) {
            if (cnt == 0) {
                ++cnt;
                continue;
            }
            int first;
            if (cnt == 1) first = 8;
            else first = 9; 
            int s = strlen(line);
            for (int i = first; i <= s; ++i)
                line[i-first] = line[i];
            line[strlen(line)-1] = '\0';
            list[cnt-1] = strdup(line);
            ++cnt;
        }
    }


    if (chdir("../.14045294") == -1) {
        printf("There is no token information so a hidden folder will be created includes token info.\n");
        chdir("..");
        system("mkdir .14045294");
        chdir("./.14045294");
        system("touch 405768455.txt");
        char token[BUFSIZE];
        init(token);
        printf("Please enter your Github token: ");
        scanf("%s", token);
        char cmd[BUFSIZE];
        init(cmd);
        strcat(cmd, "echo \"");
        strcat(cmd, token);
        strcat(cmd, "\" > ");
        strcat(cmd, "405768455.txt");
        system(cmd);
    }

    else {
        char token[BUFSIZE];
        init(token);
        FILE* fp;
        if((fp = fopen("405768455.txt", "r")) == NULL) {
            printf("Could not opened\n");
            exit(0);
        }
        else
            fgets(token, BUFSIZE, fp);
        if ((token[strlen(token)-1]) == '\n')
            token[strlen(token)-1] = '\0';
        fclose(fp);

        chdir("../Github-Committer");
        for (int i = 0; i < counter; ++i) {
            char cmd[BUFSIZE];
            init(cmd);
            strcat(cmd, "git add ");
            strcat(cmd, paths[i]);
            system(cmd);
            init(cmd);
            strcat(cmd, "git commit -m \"");
            strcat(cmd, paths[i]);
            strcat(cmd, " uploaded via github_committer\"");
            system(cmd);
            init(cmd);
            strcat(cmd, "./dedede.exp ");
            strcat(cmd, list[0]);
            strcat(cmd, " ");
            strcat(cmd, token);
            system(cmd);
        }
    }

    return EXIT_SUCCESS;
}
