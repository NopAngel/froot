import shutil
import subprocess
import sys

def check_compiler():
    if shutil.which('gcc') is not None:
        print("[+] GCC compiler found.")
        return True
    else:
        print("error: GCC compiler not found in system PATH.")
        print("please install build-essential or the gcc package.")
        sys.exit(1)

def run_project_wrapper():
    print("[*] Starting secure privilege execution sequence...")
    
    try:
        result = subprocess.run(['gcc', '--version'], 
                                capture_output=True, 
                                text=True, 
                                check=True)
        print(f"[*] Compiler version: {result.stdout.splitlines()[0]}")
    except subprocess.CalledProcessError as e:
        print(f"[-] Execution failed: {e}")

if __name__ == "__main__":
    check_compiler()
    run_project_wrapper()

