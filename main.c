#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <ftw.h>

static int remove_entry(const char *path, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    return remove(path);
}

int delete_recursive(const char *path)
{
    return nftw(path, remove_entry, 64, FTW_DEPTH | FTW_PHYS);
}
#define NAME_F 255
#define MAX_ELEMENTS 100

struct Folder_t
{
    char name[NAME_F];
    unsigned char type;
};
void go_folder(const char *name, struct termios *original);

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
    char cwd[NAME_F];
    system("clear");
    printf("──────────────────────────────────────────────────────────────────\n");
    printf("📁  %s\n", getcwd(cwd, sizeof(cwd)) ? cwd : "?");
    printf("──────────────────────────────────────────────────────────────────\n");

    for (int i = 0; i < count; i++)
    {

        char *indicator = (entries[i].type == 4) ? " 📁" : " 📃";
        if (i == selected)
            printf("%s \033[7m > %s \033[0m\n", indicator, entries[i].name);
        else
            printf("%s   %s\n", indicator, entries[i].name);
    }
    printf("\n[UP/DOWN] navigate | [ENTER] open   | [q/Q] quit");
    printf("\n[r/R]     Rename   | [s/S]   Search | [D/d] delete \n");
}

void search_mode(char search_w[], DIR *folder, struct termios *original)
{
    printf("---> %s\n", search_w);
    struct dirent *direlm;
    folder = opendir(search_w);
    if (folder == NULL)
    {
        printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
        printf("Invalide path ! try again \n");
        printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
    }
    else
    {
        enable_raw_mode(original);
        go_folder(search_w, original);
    }
}

void rename_f(char path1[], char path2[], char current_p[], struct termios *original)
{
    if (path1[0] == ' ' || path2[0] == ' ')
    {
        printf("\nNo naming provided\n");
        return;
    }

    if (rename(path1, path2) == 0)
    {
        printf("\n──────────────────────────────────────────────────────────────────\n");
        printf("\nChanging %s ---> %s succesfuly .\n", path1, path2);
        printf("\n──────────────────────────────────────────────────────────────────\n");
        enable_raw_mode(original);
        return go_folder(current_p, original);
    }
    else
    {
        system("clear");
        printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
        printf("\nChanging %s ---> %s Faild .\n", path1, path2);
        printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
    }
}

void delete_f(char *d_path, char current_p[], struct termios *original)
{
    if (strcmp(d_path, " ") == 0)
    {
        printf("\nNo file is provided\n");
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
        system("clear");
        printf("\n──────────────────────────────────────────────────────────────────\n");
        printf("\n %s Deleted successfully.", d_path);
        printf("\n──────────────────────────────────────────────────────────────────\n");
        printf("Press any key to continue...");
        enable_raw_mode(original);
        return go_folder(current_p, original);
    }
    else
    {
        system("clear");
        printf("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
        printf("\n               FAILED to delete : %s", d_path);
        printf("\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n");
    }
}
void go_folder(const char *name, struct termios *original)
{
    DIR *folder;
    struct dirent *entry;

    if (!name || name[0] == '\0')
    {
        printf("You must provide a folder name!\n");
        return;
    }

    folder = opendir(name);
    if (folder == NULL)
        return;

    if (chdir(name) != 0)
    {
        printf("Could not switch to directory: %s\n", name);
        closedir(folder);
        return;
    }

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
        char buf[3];
        read(STDIN_FILENO, buf, 1);
        // quiting
        if (buf[0] == 'q' || buf[0] == 'Q')
        {
            disable_raw_mode(original);
            printf("\nUNTIL WE MEET AGAIN!\n");
            exit(0);
        }
        // selecting
        if (buf[0] == '\n' || buf[0] == '\r')
        {
            if (arr[selected].type == 4)
            {
                go_folder(arr[selected].name, original);
                selected = 0;
            }
            else
            {
                char cmd[NAME_F + 20];
                snprintf(cmd, sizeof(cmd), "xdg-open '%s'", arr[selected].name);
                system(cmd);
            }
        }
        // moving
        if (buf[0] == '\033')
        {
            read(STDIN_FILENO, &buf[1], 1);
            read(STDIN_FILENO, &buf[2], 1);
            if (buf[1] == '[')
            {
                switch (buf[2])
                {
                case 'A':
                    if (selected > 0)
                        selected--;
                    break;
                case 'B':
                    if (selected < count - 1)
                        selected++;
                    break;
                }
            }
        }
        print_menu(arr, count, selected);
        // search mode
        if (buf[0] == 's' || buf[0] == 'S')
        {
            disable_raw_mode(original);
            char cmds[1000];
            while (1)
            {
                printf("(PATH) : ");
                scanf("%s", cmds);

                if (strcmp(cmds, "q") == 0)
                {
                    enable_raw_mode(original);
                    system("clear");
                    print_menu(arr, count, selected);
                    break;
                }
                else
                {
                    printf("\n you searched : %s \n", cmds);
                    search_mode(cmds, folder, original);
                }
            }
        }

        // rename file
        if (buf[0] == 'r' || buf[0] == 'R')
        {
            disable_raw_mode(original);
            char oldname[NAME_F];
            char newName[NAME_F];
            while (1)
            {
                printf("\n\n──────────────────────────────────────────────────────────────────\n");
                printf("\n                    [:q] to quit Rename mode\n");
                printf("\n──────────────────────────────────────────────────────────────────\n");
                printf(" > Please enter the CORRECT PATH to the Directory/file you want to rename \n");
                printf("$ ");
                scanf("%255s", oldname);
                if (strcmp(oldname, ":q") == 0)
                {
                    enable_raw_mode(original);
                    system("clear");
                    print_menu(arr, count, selected);
                    break;
                }
                printf("\n");
                printf(" > Enter the new name (if it a file you need to provide the extension) , the new name should also be in a CORRECT PATH\n");
                printf("   Exemple : /home/usr/Folder ---> /home/usr/newNameFolder\n");
                printf("$ ");
                scanf("%255s", newName);
                printf("\n");
                if (strcmp(newName, ":q") == 0)
                {
                    enable_raw_mode(original);
                    system("clear");
                    print_menu(arr, count, selected);
                    break;
                }
                rename_f(oldname, newName, arr[selected].name, original);
            }
        }
        // Deletion mode
        if (buf[0] == 'd' || buf[0] == 'D')
        {
            disable_raw_mode(original);
            char d_path[NAME_F];
            while (1)
            {
                printf("\n\n──────────────────────────────────────────────────────────────────\n");
                printf("\n                    [q] to quit Deleting mode\n");
                printf("\n──────────────────────────────────────────────────────────────────\n");
                printf("\n >> Please enter the CORRECT PATH to the file/Directory you want to delete \n");
                printf("$ ");
                scanf("%255s", d_path);
                if (strcmp(d_path, "q") == 0)
                {
                    enable_raw_mode(original);
                    system("clear");
                    print_menu(arr, count, selected);
                    break;
                }
                else
                {
                    delete_f(d_path, arr[selected].name, original);
                }
            }
        }
    }
}

int main()
{
    struct termios original;
    enable_raw_mode(&original);
    go_folder(".", &original);
    disable_raw_mode(&original);
    return 0;
}