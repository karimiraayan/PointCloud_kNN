import numpy as np
import matplotlib
import PIL
PIL.Image
WIDTH= 1024
HEIGHT = 64
NUM_NEIGHBOURS=25
TOTAL_NUM_POINTS=130000
CUTOFF=5*2**9
'''
every in put is the starter file and the arrays without any suffix are the results from vpro
'''
#loading data and files
Distance = np.fromfile('../data/Distance', dtype='uint16').reshape(TOTAL_NUM_POINTS,NUM_NEIGHBOURS)
N_prim = (np.fromfile('../data/N_prim', dtype='uint16').reshape(65536,NUM_NEIGHBOURS))
depth2d_input = np.fromfile('../data/depth2d_input_vpro', dtype='uint16').reshape(HEIGHT,WIDTH)
depth3d_input = np.fromfile('../data/depth3d_input_vpro', dtype='uint16').reshape(TOTAL_NUM_POINTS,1)
points_2d_mapping = np.fromfile('../data/points_2d_mapping_input_vpro', dtype='uint16').reshape(TOTAL_NUM_POINTS,1)
labels2d_input = np.fromfile('../data/labels2d_input_vpro', dtype='uint16').reshape(HEIGHT,WIDTH)
#padding
depth2d_input=np.insert(depth2d_input,WIDTH,np.zeros(HEIGHT),axis=1)
depth2d_input=np.insert(depth2d_input,1025,np.zeros(HEIGHT),axis=1)
depth2d_input=np.insert(depth2d_input,0,np.zeros(HEIGHT),axis=1)
depth2d_input=np.insert(depth2d_input,0,np.zeros(HEIGHT),axis=1)
depth2d_input=np.insert(depth2d_input,HEIGHT,np.zeros(1028),axis=0)
depth2d_input=np.insert(depth2d_input,65,np.zeros(1028),axis=0)
depth2d_input=np.insert(depth2d_input,0,np.zeros(1028),axis=0)
depth2d_input=np.insert(depth2d_input,0,np.zeros(1028),axis=0)
#paddding labels
labels2d_input=np.insert(labels2d_input,WIDTH,np.zeros(HEIGHT),axis=1)
labels2d_input=np.insert(labels2d_input,1025,np.zeros(HEIGHT),axis=1)
labels2d_input=np.insert(labels2d_input,0,np.zeros(HEIGHT),axis=1)
labels2d_input=np.insert(labels2d_input,0,np.zeros(HEIGHT),axis=1)
labels2d_input=np.insert(labels2d_input,HEIGHT,np.zeros(1028),axis=0)
labels2d_input=np.insert(labels2d_input,65,np.zeros(1028),axis=0)
labels2d_input=np.insert(labels2d_input,0,np.zeros(1028),axis=0)
labels2d_input=np.insert(labels2d_input,0,np.zeros(1028),axis=0)
#findin neighbours
N_prim_input=np.zeros((WIDTH*HEIGHT,NUM_NEIGHBOURS),dtype=np.int32)
for v in range (HEIGHT):
    for u in range (WIDTH):
        for xix in range (5):
            for vx in range (5):
             N_prim_input [v*WIDTH+u,xix*5+vx]=depth2d_input[v+xix,u+vx]
# extending it to 130k
N_input=np.zeros((TOTAL_NUM_POINTS,NUM_NEIGHBOURS)) 
for point_index, pixel_index_uv in enumerate(points_2d_mapping):
    N_input[point_index, :] = N_prim_input[pixel_index_uv, :]
    N_input[point_index, 12]=depth3d_input[point_index]
# get the distances
D_input=np.zeros((TOTAL_NUM_POINTS,NUM_NEIGHBOURS))
for i in range (TOTAL_NUM_POINTS):
    D_input[i,:]=np.absolute([N_input[i,:]-N_input[i,12]])

print ("//-----------------------------test results--------------------------//")
#n_prim
XX=N_prim[:,:]==N_prim_input[:,:]
XGG= np.where(XX==False)
if XX.all()==True:
        print ("test pass for N_prim")
else:
        print ("something is wrong with N_prim")
        print(XGG)
#Distance
XX=D_input[:,:]==Distance[:,:]
XGG= np.where(XX==False)
if XX.all()==True:
        print ("test pass For D")
else:
        print ("something is wrong with D")
        print(XGG)
#///////////////////////////////////////////////////knn
knn_sorted=np.where(D_input >CUTOFF, 524286, D_input)# change the not needed numbers to some big number out of our range

for z in range (5):
    D_min=np.min(knn_sorted, axis=1)
    for x in range (TOTAL_NUM_POINTS):
        knn_sorted[x,:] = np.where(knn_sorted[x,:] ==D_min[x], 524287, knn_sorted[x,:])
        
knn_sorted= np.where(knn_sorted ==524287, 1, 0)
knn = np.fromfile('../data/knn_sorted', dtype='uint16').reshape(TOTAL_NUM_POINTS,NUM_NEIGHBOURS)
XX=knn_sorted[:,:]==knn[:,:]
XGG= np.where(XX==False)

if XX.all()==True:
        print ("test pass for knn_sorted")
else:
        print ("something is wrong with knn_sorted")
        print(XGG)
#============================================================L and Lprim
#L = np.fromfile('../data/L_con', dtype='uint16').reshape(TOTAL_NUM_POINTS,NUM_NEIGHBOURS)
L_prim = (np.fromfile('../data/L_prim', dtype='uint16').reshape(65536,NUM_NEIGHBOURS))

L_prim_input=np.zeros((WIDTH*HEIGHT,5*5),dtype=np.int16)
for v in range (HEIGHT):
    for u in range (WIDTH):
        for xix in range (5):
            for vx in range (5):
             L_prim_input [v*WIDTH+u,xix*5+vx]=labels2d_input[v+xix,u+vx]

L_input=np.zeros((TOTAL_NUM_POINTS,NUM_NEIGHBOURS)) 
for point_index, pixel_index_uv in enumerate(points_2d_mapping):
    L_input[point_index, :] = L_prim_input[pixel_index_uv, :]

XX=L_prim[:,:]==L_prim_input[:,:]
XGG= np.where(XX==False)

if XX.all()==True:
        print ("test pass for L_prim")
else:
        print ("something is wrong with L_prim")
        print(XGG)

#////////////////////////////////////////////////////////////votet L
voted_L=knn_sorted*(L_input+1)
x=0
votes=np.zeros((TOTAL_NUM_POINTS,NUM_NEIGHBOURS,21))
votes2=np.zeros((TOTAL_NUM_POINTS,21))
for i in range (TOTAL_NUM_POINTS):
        for j in range (NUM_NEIGHBOURS):
                if (voted_L[i,j]!=(0)):
                        votes[i,j,int(voted_L[i,j])]=1
                        
for i in range (TOTAL_NUM_POINTS):
    for j in range (NUM_NEIGHBOURS):
            votes2[i,:]=votes[i,j,:]+votes2[i,:]
            
L_con = np.fromfile('../data/L_con', dtype='uint16').reshape(TOTAL_NUM_POINTS)
L_con_results=np.argmax(votes2,1)
L_con_results=L_con_results-1
L_con_results.astype('uint16').tofile("L_con_results")
XX=L_con_results[:]==L_con[:]
XGG= np.where(XX==False)

if XX.all()==True:
        print ("test pass For L_con")
else:
        print ("something is wrong with L_con")
        print(XGG)
print ("=====================================================================")