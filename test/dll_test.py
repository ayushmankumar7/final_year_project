import ctypes as ct 

a = ct.CDLL("./output.dll")
a.run("akiyo_qcif.yuv", "output.yuv")
