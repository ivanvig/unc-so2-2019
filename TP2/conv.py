import numpy as np
from netCDF4 import Dataset
from scipy.signal import convolve2d, fftconvolve
from matplotlib import pyplot as plt

# filename = 'OR_ABI-L2-CMIPF-M6C02_G16_s20191011800206_e20191011809514_c20191011809591.nc'
# data_dict = Dataset(filename, mode='r')

# im_matrix = data_dict.variables['CMI']
# np.ma.set_fill_value(im_matrix, 0)
# im_matrix = im_matrix.filled()
# kernel = np.array([
#     [-1, -1, -1],
#     [-1, 8,  -1],
#     [-1, -1, -1],
# ])

# im_matrix = im_matrix[::10,::10]

# conv_img = convolve2d(im_matrix, kernel, mode='valid')

NX =  21696
PNX = NX - 2

fig, axes = plt.subplots(nrows=1, ncols=2, sharey=True, sharex=True)
matrix = np.fromfile("inmatrix.bin", dtype=np.float32)
matrix = matrix.reshape(NX,NX)
axes[0].imshow(matrix, cmap='gray', vmin=0, vmax=4096)

matrix = np.fromfile("outmatrix.bin", dtype=np.float32)
matrix = matrix.reshape(PNX,PNX)

axes[1].imshow(matrix, cmap='gray', vmin=0, vmax=4096)
# plt.imshow(matrix, cmap='gray')
plt.show()

