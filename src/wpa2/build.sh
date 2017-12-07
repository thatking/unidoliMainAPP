#!/bin/sh
GCC_ARM="arm-hisiv500-linux-gcc"
#-lsns_ov2718 lsns_imx323
LIB_NAME_ARM="-ldl -lupvqe -ldnvqe -lVoiceEngine -lmpi -live -lmd -l_hiae -l_hiawb -l_hiaf -lisp -l_hidefog 
-lm 
-lpthread 
"

LIB_ARM="-L`pwd`/libhisi1.0.1.0/lib 
"

INC_ARM="-I`pwd`/libhisi1.0.1.0/include 
-I`pwd`/wpa_supplicant
"

FILE_LIST="./src/sample_comm_sys.c 
./src/unidoli_audio.c 
./src/audio_transfer_fft.c 
./src/unidoli_main.c 
./wpa_supplicant/libwpa_ctrl.a
"

#OMNIVISION_OV2718_MIPI_1080P_30FPS SONY_IMX323_CMOS_1080P_30FPS
DEFINE_ARM="-DSENSOR_TYPE=OMNIVISION_OV2718_MIPI_1080P_30FPS"

echo "->>>> build unidoli wpa2 v10 <<<<-"

$GCC_ARM  -Wall  $LIB_NAME_ARM $LIB_ARM $INC_ARM $DEFINE_ARM $FILE_LIST  -o unidoli_wpa2
