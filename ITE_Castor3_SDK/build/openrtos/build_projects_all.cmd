@echo off

set BOOTLOADER=1
set PRESETTING=
set CODEC=1
set CODEC_EX=1
rem set AUTOBUILD=1

call build_projects.cmd

@if not defined NO_PAUSE pause
