void
saxpy(int sz, float a, float *restrict x, float *restrict y)
{
#pragma omp target teams distribute parallel for simd num_teams(num_blocks) \
  map(to                                                                    \
      : x [0:sz]) map(tofrom                                                \
                      : y [0:sz])
  for (int i = 0; i < sz; ++i) {
    y[i] = a * x[i] + y[i];
  }
}
