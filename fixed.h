#include <cstdlib>
#include <cstdio>
#include <cstring>

class Fixed {
  public:
  unsigned char *data;
  unsigned int whole;
  unsigned int frac;
  unsigned int bytes;
  unsigned int bits;

  // 0 positive
  // 1 negative
  unsigned char sign;

  Fixed(int nwhole, int nfrac) {
    whole = nwhole;
    frac = nfrac;
    bits = nwhole + nfrac;
    bytes = bits / 8 + 1;
    sign = 0;
    data = new unsigned char[bytes];
    set_zero();
  }

  Fixed(Fixed *arg) {
    whole = arg->whole;
    frac = arg->frac;
    bits = arg->bits;
    bytes = arg->bytes;
    data = new unsigned char[bytes];
    set_fixed(arg);
  }

  ~Fixed() {
    if (data != 0) {
      delete[] data;
      data = 0;
    }
  }

  void set_fixed(Fixed *arg) {
    sign = arg->sign;
    for(int i = 0; i < bytes; i++) {
      data[i] = arg->data[i];
    }
  }

  void set_zero() {
    sign = 0;
    memset(data, 0, bytes);
  }

  void set(int x) {
    set_zero();
    if(x < 0) {
      x = -x;
      sign = 1;
    }
    else {
      sign = 0;
    }

    if(whole < 16) {
      unsigned int xx = 0x1;
      xx = xx << whole;
      if (x >= xx) {
        printf("error in set: %d doesn't fit in %d bits\n", x, whole);
        exit(0);
      }
    }

    if(whole < 8) {
      unsigned char ch;
      ch = (unsigned char) x;
      ch = ch << (8 - whole);
      data[0] |= ch;
    }
    else {
      unsigned char ch;
      for (int i=whole-1; i >= 0; i--) {
        ch = x & 0x01;
        set_whole_bit(i, ch);
        x = x >> 1;
      }
    }
  }

  void add(Fixed *x) {
    if(sign == 1) {
      printf("error in add(): our sign is negative\n");
      printf("adder doesn't work with negatives\n");
      exit(0);
    }
    if(x->sign == 1) {
      printf("error in add(): argument sign is negative\n");
      printf("adder doesn't work with negatives\n");
      exit(0);
    }
    if (whole != x->whole || frac != x->frac) {
      printf("error in add(): formats don't match\n");
      printf("we are %d:%d and arg is %d:%d\n",
        whole, frac, x->whole, x->frac);
      exit(0);
    }

    unsigned char top_bit, bottom_bit, carry;
    top_bit = 0;
    bottom_bit = 0;
    carry = 0;
    int tt;

    for(int i = bits - 1; i >= 0; i--) {
      top_bit = get_bit(i);
      bottom_bit = x->get_bit(i);
      tt = top_bit + bottom_bit + carry;
      //printf("debug: top_bit %d, bottom_bit %d, carry %d, tt %d\n",
      //  top_bit, bottom_bit, carry, tt);
      set_bit(i, tt%2);
      carry = tt/2;
    }
    if (carry != 0) {
      printf("error in add(): overflow\n");
      print();
      x->print();
      exit(0);
    }
  }

  void sub(Fixed *x) {
    if(sign == 1) {
      printf("error in sub(): our sign is negative\n");
      printf("sub doesn't work with negatives\n");
      exit(0);
    }
    if(x->sign == 1) {
      printf("error in sub(): argument sign is negative\n");
      printf("sub doesn't work with negatives\n");
      exit(0);
    }
    if (whole != x->whole || frac != x->frac) {
      printf("error in sub(): formats don't match\n");
      printf("we are %d:%d and arg is %d:%d\n",
        whole, frac, x->whole, x->frac);
      exit(0);
    }
    unsigned char top_bit, bottom_bit, carry;
    top_bit = 0;
    bottom_bit = 0;
    carry = 0;
    int tt;

    for(int i = bits - 1; i >= 0; i--) {
      top_bit = get_bit(i);
      bottom_bit = x->get_bit(i);

      tt = top_bit - bottom_bit - carry + 2;
      //printf("debug: top_bit %d, bottom_bit %d, carry %d, tt %d\n",
      //  top_bit, bottom_bit, carry, tt);
      set_bit(i, tt%2);
      carry = tt/2;
      carry = (carry + 1) % 2;
    }
    if (carry != 0) {
      printf("error in sub(): underflow\n");
      print();
      x->print();
      exit(0);
    }
  }

  void div(int den) {
    if(den < 0) {
      printf("error in div(): %d < 0\n", den);
      exit(0);
    }
    int den_len;
    den_len = int_len(den);
    //printf("debug: den_len %d\n", den_len);
    Fixed ff(den_len, 0);
    ff.set(den);
    Fixed rem(this);
    //Fixed rem(whole, frac);
    //rem.set_fixed(this);
    //ff.print();
    //rem.print();
    set_zero();

    //rem.print();
    for(int rx = 0; rx <= (bits - den_len); rx++) {
      //printf("rx %d\n", rx);
      int can_subtract = 1;
      int init_digit = 0;
      if (rx > 0) {
        init_digit = rx-1;
      }
      // compare
      for (int i = init_digit; i < den_len + rx; i++) {
        int top_bit, bottom_bit;
        top_bit = rem.get_bit(i);
        if (rx > i) {
          bottom_bit = 0;
        }
        else {
          bottom_bit = ff.get_bit(i - rx);
        }
        //printf("i %d top %d bottom %d\n", i, top_bit, bottom_bit);
        if(bottom_bit > top_bit) {
          can_subtract = 0;
          break;
        }
        else if (bottom_bit == top_bit) {
          // do nothing
        }
        else if (bottom_bit < top_bit) {
          break;
        }
      }

      //printf("can_subtract %d\n", can_subtract);
      
      // subtract
      if(can_subtract) {
        set_bit(rx + den_len - 1, 1);
        //printf("x\n");
        int tt;
        int top_bit, bottom_bit;
        int carry = 0;
        for (int i = rx + den_len - 1; i >= 0; i--) {
          top_bit = rem.get_bit(i);
          if (i >= rx) {
            bottom_bit = ff.get_bit(i - rx);
          }
          else {
            bottom_bit = 0;
          }
          //rem.print();
          tt = top_bit - bottom_bit - carry + 2;
          rem.set_bit(i, tt%2);
          carry = tt/2;
          carry = (carry + 1) % 2;
        } 

        //rem.print();
        if (carry != 0) {
          printf("error in div(): underflow\n");
          print();
          rem.print();
          ff.print();
          exit(0);
        }
      }
    }
  }

  void save(const char *fn) {
    FILE *ff;
    ff = fopen(fn, "w");
    for(int i=0; i < bytes; i++) {
      fputc(data[i], ff);
    }
    fclose(ff);
  }

  int int_len(int x) {
    if(x < 0) {
      printf("error in int_len(): %d < 0\n", x);
      exit(0);
    }
    int xx = 0x01;
    for(int i = 0; i < 32; i++) {
      if(xx > x) {
        return i;
      }
      xx = xx << 1;
    }
    printf("error in int_len(): overflow");
    exit(0);
  }

  void print() {
    unsigned char bit;
    print_sign();
    for(int i = 0; i < whole; i++) {
      if(i % (6 * 8) == 0) {
        printf("\n");
      }
      else if(i % 8 == 0) {
        printf(" ");
      }
      bit = get_whole_bit(i);
      print_bit(bit);
    }
    if(frac > 0) {
      printf(".");
    }
    for(int i = 0; i < frac; i++) {
      if(i % (6 * 8) == 0) {
        printf("\n");
      }
      else if(i % 8 == 0) {
        printf(" ");
      }
      bit = get_frac_bit(i);
      print_bit(bit);
    }
    printf("\n");
  }

  void print_sign() {
    if (sign == 0) {
      printf("+");
    }
    else {
      printf("-");
    }
  }

  void print_bit(unsigned char bit) {
    if(bit == 0) {
      printf("0");
    }
    else {
      printf("1");
    }
  }

  void shift_bytes_right(unsigned int n) {
    if (n >= bytes) {
      set_zero();
      return;
    }
    if (n == 0) {
      return;
    }

    //printf("shifting %u\n", n);

    int xx = n;
    for(int i = bytes-1; i >= xx; i--) {
      //printf("copy %d to %d\n", i-n, i);
      //printf("i = %d n = %d and %d >= %d\n", i, n, i, n);
      data[i] = data[i - n];
    }
    for(int i = n - 1; i >= 0; i--) {
      //printf("zero %d\n", i);
      data[i] = 0x00;
    }
  }

  unsigned char get_whole_bit(unsigned int index) {
    return get_bit(index);
  }

  unsigned char get_frac_bit(unsigned int index) {
    return get_bit(index+whole);
  }

  unsigned char get_bit(unsigned int index) {
    //if (index >= bits) {
    //  printf("error in get_bit(): index > bits (%d > %d)\n", index, bits);
    //  exit(0);
    //}
    int byte_index;
    int bit_index;
    unsigned char xx;
    byte_index = index / 8;
    bit_index = index % 8;
    xx = data[byte_index];
    xx = xx >> (7 - bit_index);
    xx = xx & 0x01;
    return xx;
  }

  void set_whole_bit(unsigned int index, unsigned char bit) {
    set_bit(index, bit);
  }

  void set_frac_bit(unsigned int index, unsigned char bit) {
    set_bit(index+whole, bit);
  }


  void set_bit(unsigned int index, unsigned char bit) {
    //if (index >= bits) {
    //  printf("error in set_bit(): index > bits (%d > %d)\n", index, bits);
    //  exit(0);
    //}
    int byte_index;
    int bit_index;
    unsigned char xx;
    byte_index = index / 8;
    bit_index = index % 8;
    xx = data[byte_index];
    bit = bit & 0x01;
    //printf("set_bit %u\n", bit);
    if (bit == 0) {
      bit = 0x1;
      bit = bit << (7 - bit_index);
      bit = ~bit;
      xx = xx & bit;
    }
    else {
      bit = bit << (7 - bit_index);
      xx = xx | bit;
    }
    data[byte_index] = xx;
  }



};


