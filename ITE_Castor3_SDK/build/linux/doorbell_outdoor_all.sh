#!/bin/bash

#export AUTOBUILD=1
export BOOTLOADER=1
export CODEC_IT9910=1
export CODEC_EX_IT9910=1
export TARGET=`basename $0 .sh | sed s/_all//g`

./${TARGET}.sh

