#!/bin/bash

echo "in dir: ${PWD}"

script=../../helper/main_memory_file_generator/bin2img.py
inbase=data/results
# inpath=${inbase}/layer_0
indim=(112 56 28 14 7 7 7)
outchan=(16 32 64 128 128 256 125)

layers=2 #

echo -e "Owning Root dir to current User ($inbase)"
sudo chown ${USER}:${USER} ${inbase}

echo -e "(Creating) result dir: ${PWD}/${inbase}/img"
mkdir -p ${PWD}/${inbase}/img

echo -e "Processing:\n"
#python3 $script $inpath/channel_0.bin 112 112 ${PWD}/${inbase}/img/l0c0 2 1
for ((l=0;l<layers;l++)); do
	echo -e "Layer $l\n"
	inpath=${inbase}/layer_$l
	channels=${outchan[l]}
	for ((i=0;i<channels;i++)); do
		echo -e "\e[1A\e[Converting .bin->png ($inpath) (Channel $i)"
		dd if=$inpath/channel_$i.bin of=${PWD}/${inbase}/img/tmp.bin status=none #conv=swab 
		python3 $script ${PWD}/${inbase}/img/tmp.bin ${indim[l]} ${indim[l]} ${PWD}/${inbase}/img/l${l}c${i} 2 1
	done

	cmd="convert "
	for ((i=0;i<channels;i++)); do
		cmd="$cmd ${PWD}/${inbase}/img/l${l}c${i}.png"
        done
	cmd="$cmd +append ${PWD}/${inbase}/img/alllayer$l.png"
	eval $cmd
	rm -rf ${PWD}/${inbase}/img/l*.png
done
cmd="convert "
for ((l=0;l<layers;l++)); do
	cmd="$cmd ${PWD}/${inbase}/img/alllayer$l.png"
done
cmd="$cmd -append ${PWD}/${inbase}/img/all.png"
eval $cmd

rm -rf ${PWD}/${inbase}/img/*.bin

eog ${PWD}/${inbase}/img/all.png

#convert ../data/out_0.png ../data/out_1.png ../data/out_2.png ../data/out_3.png +append ../data/row1.png
#convert ../data/row1.png ../data/row2.png ../data/row3.png ../data/row4.png -append ../data/output_ref_all.png
#rm -rf row*.png
#eog ..${inbase}_out.png
