#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>

/*
Sample header:
"""
File:           autocomment.c
Author:         Nicholas Marthinuss
Date:           9/6/2020
Section:        Section
E-mail:         Email
Description:    This file automagically
                adds python header comments
                to my CS homework files.
"""
*/

#ifdef __linux__
#define QUIT 0
#else
#define QUIT 1
#endif

//ANSI color constants
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_RESET "\x1b[0m"

//file-related constants
#define DIR_NAME dirpath //NOTE: This program should be one directory \
                            above the one it is modifying (im lazy)
#define CDIR_NAME ".autocomment"
#define DATAFILE "constants"
#define MAX_PATH_LEN 512
#define MAX_LINE 512
#define MAX_DESC 512

//header data constants
#define AUTHOR author
#define SECTION section
#define EMAIL email
#define TAB3 "\t\t\t"
#define NUM_TABS 3

#define LINE_FOLD 30

#define printerr(message) printf("%serror: %s%s", COLOR_RED, COLOR_RESET, message);
#define errclose printerr("could not close the file\n")
#define erropen printerr("something went wrong opening the filen\n")
#define tmpchk(len) dent->d_name[len - 3] == 't' && dent->d_name[len - 2] == 'm' && dent->d_name[len - 1] == 'p'

char *appendfname(char *path, char *filename);
FILE *maketmp(char *filename);
void readdesc(char *desc);
void readline(char *input);
int extractdesc(char *line, char *desc);
void linefold(char *desc, char *folded);

//uhhhh this program doesn't
//recover very gracefully from IO errors
//just... don't get IO errors :P

//ex: if it can't open a file for reading
//it just exits because that's the simplest thing
//I kinda do some cleanup

//I now notice that it doesn't clean up on exit,
//but it's 3am so that's a nope

char *dirpath;

char *author;
char *section;
char *email;
char *desc;

int descfound;

int main(int argc, char **argv)
{
    //quick OS check
    if (QUIT)
    {
        printerr("this is a program only intended to be run on linux machines\n");
        return -1;
    }

    //need a directory to search
    if (argc < 2)
    {
        printerr("")
            printf("no directory supplied, try --help\n");
        return -1;
    }

    if (!strcmp(*++argv, "--help"))
    {
        //print help and exit
        printf("usage: autocomment [dir | --help]\n");
        printf("\t--help: prints this help message\n");
        printf("\t[dir]: name of directory that holds files to add descriptions\n");
        printf("a description comment can be included as the first line with the format:\n");
        printf("\t# @desc: This is the description\n");
        printf("you will be prompted for a description for each file without such a comment\n");
        printf("an empty line signals end of description entry\n");
        printf("\nthis program will check for constants in %s~/autocomment/constants%s\n", COLOR_GREEN, COLOR_RESET);
        printf("if it cannot find the file, it will ask you to enter each constant field and store it in the constants file\n");
        printf("to be used on all subsequent invocations\n");
        return 0;
    }

    author = malloc(MAX_LINE * sizeof(char));
    section = malloc(MAX_LINE * sizeof(char));
    email = malloc(MAX_LINE * sizeof(char));
    desc = malloc(MAX_DESC * sizeof(char));
    descfound = 0;

    char *home = getenv("HOME");
    char *datafile = malloc(128 * sizeof(char));
    char *datapath = malloc(128 * sizeof(char));

    strcat(home, "/");

    strcpy(datapath, home);
    strcat(datapath, CDIR_NAME);
    strcat(datapath, "/");

    strcpy(datafile, datapath);
    strcat(datafile, DATAFILE);

    // printf("home: %s\nfile: %s\npath: %s\n", home, datafile, datapath);
    // return 0;

    FILE *constantsfile;
    if ((constantsfile = fopen(datafile, "r")) == NULL)
    {
        mkdir(datapath, 0700); // 700 oct -> user has rwx
        constantsfile = fopen(datafile, "w");
        if ((constantsfile = fopen(datafile, "w")) == NULL)
        {
            printerr("could not create constants file\n");
            return -1;
        }
        else
        {
            printf("No data file found. Creating one...\n");
            //grab user input
            setbuf(stdin, NULL);
            char *input = malloc(MAX_LINE * sizeof(char));
            printf("Enter your full name: ");
            readline(input);
            fprintf(constantsfile, "%s", input);
            printf("\nEnter your section: ");
            readline(input);
            fprintf(constantsfile, "%s", input);
            printf("\nEnter your email: ");
            readline(input);
            fprintf(constantsfile, "%s", input);
            if (fclose(constantsfile))
            {
                errclose return -1;
            }
            constantsfile = NULL;
        }
    }
    //if we just closed the file for writing, open again to read
    if (constantsfile == NULL)
    {
        if ((constantsfile = fopen(datafile, "r")) == NULL)
        {
            erropen return -1;
        }
    }
    //read constants file
    char *line = malloc(MAX_LINE * sizeof(char));

    // line = fgets(line, MAX_LINE, constantsfile);
    // printf("line: %s", line);
    // if (!(author = fgets(author, MAX_LINE, constantsfile)))
    // {
    //     printerr("null\n");
    //     return -1;
    // }
    // else
    // {
    //     printf("author: %s\nline: %s\n", author, line);
    // }

    // printf("line: %s", line);
    // line = fgets(line, MAX_LINE, constantsfile);
    // printf("line: %s", line);

    // printf("end of test section\n");
    // return 0;
    if (!(author = fgets(author, MAX_LINE, constantsfile)))
    {
        printerr("malformed constants file\n");
        return -1;
    }
    if (!(section = fgets(section, MAX_LINE, constantsfile)))
    {
        printerr("malformed constants file\n");
        return -1;
    }
    if (!(email = fgets(email, MAX_LINE, constantsfile)))
    {

        printerr("malformed constants file\n");
        return -1;
    }
    // printf("author: %ssection: %semail: %s", author, section, email); //newline at end of strings

    dirpath = calloc(256, sizeof(char));
    strcpy(dirpath, *argv); //copy second arg to use as global path var

    DIR *dir;

    //try to open directory
    if ((dir = opendir(DIR_NAME)) == NULL)
    {
        printerr("could not open directory\n");
        return -1;
    }
    struct dirent *dent;

    int namelen;

    //read the name of each file
    while ((dent = readdir(dir)) != NULL)
    {
        // printf("\n\nname[0]: %d, cmp: %d\n\n", dent->d_name[0], '.');

        //ignore . and ..
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

        //i don't know
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
        erropen goto cleanup;
    }
    else
    {
        if (!(tmp = fopen(tmpfname, "w")))
        {
            erropen goto cleanup;
        }
        else
        {
            //finally we can do things

            //first line may hold a description, save it to check
            char *firstline = calloc(MAX_LINE, sizeof(char));
            char *prefold = calloc(MAX_LINE, sizeof(char));
            char *line = calloc(MAX_LINE, sizeof(char));

            //check if first line has description
            fgets(firstline, MAX_LINE, current);
            //if it does, save it, else call fputs

            if (extractdesc(firstline, prefold) == 1)
            {
                //description found
                descfound = 1;
                linefold(prefold, desc);
            }
            else
            {
                descfound = 0;
                fputs(firstline, tmp);
            }

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

                    //maybe print head of file before asking for desc?
                    // char *head = malloc((MAX_PATH_LEN + 5) * sizeof(char)); //+5 chars for "head "
                    // strcpy(head, "head ");
                    // strcat(head, currentfname);

                    //if no desc found, enter one
                    if (!descfound)
                    {
                        // desc = malloc(MAX_DESC * sizeof(char));
                        printf("no description comment found.\n");
                        printf("enter description for file %s%s%s: ", COLOR_GREEN, filename, COLOR_RESET);
                        readdesc(desc);
                    }
                    //author, section, and email (constants) have newlines in their strings,
                    //no need to include them in the format
                    fprintf(current, "\"\"\"\nFile:%s%s\nAuthor:%s%sDate:%s%s\nSection%s%sEmail:%s%sDescription:%s%s\"\"\"\n\n",
                            TAB3, filename, TAB3, AUTHOR, TAB3, date, TAB3, SECTION, TAB3, EMAIL, "\t\t", desc);
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
            for (int j = 0; j < NUM_TABS; j++)
            {
                *desc++ = '\t';
            }
        }
    }
    *desc = '\n';
    desc++;
    *desc = '\0';
}

void readline(char *input)
{
    int c;
    int i = 0;
    while ((c = getchar()) != EOF && c != '\n' && i++ - 2 < MAX_LINE)
    {
        *input++ = c;
    }
    *input++ = '\n';
    *input = '\0';
}

//checks line for description comment
//stores description in desc
//returns 1 if description found, else 0
int extractdesc(char *line, char *desc)
{
    //ignore whitespace until # maybe?
    //sample lines:
    //# @desc: This is a desc
    //#@desc: This is a desc

    //skip leading whitespace
    while (*line == '\t' || *line == ' ')
    {
        line++;
    }

    // printf("line[0]: %c\n", *line);
    if (*line != '#')
    {
        return 0;
    }
    //move pointer up one since we have
    //already read the '#'
    line++;

    while (*line == ' ')
    {
        line++;
    }

    //if comment line does not have desc tag
    if (strncmp(line, "@desc:", 6))
    {
        return 0;
    }
    line += 6; //move past next 6 chars (skip annotation)
    while (*line == ' ')
    {
        line++;
    }

    int i = 0;
    while (*line != '\n' && i < MAX_LINE - 1)
    {
        i++;
        *desc = *line;
        desc++;
        line++;
    }
    *desc = '\0';
    return 1;
}

//adds newlines when space is read
//and n > LINE_FOLD
//even folding perhaps???
void linefold(char *desc, char *folded)
{
    int n = 1;
    while (*desc != '\n' && *desc != '\0' && n < MAX_DESC)
    {
        if (*desc == ' ' && n > LINE_FOLD)
        {
            *folded = '\n';
            for (int i = 0; i < 3; i++)
            {
                folded++;
                *folded = '\t';
            }
            n = 1;
        }
        else
        {
            *folded = *desc;
        }
        n++;
        folded++;
        desc++;
    }
    *folded = '\n';
    folded++;
    *folded = '\0';
}
