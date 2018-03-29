@echo off

pushd ..\..\..
call common.cmd
popd

arm-none-eabi-insight -x D:/Project/Doorbell/20171201_Doorbell_SDK_v1.2.3.2_64c33b/ITE_Castor3_SDK/sdk/target/debug/fa626/init-gdb test_timer
if errorlevel 1 pause
