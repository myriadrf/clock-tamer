# Hardware #

  1. Add a separate LED for GPS lock indication
  1. Add a buffer to let J4 to be used as 1-pps output


# Software #

  1. Debug a bug with 'HWI' breaking the next command after it.
  1. Support external 1-pps GPS locking.
  1. Implement filtering with 48-bit or 64-bit precision. This will allow us to have measurements accuracy as high as 0.1ppb and accuracy will be limited by heating noise and integration time. If someone volunteer and implement following set of functions for ATMega, it will be already a huge help:
```
int48 mul48(int32,int32);
int32 div48(int48,int32);
int48 add48(int48,int48);
int48 sub48(int48,int48);
```
  1. Implement filter with better convergence time then current exponential filter. It should be possible to drop convergence time to 50ppb to 2.5min and to 0.3ppm to 17sec.