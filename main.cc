#include "fixed.h"


int main(int argc, char **argv) {
  Fixed acc(24,40000);
  Fixed rt(24,40000);
  Fixed rta(24,40000);
  Fixed rthex(24,40000);


  acc.set_zero();

  for(int i=0; i < 16000; i++) {
    printf("approx %d\n", i);
    rta.set_zero();
    rt.set(4);
    rt.div(8*i + 1);
    rta.add(&rt);

    rt.set(2);
    rt.div(8*i + 4);
    rta.sub(&rt);

    rt.set(1);
    rt.div(8*i + 5);
    rta.sub(&rt);

    rt.set(1);
    rt.div(8*i + 6);
    rta.sub(&rt);

    if(i % 2 == 1) {
      rta.div(16);
    }
    rta.shift_bytes_right(i / 2);

    //rta.shift_bytes_right(i * 2);
    //for(int j=0; j < i; j++) {
    //  rta.div(16);
    //}

    acc.add(&rta);
    //printf("accumulator\n");
    //rta.print();

    //printf("pi\n");
    acc.save("pi.bin");
    acc.print();
  }
  
  /*
  f.set(7);
  fx.set(3);
  //fx.set(-3);
  
  f.print();
  fx.print();

  //f.add(&fx);
  //f.add(&fx);
  //f.add(&fx);
  //f.set_bit(19, 1);
  //f.set_bit(2, 0);
  //f.sub(&fx);
  //f.sub(&fx);
  f.div(255);
  f.print();
  f.set(7);
  f.div(3);
  f.print();
  //f.sub(&fx);
  //f.print();
  */
  return 0;
}



