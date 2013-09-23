#include <stdio.h>

////////////////////////////////////////////////
/* Park-Miller pseudo-random number generator */
////////////////////////////////////////////////

#define RNG_M 2147483647L /* m = 2^31 - 1 */
#define RNG_A 16807L
#define RNG_Q 127773L     /* m div a */
#define RNG_R 2836L       /* m mod a */


static long rnd_seed;

//Setup RNG
void rand_init() {
  long seedval = 1234567890L;
  rnd_seed = (seedval % (RNG_M - 1)) + 1;
};


long rnd() {
  long low, high, test;

  high = rnd_seed / RNG_Q;
  low = rnd_seed % RNG_Q;
  test = RNG_A * low - RNG_R * high;
  if (test > 0)
    rnd_seed = test;
  else
    rnd_seed = test + RNG_M;
  return rnd_seed;
}

//////////////
//Random int from 0 to lim-1:
int randi(int lim) {
  return (int)((float)lim * rnd() / (RNG_M - 1.0));
}

//////////////
//Random float 0>-1.0
float randf() {
  return (float)(rnd() / (RNG_M - 1.0));
}
