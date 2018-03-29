@echo off

pushd ..\..\..
call common.cmd
popd

rem glamomem -t glamomem.dat -i -q
rem glamomem -t glamomem.dat -l test_timer.bin
glamomem -t glamomem.dat -q -e D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/sdk/target/debug/fa626/JTAG_Switch.txt
openocd -f D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/sdk/target/debug/fa626/fa626.cfg
