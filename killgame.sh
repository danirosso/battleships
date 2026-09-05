#!/bin/sh

ps aux | grep -i make | awk '{ print $2 }'
