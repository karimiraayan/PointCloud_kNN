Author: Sven Gesper, IMS, 24.03.2021

## Snippets

`dd if=./data/input0.bin of=./data/input0_revEnd.bin conv=swab`  
`dd if=./data/input2.bin of=./data/input1_revEnd.bin conv=swab`  
`dd if=./data/input1.bin of=./data/input2_revEnd.bin conv=swab`

## Changes to reduce Control flow of MIPS

#### requirements:  
  * hardwareconfig (x cluster, x units)  
  * cnn structure & parameters (quantized in correct fpf)
    
    
    
#### preparation:  
  * Target: **generate_configuration**  
  * generates layer file (fix array to be loaded to execution by checker)  
  * generates weights file (fix array to be loaded to execution by checker)  
  * generates segments file (fix array to be loaded to execution by checker)  
    * to be executed in order   
    * dma and vpro execution order  
    * constant addresses / no control flow  
    * mapping to appropiate hw config is done buring generation of this list  
    
    
    
#### execution:  
  * Target: **sim** or **vpro** (TODO: Makefile adjust)  
  * load of cnn layer file  
  * load of cnn weights file  
  * load of segments file  
  * execution in order:  
    * initializes vpro math (cnn related MAC-shift, padding, ...)  
    * loop all segements, each segment either:  
      * dma load kernel (broadcast? pad? addr?)  
        DMA_DIRECTION::,  cluster, unit  
        dma.mm_addr = segment.in_channel, segment.out_channel, layer.in_channels, layer.out_channels, layer.kernel_size  
        dma.lm_addr = buffer + 4096 - (kernel_x * kernel_y * (lane + 1));  
        dma.word_count = kernel_x * kernel_y;  
      * dma load bias  
        DMA_DIRECTION::, cluster, unit  
        dma.lm_addr = buffer + 4096 - (kernel_x * kernel_y * (2)) - 1 - lane;  
        dma.mm_addr = *bias = ... layer.in_channels, layer.out_channels, layer.kernel_size, segment.out_channel  
        dma.word_count = 1;  
      * dma load input  
        DMA_DIRECTION::, cluster, unit  
        dma.x_size = layer.seg_in_w;  
        dma.y_size = layer.seg_in_h;  
        dma.x_stride = segment.in_MM_x_stride_0;  
        dma.mm_addr = segment.in_MM_base_0;  
        dma.lm_addr = buffer;  
        dma.pad[CommandDMA::PAD::TOP] = segment.pad_top;  
        dma.pad[CommandDMA::PAD::RIGHT] = segment.pad_right;  
        dma.pad[CommandDMA::PAD::BOTTOM] = segment.pad_bottom;  
        dma.pad[CommandDMA::PAD::LEFT] = segment.pad_left;  
      * vpro conv_start (bias)  
        layer.stride  
        layer.seg_in_w  
        layer.seg_out_w  
        layer.seg_out_h  
        buffer = buffer_calc*4096  
        layer.conv_result_shift_right  
      * vpro conv_add
        same  
      * vpro relu (+pool)  
        relu.type  
        layer.seg_out_w  
        flag [4-split due to large xend/yend | 1 blob]  
          4x xend, yend, offset  
        layer.pool.stride  
          'pooled: call 4/1x pool + relu  
          'not pooled: call 4/1x relu  
        ? layer.kernel_length  
        ? layer.conv_result_shift_right  
      * vpro pool ...  
      * vpro shift_store  
        lane  
        buffer = (int(buffer_calc) * 4096) + lane * 1024;  
        flag [4-split due to large xend/yend | 1 blob]  
        4x xend, yend, offset  
        layer.store_shift_right  
        layer.seg_out_w  
      * vpro residual  
        buffer = (int(buffer_calc) * 4096);  
        xend, yend, offset  
        layer.seg_in_w  
        layer.residual_0_left_shift  
        layer.residual_1_left_shift  
        layer.seg_out_w  
     * dma store result  
        cluster,   
        segment.out_MM_base, unit, buffer,  
        segment.out_MM_x_stride,   
        xsize = seg.seg_in_w, ysize = seg_in_h  
      * dma wait  
      * vpro wait  
    * execute relevant function (inlined)  
    * communicates with checker  
  * check:
    * inter - function calls need sync?
    * 
