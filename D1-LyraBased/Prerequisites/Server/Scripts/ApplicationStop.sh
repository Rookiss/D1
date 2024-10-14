#!/bin/bash

REPOSITORY=/home/ubuntu/server
PROCESS_NAME=D1Server

CURRENT_PID=$(pgrep -fl $PROCESS_NAME | awk '{print $1}')

if [ -z "$CURRENT_PID" ]; then
    echo "The server is not currently running, so it cannot be shut down."
else
    echo "Currently Running Server PID: $CURRENT_PID"
    echo "The server is currently running, so shut it down"
    kill -15 $CURRENT_PID
    sleep 5
fi

rm -rf $REPOSITORY/*