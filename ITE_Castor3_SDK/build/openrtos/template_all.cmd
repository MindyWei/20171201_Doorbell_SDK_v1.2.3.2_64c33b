@echo off

set BOOTLOADER=1
set CODEC=1
set CODEC_EX=1
set CODEC_IT9850=1
set CODEC_EX_IT9850=1
rem set AUTOBUILD=1

call template.cmd

@if not defined NO_PAUSE pause
