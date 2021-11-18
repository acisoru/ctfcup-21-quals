#!/bin/sh
while true; do socat tcp-l:1537,reuseaddr,fork,crlf exec:"python ./server.py",pty,ctty,echo=0; sleep 10; done
