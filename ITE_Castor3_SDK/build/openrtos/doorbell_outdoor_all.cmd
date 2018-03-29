@echo off

set BOOTLOADER=1
set CODEC_IT9910=1
set CODEC_EX_IT9910=1
rem set AUTOBUILD=1

call doorbell_outdoor.cmd

@if not defined NO_PAUSE pause
