from rknnlite import RKNN

model_path = fr"/root/CMake/model/deploy.prototxt"
weights_path = fr"/root/CMake/model/res10_300x300_ssd_iter_140000.caffemodel"

rknn = RKNN()

ret = rknn.load_caffe(model=model_path, weights=weights_path)
if ret != 0:
    print("加载 Caffe 模型失败！")
    exit(ret)

rknn.config(target=RKNN.TARGET_NPU)
ret = rknn.build(do_quantization=True, dataset='./dataset.txt')
if ret != 0:
    print("模型编译失败！")
    exit(ret)
rknn.export_rknn('model.rknn')

rknn.release()
