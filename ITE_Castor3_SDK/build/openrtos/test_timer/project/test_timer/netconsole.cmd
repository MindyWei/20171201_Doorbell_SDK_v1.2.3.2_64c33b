@echo off

set CFG_PROJECT=%~n0

pushd ..\..\..
call common.cmd
popd

nc -l -u -p 
