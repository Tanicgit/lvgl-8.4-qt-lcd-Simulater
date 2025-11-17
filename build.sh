#!/bin/bash -x

set -e 
pwd_path=$(cd $(dirname "$0") && pwd)

source /opt/fsl-imx-wayland/6.1-mickledore/environment-setup-armv8a-poky-linux

cd $pwd_path

cmd=${1:-release}

case $cmd in
    "clean")
        rm -rf build
    ;;

    "dbg")
        if [  -d build/debug ];then
            cd build/debug
            cmake --build .
        else         
            mkdir -p build/debug
            cd build/debug  
            cmake -DCMAKE_BUILD_TYPE=Debug ../..  
            cmake --build .
        fi     
    ;;

    "release")
        
        if [  -d build/release ];then
            cd build/release
            cmake --build . 
        else         
            mkdir -p build/release
            cd build/release  
            cmake -DCMAKE_BUILD_TYPE=Release ../..  
            cmake --build .
        fi     
    ;;



esac


