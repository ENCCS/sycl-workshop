__global__ void saxpy(int sz, float a, float *restrict x, float *restrict y) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i < sz)
    y[i] = a * x[i] + y[i];
}
