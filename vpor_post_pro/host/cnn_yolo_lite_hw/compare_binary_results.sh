#!/bin/bash

echo "in dir: $PWD"

indim=(112 56 28 14 7 7 7)
outchan=(16 32 64 128 128 256 125)

layers=1 #

for ((l=0;l<layers;l++)); do
	echo -e "Layer $l"
	inpath=./data/result/layer_$l
	channels=${outchan[l]}
	for ((i=0;i<channels;i++)); do
		echo -ne "\033[1KComparing .bin ($inpath) (Channel $i)"
		file1=./data/result/layer_$l/channel_$i.bin
		file2=/home/gesper/repositories/CORE_VPRO/sw/vpro/cnn_yolo_lite_sim/data/reference_c/binary/Layer_$((l+1))/channel_$i.bin
		diff -q $file1 $file2 1>/dev/null
		if [[ $? == "0" ]]
        then
          echo -e "\t\e[92mThe same!\e[39m"
        else
          echo -e "\t\e[91mNot as Reference!\e[39m"
        fi
	done
done
