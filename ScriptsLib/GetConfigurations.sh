#!/bin/sh
MY_PATH="`dirname \"$0\"`"
MY_PATH=`realpath $MY_PATH`
/bin/find "$MY_PATH/../ConfigurationFiles" -mindepth 1 -maxdepth 1 -type f -print0 | xargs -0 -I {} basename "{}" .xml | tr '\n' ' '
