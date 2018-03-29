@echo off

set BOOTLOADER=1
rem set AUTOBUILD=1

call dynamic_ui.cmd

@if not defined NO_PAUSE pause
