#!/bin/bash
#
# Cross-compiling with Visual Studio Code and Petalinux SDK:
# Put following stuff into .vscode/settings.json (replace the value for PETALINUX_SDK with the actual location):
# {
#    "cmake.environment": { "PETALINUX_SDK": "/mnt/yocto_500G/petalinux/environment-setup-aarch64-xilinx-linux" },
#    "cmake.configureSettings": { "TARGET_HW": "ZUP" },
#    "cmake.cmakePath": "${workspaceRoot}/cross-cmake.sh"
# }
#
source ${PETALINUX_SDK}
cmake "${@}"
