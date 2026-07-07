/**
 * @mainpage FROOT - Compilation Guide
 *
 * FROOT is a lightweight security utility designed as an alternative to `sudo` and `doas`. 
 * It enables controlled command execution with elevated permissions via extensive configuration profiles.
 *
 * @section req_sec System Requirements
 * - **Operating System:** UNIX-like environment (GNU/Linux highly recommended).
 * - **Compiler:** GCC (GNU Compiler Collection).
 * - **Build System:** GNU Make.
 * - **Version Control:** Git.
 *
 * @warning Compatibility with native Windows environments is not officially supported. 
 * If you are running Windows, you must use **WSL2 (Windows Subsystem for Linux)**.
 *
 * @section comp_sec Build and Installation Setup
 *
 * @subsection step1 Step 1: Install Dependencies
 * Ensure your system package manager has all the required development tools:
 * @code{.sh}
 * # For Debian/Ubuntu-based systems:
 * sudo apt update && sudo apt install build-essential git
 * 
 * # For Arch Linux-based systems:
 * sudo pacman -Syu base-devel git
 * @endcode
 *
 * @subsection step2 Step 2: Clone the Repository
 * Fetch the latest source code from the official repository:
 * @code{.sh}
 * git clone https://github.com/NopAngel/froot
 * cd froot
 * @endcode
 *
 * @subsection step3 Step 3: Compile the Binary
 * Execute the build script using the Makefile:
 * @code{.sh}
 * make
 * @endcode
 *
 * @subsection step4 Step 4: Install System-Wide
 * Install the compiled executable into the standard system path:
 * @code{.sh}
 * sudo make install
 * @endcode
 *
 * @note After installation, you may need to configure the `froot.conf` file 
 * before executing commands with elevated privileges.
 */

