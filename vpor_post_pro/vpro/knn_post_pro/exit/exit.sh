#echo "[Exit-Script] start"
# in dir: $PWD"WD"
#echo "empty"

#
# after storing data from mm this script is called (last action of simulator)
# can be used to convert data from bin to png ...
# python3 ../exit/bin2img.py ../data/output.bin 25 65536 ../data/output 2 1
python3 ../data/tester.py

#echo "img_out"

