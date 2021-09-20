void saxpy(int sz, float a, float *restrict x, float *restrict y) {
#pragma acc kernels
  for (int i = 0; i < sz; ++i)
    y[i] = a * x[i] + y[i];
}
