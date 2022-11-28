#!/bin/bash

HOST=fpga-00

echo "####### PREPARE ########"
#ssh -t $(HOST).ims-as.uni-hannover.de "echo \"SSH initiated to \$HOSTNAME\""

echo "> Copy Main.cpp to remote..."
scp -B main.cpp gesper#@$HOST.ims-as.uni-hannover.de:~/repositories/CORE_VPRO/sw/vpro/verifyHW/main.cpp #&> /dev/null
echo "> Copy sources/ to remote..."
scp -B -r sources gesper#@$HOST.ims-as.uni-hannover.de:~/repositories/CORE_VPRO/sw/vpro/verifyHW/ #&> /dev/null
echo "> Copy includes/ to remote..."
scp -B -r includes gesper#@$HOST.ims-as.uni-hannover.de:~/repositories/CORE_VPRO/sw/vpro/verifyHW/ #&> /dev/null

echo "> Creating dir 'remote_compiled'..."
mkdir -p remote_compiled

echo "####### MAKE ########"
echo "> Calling Remote Make all..."
ssh -t gesper#@$HOST.ims-as.uni-hannover.de "cd ~/repositories/CORE_VPRO/sw/vpro/verifyHW && make clean main.bin" #&> remote_compiled/compile.log
echo "  Make log (remote_compiled/compile.log):"

echo "########"
tail -n 20 remote_compiled/compile.log
echo "########"

echo "> Downloading main.bin"
scp -B gesper#@$HOST.ims-as.uni-hannover.de:~/repositories/CORE_VPRO/sw/vpro/verifyHW/main.bin remote_compiled/main.bin #&> /dev/null
echo "> Downloading final.elf"
scp -B gesper#@$HOST.ims-as.uni-hannover.de:~/repositories/CORE_VPRO/sw/vpro/verifyHW/final.elf remote_compiled/final.elf #&> /dev/null

echo "> Using as local main.bin & final.elf"
cp remote_compiled/main.bin main.bin
cp remote_compiled/final.elf final.elf

echo "###### DONE ######"
