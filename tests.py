from glob import glob
import subprocess
from colorama import init as colorama_init
from colorama import Fore
from colorama import Style
from typing import List

colorama_init()

TEST_FOLDER_NAME = "examples"
EXTENSION = "tbd"
COMPILER_PATH = "build/tbd"

subprocess.run(["make"])

test_files: List[str] = []

for f in glob(f"{TEST_FOLDER_NAME}/*.{EXTENSION}"):
    test_files.append(f)

print(f"{Fore.CYAN}FOUND {len(test_files)} {"test" if len(test_files) == 1 else "tests"} in ./{TEST_FOLDER_NAME}{Style.RESET_ALL}\n\n\n")


i = 1
for test in test_files:
    print(f"{Fore.WHITE}Running Test [{Fore.GREEN}{i}/{len(test_files)}{Style.RESET_ALL}] : {test}\n\n\n")
    subprocess.run([COMPILER_PATH, test])
    # subprocess.run(["as", "".join(test.split(".")[:-1]) + ".S", "-o", "".join(test.split(".")[:-1]) + ".out"])
    # subprocess.run(["ld", "".join(test.split(".")[:-1]) + ".out", "-o", "".join(test.split(".")[:-1]) + ".exe"])
    # """as examples/out/test.S -o examples/out/test.o && ld examples/out/test.o -o examples/out/test.exe"""
    print("\n\n\n")
    i += 1
    