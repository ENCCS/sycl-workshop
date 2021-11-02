Q.submit([&](handler &cgh) {
  accessor acc { buf, cgh, write_only };

  cgh.parallel_for(range<2> { n_work_items }, [=](id<2> idx) {
    acc[idx] = 42.0;
  });
});
