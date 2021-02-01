import re
import os
import sys
from pathlib import Path

HEADER_REGEX = r"\/\*(\**)\n\*\* File:\t\t.*\n\*\* Project:\t\tCMSC 202.*\n\*\* Author:\t\t.*\n\*\* Date:\t\t.*\n\*\* Section:\t\t.*\n\*\* Email:\t\t.*\n\*\*\n"

HOMEPATH = str(Path.home())
SUBMIT_PATH = HOMEPATH + "/CMSC/cmsc202/submissions/"

CPP_EXT = '.cpp'
CURR_DIR = '.'

ANSI_UNDERLINE = "\u001b[4m"
COLOR_MAGENTA = "\u001b[35m"
COLOR_RESET = "\u001b[0m"

MAKEFILE = 'makefile'

HEADER_LINES = 8

if __name__ == "__main__":
    if (len(sys.argv) > 1):
        # use arg as assignment name if given
        folder = SUBMIT_PATH + sys.argv[1] + '/'

    else:
        folder = SUBMIT_PATH + input("enter assignment folder name: ") + '/'

    source_files = []
    for _, _, files, in os.walk(CURR_DIR):
        for filename in files:
            # include makefile
            if filename.lower() == MAKEFILE:
                source_files.append(filename)
            # if we find a source file, check for a header comment
            if filename.endswith(CPP_EXT):

                source_files.append(filename)

                with open(filename) as source_file:
                    lines = ''
                    for _ in range(HEADER_LINES):
                        lines += source_file.readline()

                    # check for header comment
                    match = re.match(HEADER_REGEX, lines)

                    if not match:
                        print(COLOR_MAGENTA + "warn: " + COLOR_RESET +
                              "skipping un-commented file " + ANSI_UNDERLINE + filename + COLOR_RESET)
                        # remove source file from copy list
                        source_files.pop()

            # now try to copy all source files over
            for file in source_files:
                result = os.system("cp " + file + " " + folder)
                if result:
                    print("copy failed. did you give the correct assignment name?")
