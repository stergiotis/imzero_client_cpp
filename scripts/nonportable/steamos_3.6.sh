#!/bin/bash
sudo steamos-readonly disable
sudo pacman-key --init
sudo pacman-key --populate archlinux
# reinstall missing system files
missing=$(pacman -Qk 2>/dev/null | grep -ve ' 0 missing' | grep -ie ^libc -e glibc -e gcc -e clang -e headers -e udev -e systemd  | cut -d ":" -f 1)
sudo pacman -Syu "$missing" --overwrite '*'

pacman="sudo pacman -S --noconfirm"
if ! [ -x "$(command -v wget)" ]; then
	$pacman wget
fi
if ! [ -x "$(command -v cmake)" ]; then
	$pacman cmake
fi
if ! [ -x "$(command -v clang)" ]; then
	$pacman clang
fi
if ! [ -x "$(command -v gcc)" ]; then
	$pacman gcc
fi
if ! [ -x "$(command -v xsltproc)" ]; then
	$pacman libxslt
fi
if ! [ -x "$(command -v bzip2)" ]; then
	$pacman bzip2
fi
# freetype
$pacman freetype2

# sdl3 dependencies
$pacman alsa-utils libxext libxcb libdecor wayland

# glfw
$pacman gflw

$pacman base-devel

# skia dependencies
$pacman libjpeg-turbo libpng libwebp ninja procps-ng python rsync harfbuzz glibc gcc-libs icu fontconfig expat libc++ libglvnd libx11 xorgproto zlib

# note: steam-os uses a non-stock kernel: s/linux-headers/linux-neptune-headers/
$pacman holo-3.6/linux-headers linux-neptune-headers holo-3.6/linux-lts-headers

# tracy dependencies
$pacman wayland wayland-protocols libglvnd libxkbcommon freetype2 dbus hicolor-icon-theme capstone libffi brotli holo-3.6/glib2 graphite #intel-tbb
# tracy missing files on steamos
$pacman brotli holo-3.6/glib2 graphite pcre2 libsysprof-capture libxau libxdmcp
