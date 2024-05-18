import os
import subprocess
import time
import shutil

def run_debug_exe():
    current_dir = os.getcwd()

    bin_dir = os.path.join(current_dir, "bin")
    if not os.path.exists(bin_dir) or not os.path.isdir(bin_dir):
        print("bin directory not found.")
        return

    for subdir in os.listdir(bin_dir):
        subdir_path = os.path.join(bin_dir, subdir)
        if os.path.isdir(subdir_path):
            debug_dir = os.path.join(subdir_path, "Debug")
            if not os.path.exists(debug_dir) or not os.path.isdir(debug_dir):
                print(f"Debug directory not found in {subdir}. Skipping...")
                continue

            exe_files = [file for file in os.listdir(debug_dir) if file.endswith(".exe")]
            if len(exe_files) != 1:
                print(f"Expected 1 .exe file in Debug directory of {subdir}. Skipping...")
                continue

            exe_path = os.path.join(debug_dir, exe_files[0])
            subprocess.Popen(exe_path, cwd=subdir_path)
            time.sleep(1)
            subprocess.run(["taskkill", "/f", "/im", exe_files[0]])  # Close the program

            bmp_files = [file for file in os.listdir(subdir_path) if file.endswith(".bmp")]
            tests_dir = os.path.join(bin_dir, "tests")
            if not os.path.exists(tests_dir):
                os.makedirs(tests_dir)
            for bmp_file in bmp_files:
                shutil.copy2(os.path.join(subdir_path, bmp_file), tests_dir)
                os.remove(os.path.join(subdir_path, bmp_file))

if __name__ == "__main__":
    run_debug_exe()
