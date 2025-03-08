#!/bin/bash
set -ev
apt install curl gcc git libgl1 libgl1-mesa-dev libssl-dev libfontconfig1-dev mesa-common-dev pkg-config python3 unzip libgles2-mesa-dev
rm -f llvm.sh
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
./llvm.sh 18
update-alternatives --install /usr/bin/clang clang /usr/bin/clang-18 90 \
	            --slave /usr/bin/clang++ clang++ /usr/bin/clang++-18 \
		    --slave /usr/bin/llvm-config llvm-config /usr/bin/llvm-config-18 \
		    --slave /usr/bin/lld-link lld-link /usr/bin/lld-link-18
