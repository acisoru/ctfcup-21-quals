#!/bin/sh
while true; do socat tcp-l:1687,reuseaddr,fork,crlf exec:"python ./padding_server.py",pty,ctty,echo=0; sleep 5; done
