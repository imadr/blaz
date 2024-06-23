import os
import subprocess
import time
from colorama import Fore, Back, Style
import shutil
from PIL import Image
from PIL import ImageChops

print(Fore.YELLOW + "building..." + Style.RESET_ALL)
subprocess.run(["cmake", "--build", "."], stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)

current_dir = os.getcwd()
DEBUG_OR_RELEASE = "Debug"
bin_dir = os.path.join(current_dir, "bin")
if not os.path.exists(bin_dir) or not os.path.isdir(bin_dir):
    print(Fore.RED + "bin directory not found.")
    exit(0)
tests_dir = os.path.join(bin_dir, "tests")

for subdir in os.listdir(bin_dir):
    if(subdir == "tests"):
        continue
    print(Fore.YELLOW + "running sample "+subdir + Style.RESET_ALL)
    subdir_path = os.path.join(bin_dir, subdir)
    if os.path.isdir(subdir_path):
        exe_dir = os.path.join(subdir_path, DEBUG_OR_RELEASE)
        if not os.path.exists(exe_dir) or not os.path.isdir(exe_dir):
            print(Fore.RED + f"bin directory not found in {subdir}. skipping...")
            continue

        exe_files = [file for file in os.listdir(exe_dir) if file.endswith(".exe")]
        if len(exe_files) != 1:
            print(Fore.RED + f"expected .exe file in directory {subdir}. skipping...")
            continue

        exe_path = os.path.join(exe_dir, exe_files[0])
        subprocess.Popen(exe_path, cwd=exe_dir, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
        time.sleep(1)
        subprocess.run(["taskkill", "/f", "/im", exe_files[0]], stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)

        bmp_files = [file for file in os.listdir(exe_dir) if file.endswith(".bmp")]
        if not os.path.exists(tests_dir):
            os.makedirs(tests_dir)
        for bmp_file in bmp_files:
            shutil.copy2(os.path.join(exe_dir, bmp_file), tests_dir)
            os.remove(os.path.join(exe_dir, bmp_file))

bmp_files = [file for file in os.listdir(os.path.join(os.path.join(current_dir, "bin"), "tests")) if file.endswith(".bmp")]

for bmp_file in bmp_files:
    generated_image_path = os.path.join(tests_dir, bmp_file)
    reference_image_path = os.path.join(os.path.join(os.path.join(current_dir, "samples"), "tests"), bmp_file)
    if(not os.path.isfile(reference_image_path)):
        print(Fore.RED + f"reference image {reference_image_path} does not exist" + Style.RESET_ALL)
        exit()

    generated_image = Image.open(generated_image_path)
    reference_image = Image.open(reference_image_path)
    if ImageChops.difference(generated_image, reference_image).getbbox() is None:
        print(Fore.GREEN + f"{bmp_file} passed" + Style.RESET_ALL)
    else:
        print(Fore.RED + f"{bmp_file} failed" + Style.RESET_ALL)
