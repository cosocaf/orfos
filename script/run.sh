#!/bin/bash -e

SCRIPT_DIR=$(cd $(dirname $0); pwd)
OS=kernel
CPUS=1
FS=$SCRIPT_DIR/../fs.img

if [ ! -e $FS ]; then
  # qemu-img create -f raw $FS 128M
  touch $FS
fi

cd $SCRIPT_DIR/../

if [ ! -e build ]; then
  cmake -G Ninja -B build
fi

cmake --build build

echo "Build succeeded."

cd $SCRIPT_DIR/../build/kernel

echo "Start qemu..."

qemu-system-riscv64 \
  -machine virt \
  -bios none \
  -kernel $OS \
  -m 128M \
  -smp $CPUS \
  -nographic \
  -global virtio-mmio.force-legacy=false \
  -drive file=$FS,if=none,format=raw,id=x0 \
  -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
