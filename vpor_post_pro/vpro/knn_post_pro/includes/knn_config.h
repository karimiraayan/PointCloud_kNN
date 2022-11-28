const int LOAD_SIZE_RF= 35;
const int NUM_NEAREST_NEIGHBOURS= 5;
const int lm_size =8192;
const int CUTOFF= 2560;// 5*2**9
const int LOAD_SIZE_LM=140;
const int OUT_RANGER=524287;// Lidar can messure up to 120 meters, *2**9
const int SGM_BATCH_SIZE=LOAD_SIZE_LM*NUM_VU_PER_CLUSTER*NUM_CLUSTERS;