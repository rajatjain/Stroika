#!/bin/bash
THIS_SCRIPT_DIR="`dirname \"$0\"`"
THIS_SCRIPT_DIR=`realpath $THIS_SCRIPT_DIR`
THIS_SCRIPT_DIR="$THIS_SCRIPT_DIR/"

TEXT="$1"


if [ ! -z $StroikaRoot ] ; then
	TEXT=`echo $TEXT | sed s%$StroikaRoot%\{StroikaRoot\}%g`
fi
if [ ! -z $TOP_ROOT ] ; then
	TEXT=`echo $TEXT | sed s%$TOP_ROOT%\{TOP_ROOT\}%g`
fi
echo -n $TEXT