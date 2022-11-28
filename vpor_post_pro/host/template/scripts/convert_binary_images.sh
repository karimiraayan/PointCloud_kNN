#!/bin/bash

echo "in dir: $PWD"

script=../../helper/main_memory_file_generator/bin2img.py
inpath=./data/result/layer_0
indim=(112 56 28 14 7 7 7)
outchan=(16 32 64 128 128 256 125)

layers=2 #

#python3 $script $inpath/channel_0.bin 112 112 $PWD/data/result/img/l0c0 2 1
for ((l=0;l<layers;l++)); do
	echo -e "Layer $l\n"
	inpath=./data/result/layer_$l
	channels=${outchan[l]}
	for ((i=0;i<channels;i++)); do
		echo -e "\e[1A\e[Converting .bin->png ($inpath) (Channel $i)"
		dd if=$inpath/channel_$i.bin of=$PWD/data/result/img/tmp.bin status=none #conv=swab 
		python3 $script $PWD/data/result/img/tmp.bin ${indim[l]} ${indim[l]} $PWD/data/result/img/l${l}c${i} 2 1
	done

	cmd="convert "
	for ((i=0;i<channels;i++)); do
		cmd="$cmd $PWD/data/result/img/l${l}c${i}.png"
        done
	cmd="$cmd +append $PWD/data/result/img/alllayer$l.png"
	eval $cmd
	rm -rf $PWD/data/result/img/l*.png
done
cmd="convert "
for ((l=0;l<layers;l++)); do
	cmd="$cmd $PWD/data/result/img/alllayer$l.png"
done
cmd="$cmd -append $PWD/data/result/img/all.png"
eval $cmd

rm -rf $PWD/data/result/img/*.bin

eog $PWD/data/result/img/all.png

#convert ../data/out_0.png ../data/out_1.png ../data/out_2.png ../data/out_3.png +append ../data/row1.png
#convert ../data/row1.png ../data/row2.png ../data/row3.png ../data/row4.png -append ../data/output_ref_all.png
#rm -rf row*.png
#eog ../data/result_out.png
