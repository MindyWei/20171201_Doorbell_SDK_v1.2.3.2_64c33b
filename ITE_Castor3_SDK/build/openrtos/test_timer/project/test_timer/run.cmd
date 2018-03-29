@echo off

pushd ..\..\..
call common.cmd
popd

glamomem -t glamomem.dat -i -q
glamomem -t glamomem.dat -l test_timer.bin

if "" == "y" (
    glamomem -t glamomem.dat -R 0x00000001 -a 0xd8300000
) else (
    glamomem -t glamomem.dat -R 0x0025 -a 0x1F00
)
