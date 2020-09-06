#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

/*
Sample header:
"""
File:           autocomment.c
Author:         Nicholas Marthinuss
Date:           9/6/2020
Section:        13
E-mail:         nmarthi1@umbc.edu
Description:    This file automagically
                adds python header comments
                to my CS homework files.
"""
*/

//ANSI color constants
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m"

//file-related constants
#define DIR_NAME "test\0" //NOTE: This program should be one directory \
                            above the one it is modifying (im lazy)
#define MAX_PATH_LEN 512
#define MAX_LINE 512
#define MAX_DESC 512

//header data constants
#define AUTHOR "Nicholas Marthinuss"
#define SECTION "13"
#define EMAIL "nmarthi1@umbc.edu"
#define TAB4 "\t\t\t\t"

#define printerr(message) printf("%serror: %s%s", COLOR_RED, COLOR_RESET, message);
#define errclose printerr("could not close the file\n")
#define erropen printerr("something went wrong opening the filen\n")
#define tmpchk(len) dent->d_name[len - 3] == 't' && dent->d_name[len - 2] == 'm' && dent->d_name[len - 1] == 'p'

char *appendfname(char *path, char *filename);
FILE *maketmp(char *filename);
void readdesc(char *desc);

//uhhhh this program doesn't
//recover very gracefully from IO errors
//just... don't get IO errors :P

//ex: if it can't open a file for reading
//it just exits because that's the simplest thing
//I kinda do some cleanup

//I now notice that it doesn't clean up on exit,
//but it's 3am so that's a nope

int main(int argc, char **argv)
{
    DIR *dir;

    if ((dir = opendir(DIR_NAME)) == NULL)
    {
        printerr("could not open directory\n");
        return -1;
    }
    struct dirent *dent;

    int namelen;
    while ((dent = readdir(dir)) != NULL)
    {
        // printf("\n\nname[0]: %d, cmp: %d\n\n", dent->d_name[0], '.');
        if (dent->d_name[0] == '.')
        {
            continue;
        }
        //check if it's trying to open itself and don't do that
        if (!strcmp(dent->d_name, "autocomment\0"))
        {
            continue;
        }
        namelen = strlen(dent->d_name);
        // printf("namelen: %d\n", namelen);
        // printf("name[max - 2]: %c\nname[max - 1]: %c\nname[max - 0]: %c\n", dent->d_name[namelen - 3], dent->d_name[namelen - 2], dent->d_name[namelen - 1]);
        if (tmpchk(namelen))
        {
            //this is a tmp file and should be ignored
            continue;
        }
        // printf("name[0]: %c\nequals: %d\n", dent->d_name[0], dent->d_name[0] == '.');
        // printf("name: %s\n", dent->d_name);
        char *fname = malloc(256 * sizeof(char));
        sprintf(fname, "%s%c", dent->d_name, '\0');
        // strcat(fname, "\0");
        // printf("fname: %s\n", fname);
        // exit(-1);
        maketmp(fname);
    }
    //we need to re-traverse the directory
    //for .tmp files, so we just reopen the dir
    closedir(dir);
    if ((dir = opendir(DIR_NAME)) == NULL)
    {
        printerr("could not open directory to delete .tmp files\n");
        return -1;
    }

    char *fullpath = calloc(MAX_PATH_LEN, sizeof(char));

    while ((dent = readdir(dir)) != NULL)
    {
        namelen = strlen(dent->d_name);
        if (tmpchk(namelen))
        {
            strcpy(fullpath, DIR_NAME);
            fullpath = appendfname(fullpath, dent->d_name);
            //delete the tmp file
            if (remove(fullpath))
            {
                printerr("could not remove file ");
                printf("%s\n", fullpath);
            }
        }
    }

    closedir(dir);
    free(dent);
    dent = NULL;
    free(fullpath);
    fullpath = NULL;
    return 0;
}

//appends filename to path
char *appendfname(char *path, char *filename)
{
    if (strlen(path) > 256)
    {
        //only allow 256 for path
        //since other 256 chars are for file name
        printerr("path too long\n");
        exit(-1);
    }
    int len = strlen(DIR_NAME);
    // printf("len of filename: %ld\n", strlen(filename));
    // while (*filename != '\0')
    // {
    //     printf("non-null char: %c\n", *filename);
    //     filename++;
    // }
    // printf("sEgFaUlT\n");
    // exit(-1);
    int c;
    int i;
    strcat(path, "/");
    // strcpy(filename, "file1.txt");
    filename[strlen(filename)] = '\0'; //I don't know why I need this but I do
    for (i = 1; (c = *filename) != '\0'; i++)
    {
        // printf("filename: %s\n", filename);
        // exit(-1);
        // printf("char: %c\n", c);
        filename++;
        // if (i > 13)
        // {
        //     printf("filename: %s\npath: %s\n", filename, path);
        //     exit(-1);
        // }
        // printf("appending %c at pos %d\n", c, len + i);
        path[len + i] = c;
    }
    path[len + i] = '\0';
    // printf("len + i: %d\n", len + i);
    // printf("path: %s\n", path);
    return path;
}

//makes and writes to the tmp file
//then writes the header and appends
//the original file from tmp
FILE *maketmp(char *filename)
{
    FILE *current;
    FILE *tmp;

    char *currentfname = calloc(MAX_PATH_LEN, sizeof(char));
    char *tmpfname = calloc(MAX_PATH_LEN + 5, sizeof(char));

    strcpy(currentfname, DIR_NAME);
    strcpy(tmpfname, DIR_NAME);

    // printf("filename: %s\nlen: %ld\n", filename, strlen(filename));
    // exit(1);
    currentfname = appendfname(currentfname, filename);
    strcpy(tmpfname, currentfname);
    strcat(tmpfname, ".tmp\0");

    if (!(current = fopen(currentfname, "r")))
    {
        printerr("could not read file\n");
        goto cleanup;
    }
    else
    {
        if (!(tmp = fopen(tmpfname, "w")))
        {
            printerr("could not create tmpfile\n");
            goto cleanup;
        }
        else
        {
            //finally we can do things
            char *line = calloc(MAX_LINE, sizeof(char));
            while ((line = fgets(line, MAX_LINE, current)) != NULL)
            {
                fputs(line, tmp);
            }
            //at this point (hopefully) the original file has been copied to the .tmp file
            if (fclose(current))
            {
                errclose goto cleanup;
            }
            else
            {
                //open the file for writing...
                //we are now writing the header
                if (!(current = fopen(currentfname, "w")))
                {
                    erropen goto cleanup;
                }
                else
                {
                    //need some kind of input and a way to truncate it
                    //45 char line limit:
                    //TODO: write chars until chars>45 then wait for space and write newline instead
                    //for now we will just let user handle newlines
                    //(4 tabs)
                    // desc = readline();
                    //get the date
                    time_t t = time(NULL);
                    struct tm time = *localtime(&t);
                    char *date = malloc(11 * sizeof(char)); //10 is max date chars + 1 for nullptr
                    //format date output to *date
                    sprintf(date, "%d/%d/%d%c", time.tm_mon + 1, time.tm_mday, time.tm_year + 1900, '\0');

                    printf("enter description for file %s%s%s: ", COLOR_GREEN, filename, COLOR_RESET);
                    char *desc = malloc(MAX_DESC * sizeof(char));
                    //note that I have to insert 4 TABS after every newline
                    readdesc(desc);
                    fprintf(current, "\"\"\"\nFile:%s%s\nAuthor:%s%s\nDate:%s%s\nSection%s%s\nEmail:%s%s\nDescription:%s%s\"\"\"\n\n",
                            TAB4, filename, TAB4, AUTHOR, TAB4, date, TAB4, SECTION, TAB4, EMAIL, "\t\t\t", desc);
                    //I only need 3 tabs for desc but I'm not gonna define something else for just ^^^^^^
                    //at this point, the original file has the header but the rest is still in tmp

                    if (fclose(tmp))
                    {
                        errclose goto cleanup;
                    }
                    else
                    {
                        if (!(tmp = fopen(tmpfname, "r")))
                        {
                            erropen goto cleanup;
                        }
                        else
                        {
                            //last step...
                            line = calloc(MAX_LINE, sizeof(char));
                            while ((line = fgets(line, MAX_LINE, tmp)) != NULL)
                            {
                                fputs(line, current);
                            }
                            if (fclose(current))
                            {
                                errclose goto cleanup;
                            }
                            else
                            {
                                if (fclose(tmp))
                                {
                                    errclose goto cleanup;
                                }
                                else
                                {
                                cleanup:
                                    free(line);
                                    line = NULL;
                                    free(tmpfname);
                                    tmpfname = NULL;
                                    free(currentfname);
                                    currentfname = NULL;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

//reads a description line
//adds 4 tabs after newline
void readdesc(char *desc)
{
    int c;
    int lastchar = 0;
    int i = 0;
    while ((c = getchar()) != '\0' && c != EOF && i++ < MAX_DESC - 1)
    {
        *desc = c;
        desc++;
        if (c != '\n')
        {
            // printf("added char %c\n", c);
        }
        if (lastchar == '\n' && c == '\n')
        {
            break;
        }
        lastchar = c;
        if (c == '\n')
        {
            i += 16; //4 tabs * 4 spaces per tab
            for (int j = 0; j < 4; j++)
            {
                *desc = '\t';
                // printf("added tab\n");
                desc++;
            }
        }
    }
    *desc = '\n';
    desc++;
    *desc = '\0';
}