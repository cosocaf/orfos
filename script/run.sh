#!/bin/bash -e

SCRIPT_DIR=$(cd $(dirname $0); pwd)

cd $SCRIPT_DIR/../

if [ ! -e build ]; then
  cmake -G Ninja -B build
fi

cmake --build build

echo "Build succeeded."

cd $SCRIPT_DIR/../build/kernel

OS=kernel.elf
CPUS=1
FS=$SCRIPT_DIR/../fs.img

if [ ! -e $FS ]; then
  qemu-img create -f qcow2 $FS 4G
fi

echo "Start qemu..."

qemu-system-riscv64 \
  -machine virt \
  -bios none \
  -kernel $OS \
  -m 128M \
  -smp $CPUS \
  -nographic \
  -global virtio-mmio.force-legacy=false \
  -drive file=$FS,if=none,format=qcow2,id=x0 \
  -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
