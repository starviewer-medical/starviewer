#!/bin/bash
SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)
. $SCRIPTS_ROOT/config/setup.sh

# This script detects the operating system type and then installs the required dependencies
if [[ $(uname) == 'Linux' ]]
then
    echo "Linux based system detected."
    if [ -e /etc/debian_version ]
    then
        echo "Debian-based system detected."
        if [ "10" = `cut -d . -f 1 /etc/debian_version` ]
        then
            echo "Debian 10 (Buster) detected."
            echo "Please, input the password to install the dependencies required to run and compile Starviewer."
            sudo apt-get install build-essential cmake git wget rsync xsltproc rpm
            sudo apt-get install libssl1.1 libssl-dev zlib1g zlib1g-dev libgl1 libgl1-mesa-dev libxt6 libxt-dev libglu1-mesa libglu1-mesa-dev
        fi
    fi
elif [[ $(uname) == 'Darwin' ]]
then
    echo "macOS system detected."
elif [[ $(uname) == 'MSYS_NT'* ]]
then
    echo "Windows NT (MSYS2) system detected."

    # MSys2 packages
    # --------------
    pacman -S --noconfirm unzip mingw64/mingw-w64-x86_64-cmake cmake git wget rsync
    pacman -Syu --noconfirm

    echo "[!] If Pacman upgrades, EXPECT FAILURE on wget or other commands. Close this window and re-run this script again. [!]"

    # MSVC
    # ----
	mkdir -p "${DOWNLOAD_PREFIX}"
    download_and_verify 2f9a69561f6678f745b55ea38a606180b3941637d7e8cbbb65acae6933152d3e \
                    'vs_BuildTools.exe' \
                    'https://download.visualstudio.microsoft.com/download/pr/dfb60031-5f2b-4236-a36b-25a0197459bc/2f9a69561f6678f745b55ea38a606180b3941637d7e8cbbb65acae6933152d3e/vs_BuildTools.exe'

    echo '.\vs_BuildTools.exe --lang en-US --layout vs --wait --add Microsoft.VisualStudio.Workload.VCTools --add Microsoft.VisualStudio.Component.Windows10SDK.17763 --add Microsoft.VisualStudio.Component.VC.Redist.14.Latest' > "${DOWNLOAD_PREFIX}/vs_BuildTools_offlinePackager.bat"
    if [[ ! -d "${DOWNLOAD_PREFIX}/vs" ]]
    then
            echo "[!] No 'vs' offline installation directory detected at the downloads directory. [!]"
            echo "A file named 'vs_BuildTools_offlinePackager.bat' has been created at the downloads directory."
            echo "1. Run the 'vs_BuildTools_offlinePackager.bat' on a internet connected machine."
            echo "2. If the execution was performed on a different machine, copy the created 'vs' directory to the downloads folder."
            read -p "Press enter key when ready to continue..."
    else
            echo "Visual Studio Build Tools offline installation directory detected."
    fi

    # Check if an offline visual studio installer is present
    if [[ -e "${DOWNLOAD_PREFIX}/vs/vs_BuildTools.exe" ]]
    then
        echo "Visual Studio Build Tools offline installation directory detected. Doing an unattended setup."
        "${DOWNLOAD_PREFIX}/vs/vs_BuildTools.exe" --noUpdateInstaller --wait --noWeb --nocache --passive
        if [ $? -ne 0 ]
        then
            echo "Error installing Visual Studio Build Tools."
            echo "Quitting..."
            exit
        fi
    fi

    # Long file name support
    # ----------------------
    reg query 'HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\FileSystem' -v LongPathsEnabled | grep "LongPathsEnabled    REG_DWORD    0x1"
    if [[ $? -ne 0 ]]
    then
        echo "It is strongly recommended that you lift path length limit to avoid problems with ITK. If you have not done it already check this link: https://www.howtogeek.com/266621/how-to-make-windows-10-accept-file-paths-over-260-characters/"
        read -p "Do you want the script to do it for you [Y/n]? Press Ctrl+C to stop. " RESPONSE
        case $RESPONSE in
            [Yy] )
                reg add 'HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\FileSystem' -v LongPathsEnabled -d 1 -t REG_DWORD
                echo "[!] You must reboot the computer. [!]"
                echo "Quitting..."
                exit
                ;;
            * )
                echo "Beware that the ITK compilation may fail."
                echo "Continuing..."
                ;;
        esac
    fi
fi
