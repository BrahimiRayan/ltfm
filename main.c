#define _XOPEN_SOURCE 500
#define VERSION "1.0.0"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <ftw.h>
#include <sys/stat.h>

#define CLEAR_SCREEN() do { system("clear"); printf("\033[H\033[2J\033[3J"); fflush(stdout); } while(0)
#define NAME_F 255
#define MAX_ELEMENTS 150

struct Folder_t
{
    char name[NAME_F];
    unsigned char type;
};

void draw_start(){
    CLEAR_SCREEN();
    printf("                                    ██╗  ████████╗███████╗███╗   ███╗\n");
    printf("                                    ██║  ╚══██╔══╝██╔════╝████╗ ████║\n");
    printf("                                    ██║     ██║   █████╗  ██╔████╔██║\n");
    printf("                                    ██║     ██║   ██╔══╝  ██║╚██╔╝██║\n");
    printf("                                    ███████╗██║   ██║     ██║ ╚═╝ ██║\n");
    printf("                                    ╚══════╝╚═╝   ╚═╝     ╚═╝     ╚═╝\n");
    printf("\n                                       Linux Terminal File Manager\n");
    printf("                           ────────────────────────────────────────────────────\n");
    printf("                                         version %s\n\n", VERSION);
    printf("Press [ENTER] to start ..");
    getchar();
    CLEAR_SCREEN();
}

void draw_goodbye(){
    CLEAR_SCREEN();
    printf("                                    ██╗  ████████╗███████╗███╗   ███╗\n");
    printf("                                    ██║  ╚══██╔══╝██╔════╝████╗ ████║\n");
    printf("                                    ██║     ██║   █████╗  ██╔████╔██║\n");
    printf("                                    ██║     ██║   ██╔══╝  ██║╚██╔╝██║\n");
    printf("                                    ███████╗██║   ██║     ██║ ╚═╝ ██║\n");
    printf("                                    ╚══════╝╚═╝   ╚═╝     ╚═╝     ╚═╝\n");
    printf("\n                                       Linux Terminal File Manager\n");
    printf("                           ────────────────────────────────────────────────────\n");
    printf("                                            Until we meet again!\n\n");
}

void Answer (char message[] , int t){
    if(t == 0 ){
        CLEAR_SCREEN();
        printf("\n──────────────────────────────────────────────────────────────────\n#");
        printf("#\n                  %s \n#", message);
        printf("#\n──────────────────────────────────────────────────────────────────\n");
        sleep(1.6);
    }else if(t == -1) {
        CLEAR_SCREEN();
        printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n#");
        printf("#\n             %s \n#", message);
        printf("#\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
        sleep(2);
    }else if (t == 1){
        printf("\n──────────────────────────────────────────────────────────────────\n#");
        printf("#\n                  %s \n#", message);
        printf("#\n──────────────────────────────────────────────────────────────────\n");
    }
}

static int remove_entry(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    (void)sb; (void)typeflag; (void)ftwbuf;
    return remove(path);
}

int delete_recursive(const char *path)
{
    return nftw(path, remove_entry, 64, FTW_DEPTH | FTW_PHYS);
}

void enable_raw_mode(struct termios *original)
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, original);
    raw = *original;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode(struct termios *original)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, original);
}

void Ordering_elements(struct Folder_t tab[], int count)
{
    struct Folder_t tmp;
    for (int i = 0; i < count - 1; i++)
    {
        int min_idx = i;
        for (int j = i + 1; j < count; j++)
        {
            if (strcmp(tab[min_idx].name, tab[j].name) > 0)
                min_idx = j;
        }
        if (min_idx != i)
        {
            tmp = tab[i];
            tab[i] = tab[min_idx];
            tab[min_idx] = tmp;
        }
    }
}

void print_menu(struct Folder_t entries[], int count, int selected)
{
    char cwd[4096];
    int num_f = 0 ;
    CLEAR_SCREEN();
    printf("──────────────────────────────────────────────────────────────────\n");
    printf("📁  %s\n", getcwd(cwd, sizeof(cwd)) ? cwd : "?");
    printf("──────────────────────────────────────────────────────────────────\n");
    
    for (int i = 0; i < count; i++)
    {
        char *indicator = (entries[i].type == 4) ? " 📁" :" 📃";
        if(entries[i].type == 4) num_f++ ;
        if (i == selected)
            printf("%s \033[7m > %s \033[0m\n", indicator, entries[i].name);
        else
            printf("-%s %s \n", indicator, entries[i].name);
    }
    printf("\n(Folders) : %d / %d \n" , num_f , count);
    printf("\n(Files) : %d / %d \n" ,count - num_f , count);

    printf("\n[UP/DOWN] navigate | [ENTER] open   | [q/Q] quit");
    printf("\n[R/r]     Rename   | [S/s]   Search | [D/d] delete");
    printf("\n[C/c]     Create mode \n");
}

void search_mode(char search_w[], struct termios *original)
{
    DIR *test = opendir(search_w);
    if (test == NULL)
    {
        Answer("Invalid path! Try again.", -1);
        enable_raw_mode(original);
    }
    else
    {
        closedir(test);
        chdir(search_w);
        enable_raw_mode(original);
    }
}

void rename_f(char path1[], char path2[], struct termios *original)
{
    if (path1[0] == '\0' || path2[0] == '\0')
    {
        Answer("No name provided." , -1);
        enable_raw_mode(original);
        return;
    }

    if (rename(path1, path2) == 0)
    {
        Answer("The input renamed successfully.", 0);
        enable_raw_mode(original);
    }
    else
    {
        Answer("The input Cannot be renamed make ! No special characters are allowed!", -1);
        enable_raw_mode(original);
    }
}

void delete_f(char *d_path, struct termios *original)
{
    if (d_path[0] == '\0')
    {
        Answer("No file provided.", -1);
        enable_raw_mode(original);
        return;
    }

    struct stat st;
    int result;

    if (stat(d_path, &st) == 0 && S_ISDIR(st.st_mode))
        result = delete_recursive(d_path);
    else
        result = remove(d_path);

    if (result == 0)
    {
        Answer("INPUT deleted successfuly !", 0);
        enable_raw_mode(original);
    }
    else
    {
        Answer("INPUT failed to be deleted ! try again or check the PATH ", -1);
        enable_raw_mode(original);
    }
}

void create_file(struct termios *original)
{
    char name[NAME_F];
    Answer("File Creation mode" , 1);
    printf("<!> Please enter the name of the file + extension.\n");
    printf("\nExample: FileName.txt , main.c , index.html ...\n");
    printf("$ ");

    scanf("%254s", name);

    FILE *file = fopen(name, "w");
    if (file == NULL)
    {
        Answer("File failed to be created ! try again ." , -1);
        return;
    }

    Answer("File is created successfully." , 0);
    fclose(file);
    enable_raw_mode(original);
}

void create_folder(struct termios *original)
{
    char name[NAME_F];
    Answer("File Creation mode" , 1);
    printf("<!> Please enter the name of the folder.\n");
    printf("$ ");

    scanf("%254s", name);

    if (mkdir(name, 0777) == -1)
    {
         Answer("Folder failed to be created ! try again ." , -1);
        return;
    }

    Answer("Folder is created successfully." , 0);
    enable_raw_mode(original);
}

void go_folder(const char *start, struct termios *original)
{
    chdir(start);

top:;
    DIR *folder;
    struct dirent *entry;
    char current[4096];
    getcwd(current, sizeof(current));

    folder = opendir(".");
    if (folder == NULL)
        return;

    struct Folder_t arr[MAX_ELEMENTS];
    int count = 0;
    while ((entry = readdir(folder)) != NULL && count < MAX_ELEMENTS)
    {
        strcpy(arr[count].name, entry->d_name);
        arr[count].type = entry->d_type;
        count++;
    }
    closedir(folder);

    Ordering_elements(arr, count);

    int selected = 0;
    print_menu(arr, count, selected);

    while (1)
    {
        char buf[3] = {0};
        read(STDIN_FILENO, buf, 1);

        if (buf[0] == 'q' || buf[0] == 'Q')
        {
            disable_raw_mode(original);
            draw_goodbye();
            exit(0);
        }

        if (buf[0] == '\n' || buf[0] == '\r')
        {
            if (arr[selected].type == 4)
            {
                if (chdir(arr[selected].name) == 0)
                    goto top;
            }
            else
            {
                char cmd[NAME_F + 20];
                snprintf(cmd, sizeof(cmd), "xdg-open '%s' &", arr[selected].name);
                system(cmd);
            }
        }

        //navigation 
        if (buf[0] == '\033')
        {
            read(STDIN_FILENO, &buf[1], 1);
            read(STDIN_FILENO, &buf[2], 1);
            if (buf[1] == '[')
            {
                switch (buf[2])
                {
                case 'A':
                    if (selected > 0) selected--;
                    break;
                case 'B':
                    if (selected < count - 1) selected++;
                    break;
                }
            }
        }

        /* search mode */
        if (buf[0] == 's' || buf[0] == 'S')
        {
            disable_raw_mode(original);
            char cmds[1000];
            while (1)
            {
                Answer("[:q] to quit Search mode" , 1);
                printf("(Search): ");
                scanf("%999s", cmds);

                if (strcmp(cmds, ":q") == 0 || strcmp(cmds, ":Q") == 0)
                {
                    enable_raw_mode(original);
                    break;
                }
                else
                {
                    search_mode(cmds, original);
                    goto top;
                }
            }
        }

        /* rename mode */
        if (buf[0] == 'r' || buf[0] == 'R')
        {
            disable_raw_mode(original);
            char oldname[NAME_F];
            char newName[NAME_F];
            while (1)
            {
                Answer("[:q] to quit Rename mode" , 1);
                printf(" > Enter the path of the file/directory to rename:\n$ ");
                scanf("%254s", oldname);
                if (strcmp(oldname, ":q") == 0 || strcmp(oldname, ":Q") == 0)
                {
                    enable_raw_mode(original);
                    break;
                }
                printf("\n > Enter the new name/path:\n$ ");
                scanf("%254s", newName);
                if (strcmp(newName, ":q") == 0 || strcmp(newName, ":Q") == 0)
                {
                    enable_raw_mode(original);
                    break;
                }
                rename_f(oldname, newName, original);
                goto top;
            }
        }

        /* deletion mode */
        if (buf[0] == 'd' || buf[0] == 'D')
        {
            disable_raw_mode(original);
            char d_path[NAME_F];
            while (1)
            {
                Answer("[:q] to quit Delete mode" , 1);
                printf("\n >> Enter the path of the file/directory to delete:\n$ ");
                scanf("%254s", d_path);
                if (strcmp(d_path, ":q") == 0 || strcmp(d_path, ":Q") == 0)
                {
                    enable_raw_mode(original);
                    break;
                }
                delete_f(d_path, original);
                goto top;
            }
        }

        /* create mode */
        if (buf[0] == 'c' || buf[0] == 'C')
        {
            disable_raw_mode(original);
            char chose[3];
            while (1)
            {
                Answer("[:q] to quit Create mode" , 1);
                printf("\n >> What do you want to create?\n (F/f) File\n (D/d) Directory\n$ ");
                scanf("%2s", chose);

                if (strcmp(chose, "f") == 0 || strcmp(chose, "F") == 0)
                {
                    create_file(original);
                    goto top;
                }
                else if (strcmp(chose, "D") == 0 || strcmp(chose, "d") == 0)
                {
                    create_folder(original);
                    goto top;
                }
                else if (strcmp(chose, ":q") == 0 || strcmp(chose, ":Q") == 0)
                {
                    enable_raw_mode(original);
                    break;
                }
            }
        }

        print_menu(arr, count, selected);
    }
}

int main()
{
    draw_start();
    struct termios original;
    enable_raw_mode(&original);
    go_folder(".", &original);
    disable_raw_mode(&original);
    return 0;
}