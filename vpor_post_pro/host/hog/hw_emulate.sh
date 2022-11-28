# simple script to run hw simulation

make clean_all compile_all


./../../helper/main_memory_file_generator/img2bin image.bmp 1500 1000 img_in.bin 2

./main img_in.bin 1500 1000 img_out.bin 1494 994

./../../helper/main_memory_file_generator/bin2img img_out.bin 1496 994 img_out.bmp 2

