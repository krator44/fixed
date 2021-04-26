#include "fixed.h"


int main(int argc, char **argv) {
  Fixed acc(24,40000);
  Fixed rt(24,400);
  Fixed rta(24,40000);
  Fixed rthex(24,40000);


  rt.set(3);
  rt.div(7);
  rt.print();

  rt.shift_bytes_right(2);
  rt.print();

  rt.set(3);
  rt.div(7);
  rt.div(16);
  rt.print();
  
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



