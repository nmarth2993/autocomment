import os
import re
import datetime
import sys
from pathlib import Path

CURR_DIR = '.'
CPP_EXT = '.cpp'
AUTOCOMMENT_EXT = '.actmp'

ANNOT = '@desc:'
HEADER_START = '/' + '*' * 41
HEADER_STOP = '*' * 41 + '/'
DESC_ANNOT_REGEX = r'(\s*)(\/\/)(\s*)(@desc:)'

KEY_AUTHOR = 'author'
KEY_SECTION = 'section'
KEY_EMAIL = 'email'

TABS = '\t\t'

CONST_FILE_PATH = str(Path.home()) + "/.autocomment/constants"

NEWLINE_PAD = "\n** "

COLOR_MAGENTA = "\u001b[35m"
COLOR_RESET = "\u001b[0m"

# the number of characters before looking
# for a space at which to fold
N_FOLD = 50


def fold(line: str) -> str:
    """
    PRE: line contains the substring '@desc'

    This method takes in a string containing a description annotation
    and returns a string with newlines inserted on or after n characters
    have been read, where n is the target number of characters in a line

    :param line: the description line given
    :return: string containing newlines at appropriate locations
    """
    start = line.index(ANNOT)
    line = line[start + len(ANNOT) + 1:]  # move past space
    folded = []
    n = 0
    for i in range(len(line)):
        if n > N_FOLD and line[i] == ' ':
            folded.append(NEWLINE_PAD)
            n = 0

        else:
            folded.append(line[i])

        n += 1

    return ''.join(folded)


def read_constants(constants: dict):
    """
    This method reads constants from the constand file
    into the given dictionary

    :param constants: the dict to read into
    """

    # key are in order author, section, email
    keys = [KEY_AUTHOR, KEY_SECTION, KEY_EMAIL]

    with open(CONST_FILE_PATH, 'r') as constfile:
        for key in keys:
            constants[key] = constfile.readline()[:-1]


if __name__ == "__main__":
    project_name = ''
    match = None
    # if user gives a project name, we can process it and can skip
    # asking them for input later
    if (len(sys.argv) > 1):
        project_name = sys.argv[1]
        match = re.search(r"[0-9]", project_name)
        if match:
            m_index = match.start()
            # put space between letters and digit
            project_name = project_name[:m_index] + ' ' + project_name[m_index]
            project_name = list(project_name)
            # make first letter uppercase
            project_name[0] = project_name[0].upper()
            project_name = ''.join(project_name)


    for dirname, subdirs, files in os.walk(CURR_DIR):
        constants = {}
        read_constants(constants)

        # if we couldn't parse a name, fall back to user input
        if not match:
            project_name = input("Enter the name of the project (e.g. Project 1): ")

        for filename in files:
            skip = 0
            if filename.endswith(CPP_EXT):

                print(f"found cpp file: {filename}")
                desc = ''
                with open(filename, 'r') as sourcefile, open(filename[:-4] + AUTOCOMMENT_EXT, 'w') as tempfile:
                    # first, find if there are any comment annotations
                    # comment annotations are of the form // @desc: Description goes here
                    # with line folding every 40 characters
                    first_line = sourcefile.readline()
                    result = re.match(DESC_ANNOT_REGEX, first_line)
                    if result:
                        # we have a match
                        desc = fold(first_line)
                    else:
                        # no desc annotation found in
                        # print(
                        #    f"no desc annotation found in {filename}. skipping...")
                        skip = 1
                        print(COLOR_MAGENTA + "warn:" + COLOR_RESET + " skipped " + filename + " (no desc annotation)")

                    tempfile.writelines(sourcefile.readlines())

                with open(filename[:-4] + AUTOCOMMENT_EXT, 'r') as tempfile, open(filename, 'w') as sourcefile:

                    if not skip:
                        """
                        File: filename
                        Project: CMSC 202 USER ENTERS PROJECT STRING
                        Author: ~/.autocomment/constants
                        Section: ~/.autocomment/constants
                        E-Mail: ~/.autocomment/constants
                        **empty line
                        description
                        **empty line
                        """
                        sourcefile.write(HEADER_START + '\n')

                        sourcefile.write("** File:" + TABS +
                                         f"{filename}" + '\n')
                        sourcefile.write(
                            "** Project:" + TABS + f"CMSC 202 {project_name}, Spring 2021" + '\n')
                        sourcefile.write(
                            "** Author:" + TABS + f"{constants[KEY_AUTHOR]}" + '\n')
                        today = datetime.date.today()
                        sourcefile.write(
                            "** Date:" + TABS + f"{today.month}/{today.day}/{str(today.year)[2:]}" + '\n')
                        sourcefile.write(
                            "** Section:" + TABS + f"{constants[KEY_SECTION]}" + '\n')
                        sourcefile.write(
                            "** Email:" + TABS + f"{constants[KEY_EMAIL]}" + '\n')

                        sourcefile.write("**" + '\n')
                        sourcefile.write("** " + desc)

                        # two empty lines trail
                        sourcefile.write("**" + '\n')
                        sourcefile.write("**" + '\n')

                        sourcefile.write(HEADER_STOP + '\n')

                    else:
                        # if we didn't find a description in a comment, we need to put
                        # the first line back again
                        sourcefile.write(first_line)

                    # now just copy everything over
                    sourcefile.writelines(tempfile.readlines())

                # delete the temp file
                os.remove(filename[:-4] + AUTOCOMMENT_EXT)
