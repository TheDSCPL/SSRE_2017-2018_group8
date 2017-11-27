#!/bin/bash

IO=/proc/$1/io          # io data of command
if [ ! -e $IO ] ; then
#    ( >&2 echo -e "PID \"$1\" not found!" )
    exit 127
fi

TEMPFILE=$(cat $IO 2>&1)   # "copy" data
echo "$TEMPFILE" | sed 's/.*/& Bytes/' | column -t
echo

exit 0;