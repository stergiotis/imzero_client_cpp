#!/bin/bash
sudo steamos-readonly disable
sudo pacman-key --init
sudo pacman-key --populate archlinux
sudo pacman-key --populate holo
# reinstall missing system files
missing=$(pacman -Qk 2>/dev/null | grep -ve ' 0 missing' | grep -ie ^libc -e glibc -e gcc -e clang -e headers -e udev -e systemd  | cut -d ":" -f 1)
sudo pacman -Syu "$missing" --overwrite '*'
sudo pacman -Syu linux-api-headers # linux-api-headers is not correctly reporting missing files

pacman="sudo pacman -S --noconfirm"
$pacman wget
$pacman cmake
$pacman holo-3.6/clang
$pacman holo-3.6/gcc
$pacman libxslt
$pacman bzip2
$pacman dhall dhall-json dhall

# freetype
$pacman freetype2

# sdl3 dependencies
$pacman alsa-utils libxext libxcb libdecor wayland

# glfw
$pacman glfw-x11

$pacman base-devel

# skia dependencies
$pacman libjpeg-turbo libpng libwebp ninja procps-ng python rsync harfbuzz gcc-libs icu fontconfig expat libc++ libglvnd libx11 xorgproto zlib
$pacman holo-3.6/glibc

# note: steam-os uses a non-stock kernel: s/linux-headers/linux-neptune-headers/
kernel_version=$(uname -a | grep -E -o "neptune-[0-9]+")
$pacman holo-3.6/linux-headers "linux-${kernel_version}-headers" holo-3.6/linux-lts-headers

# tracy dependencies
$pacman wayland wayland-protocols libglvnd libxkbcommon freetype2 dbus hicolor-icon-theme capstone libffi brotli graphite #intel-tbb
# tracy missing files on steamos
$pacman brotli holo-3.6/glib2 graphite pcre2 libsysprof-capture libxau libxdmcp
