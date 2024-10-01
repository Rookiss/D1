#!/bin/bash

REPOSITORY=/home/ubuntu/server
LAUNCHER_NAME=D1Server

nohup sh $REPOSITORY/$LAUNCHER_NAME.sh > /dev/null 2>&1 &