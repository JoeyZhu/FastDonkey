import sys
sys.path.append('.')
import keras2caffe
from tensorflow.python.keras.models import Model, load_model
import cv2
import numpy as np
import caffe

if __name__ == '__main__':

    model = load_model("./mypilot_merge_all_noshift")

    keras2caffe.convert(model, 'caffe_mypilot.prototxt', 'caffe_mypilot.caffemodel')

    exit()
    #testing the model

    caffe.set_mode_gpu()
    net  = caffe.Net('caffe_mypilot.prototxt', 'caffe_mypilot.caffemodel', caffe.TEST)

    img = cv2.imread('1030_cam-image_array_.jpg')
    img = cv2.resize(img, (160, 120))
    img = img[...,::-1]  #RGB 2 BGR

    data = np.array(img, dtype=np.float32)
    data = data.transpose((2, 0, 1))
    data.shape = (1,) + data.shape

    # data -= 128

    out = net.forward_all(data = data)
    angle = out['angle_out']
    throttle = out['throttle_out']
    print ("caffe: ", angle, ", ", throttle)
