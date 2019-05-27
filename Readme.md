# FastDonkey : a c/c++ framework for Donkeycar

FastDonkey is a c/c++ framework for DonkeyCar to autonumously drive. 

## What's better to the python version donkeycar
* Higher loop frequency. Main loop cycle time is shortened from 30ms to 20ms roughly.
* PWM direct drive from Pi. Do not need a servo drive board. Also no iic communication delay.


## Dependency
* [Python version donkeycar](https://github.com/autorope/donkeycar). Used for training data collection and keras model training.
* [ncnn deep learning framework](https://github.com/Tencent/ncnn). For high performance inference computing. Install it on Pi.
* [Raspicam](https://github.com/cedricve/raspicam). C++ API for using Raspberry camera. Install it on Pi.
* [Keras2caffe](https://github.com/uhfband/keras2caffe.git ). Transform model from keras to caffe. Install it on PC.
* [RPIO](https://github.com/metachris/RPIO).  a GPIO toolbox for the Raspberry Pi. No need to install, source code included in this repo. Install it on Pi.
* [caffe](http://caffe.berkeleyvision.org/). Caffe is a deep learning framework. Install it on Pi and PC.

## How to use
1. Prepare model.
1.1 Get keras model from python version Donkeycar.
1.2 Transform to caffe model with Keras2caffe. Remember to change your model path in keras_2_caffe.py
```
python keras_2_caffe.py
```
1.3 Upgrade caffe model in PC for ncnn. Upgrade tools are from caffe.
```
./upgrade_net_proto_binary.bin caffe_mypilot.caffemodel new_caffe_mypilot.caffemodel
./upgrade_net_proto_text.bin caffe_mypilot.prototxt new_caffe_mypilot.prototxt
```
1.4 Transorm updated caffe model to ncnn model in Pi. caffe2ncnn is from ncnn.
```    
./caffe2ncnn new_caffe_mypilot.prototxt new_caffe_mypilot.caffemodel ncnn_mypilot.param ncnn_mypilot.bin
```
2. Wiring
2.1 Just connect any pwm pins to throttle and steering servo. My connection is as follows:
```
throttle， 18(RGPIO number）,12(pin number)，G1(gpio number)
steering， 12(RGPIO number）,32(pin number)，G26(gpio number)
```
I am crazy about pin number too!

3. Compile and RUN
3.1 clone this repo and :
```
mkdir build
cd build
cmake ..
make
sudo ./fastdonkey
```

Enjoy crashing! :-P

TODOs:
1. Put getting frame from camera to another thread. May save another 10ms.
2. Use ncnn optimize tool to accelerate inference speed.


==========================================================
Build for Raspberry Pi 3
install g++ cmake protobuf

$ cd <ncnn-root-dir>
$ mkdir -p build
$ cd build
$ cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/pi3.toolchain.cmake -DPI3=ON ..
$ make -j4
$ make install
pick build/install folder for further usage


ncnnoptimize mobilenet.param mobilenet.bin mobilenet-opt.param mobilenet-opt.bin 65536


https://github.com/Tencent/ncnn/wiki/quantized-int8-inference


Cycle Time: 16.8884ms
angle_out: -0.264942
throttle_out: 0.308929
Cycle Time: 16.4792ms
angle_out: -0.264942
throttle_out: 0.308929
Cycle Time: 17.0195ms
angle_out: -0.264942
throttle_out: 0.308929
Cycle Time: 16.3378ms
angle_out: -0.264942
throttle_out: 0.308929
Cycle Time: 17.0482ms
angle_out: -0.264942
throttle_out: 0.308929

getconf LONG_BIT

