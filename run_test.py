import os
import subprocess
import time
from colorama import Fore, Back, Style
import shutil
from PIL import Image
from PIL import ImageChops
from PIL import ImageGrab
import pywinctl
import numpy as np


def mse(image1, image2):
    img1 = np.array(image1)
    img2 = np.array(image2)
    if img1.shape != img2.shape:
        raise ValueError("Images must have the same dimensions for MSE calculation.")
    mse_value = np.mean((img1 - img2) ** 2)
    return mse_value

def screenshot(window_name, screenshot_path):
    window = pywinctl.getWindowsWithTitle(window_name)[0]
    rect = window.getClientFrame()
    if window:
        bbox = rect.left, rect.top, rect.right, rect.bottom
        screenshot = ImageGrab.grab(bbox=bbox)
        screenshot.save(screenshot_path)
        print(Fore.GREEN + "screenshot saved in " + screenshot_path + Style.RESET_ALL)

if not os.path.exists(os.path.join(os.getcwd(), "build")):
    print(Fore.RED + "build directory not found" + Style.RESET_ALL)
    exit()

print(Fore.YELLOW + "building..." + Style.RESET_ALL)
cwd = os.getcwd()
os.chdir("build")
subprocess.run(["cmake", "--build", "."], stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
os.chdir(cwd)

current_dir = os.getcwd()
DEBUG_OR_RELEASE = "Debug"
bin_dir = os.path.join(current_dir, "bin")

tests_dir = os.path.join(bin_dir, "tests")

if os.path.exists(tests_dir):
    img_files = [file for file in os.listdir(tests_dir) if file.endswith(".bmp")]
    for bmp_file in img_files:
        os.remove(os.path.join(tests_dir, bmp_file))

if len(os.listdir(bin_dir)) == 0:
    print(Fore.RED + f"bin dir empty" + Style.RESET_ALL)
    exit()

sleep_time = [1, 1, 1, 1, 10, 1, 1, 1]
i = 0
for subdir in os.listdir(bin_dir):
    if(subdir == "tests"):
        continue
    print(Fore.YELLOW + "running sample " + subdir + Style.RESET_ALL)
    subdir_path = os.path.join(bin_dir, subdir)
    if os.path.isdir(subdir_path):
        exe_dir = os.path.join(subdir_path, DEBUG_OR_RELEASE)
        if not os.path.exists(exe_dir) or not os.path.isdir(exe_dir):
            print(Fore.RED + f"bin directory not found in {subdir}, skipping..." + Style.RESET_ALL)
            continue

        exe_files = [file for file in os.listdir(exe_dir) if file.endswith(".exe")]
        if len(exe_files) != 1:
            print(Fore.RED + f"expected .exe file in directory {subdir}, skipping..." + Style.RESET_ALL)
            continue

        exe_path = os.path.join(exe_dir, exe_files[0])
        if not os.path.exists(tests_dir):
            os.makedirs(tests_dir)

        exe_window_name = exe_files[0].replace(".exe", "")
        subprocess.Popen(exe_path, cwd=exe_dir, stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)
        time.sleep(sleep_time[i])
        i += 1
        screenshot(exe_window_name, os.path.join(tests_dir, exe_window_name+".png"))
        subprocess.run(["taskkill", "/f", "/im", exe_files[0]], stdout=subprocess.DEVNULL, stderr=subprocess.STDOUT)

img_files = [file for file in os.listdir(os.path.join(os.path.join(current_dir, "bin"), "tests")) if file.endswith(".png")]
tests_passed = 0
tests_failed = 0
for bmp_file in img_files:
    generated_image_path = os.path.join(tests_dir, bmp_file)
    reference_image_path = os.path.join(os.path.join(os.path.join(current_dir, "samples"), "tests"), bmp_file)
    if(not os.path.isfile(reference_image_path)):
        tests_failed += 1
        print(Fore.RED + f"reference image {reference_image_path} does not exist, skipping..." + Style.RESET_ALL)
        continue

    generated_image = Image.open(generated_image_path)
    reference_image = Image.open(reference_image_path)

    generated_image = generated_image.convert('L')
    reference_image = reference_image.convert('L')
    mse_value = mse(generated_image, reference_image)
    if mse_value < 0.5:
        print(Fore.GREEN + f"{bmp_file} passed" + Style.RESET_ALL)
        tests_passed += 1
    else:
        print(Fore.RED + f"{bmp_file} failed" + Style.RESET_ALL)
        tests_failed += 1

print()
print()
print(Fore.GREEN + str(tests_passed) + Style.RESET_ALL+" tests passed and "+ Fore.RED + str(tests_failed) + Style.RESET_ALL +" tests failed")