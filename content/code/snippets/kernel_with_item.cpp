Q.submit([&](handler& cgh) {
  auto accA = bufA.get_access<access::mode::read>(cgh);
  auto accB = bufB.get_access<access::mode::read>(cgh);
  auto accR = bufR.get_access<access::mode::write>(cgh);

  cgh.parallel_for(range { dataSize }, [=](item<1> itm) {
    auto globalId  = itm.get_id();
    accR[globalId] = accA[globalId] + accB[globalId];
  });
});
