#!/bin/bash

if [ -n "$1" ]; then
	avrdude -p m168p -c avrispmkII -P usb -U flash:w:$1
else
	echo "empty arg"
fi