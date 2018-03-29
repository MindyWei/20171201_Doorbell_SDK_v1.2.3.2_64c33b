@echo off

pushd ..\..\..
call common.cmd
popd

D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/tool/bin/libCodeSize C:/ITEGCC/BIN/arm-none-eabi-readelf.exe D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/lib/fa626 D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/build/openrtos/test_timer/project/test_timer/test_timer.symbol test_timer_CodeSize.txt

pause
