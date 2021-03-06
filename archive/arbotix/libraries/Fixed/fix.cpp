/*
 * The ideas and algorithms have been cherry-picked from a large number
 * of previous implementations available on the Internet, and from the
 * AVR GCC lib sources.
 * Copyright (c) 2002  Michael Stumpf  <mistumpf@de.pepperl-fuchs.com>
 * Copyright (c) 2006  Dmitry Xmelkov
 * Copyright (C) 2012-2015 Free Software Foundation, Inc.
 * Contributed by Sean D'Epagnier  (sean@depagnier.com)
 * Georg-Johann Lay (avr@gjlay.de)
 * All rights reserved.
 * Maximilan Rosenblattl, Andreas Wolf 2007-02-07
 * Copyright (c) 2010-2012 Ivan Voras <ivoras@freebsd.org>
 * Copyright (c) 2012 Tim Hartrick <tim@edgecast.com>

 * Modified to work on ArbotiX-M robocontroller by Scott J. Rodgers, (c) 2016
 * Some of the implementations mentioned above replaced by my custom code

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in
 the documentation and/or other materials provided with the
 distribution.
 * Neither the name of the copyright holders nor the names of
 contributors may be used to endorse or promote products derived
 from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/
//fix.cpp
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include "fix.h"
#include "sine_data.h"
#include "arcsine_data.h"
#include "Arduino.h"

#define MAX_STRING_SIZE 8

//atol function ignores sign
int32_t _atol(const char* s) {
  int32_t v=0;

  //ignore whitespace
  while (*s == ' ' || (uint16_t)(*s - 9) < 5u)
    ++s;
  //skip over sign
  if (*s == '-' || *s == '+')
    ++s;
  //convert ascii to number
  while ((uint16_t)(*s - '0') < 10u) {
    v = v*10 + *s - '0';
    ++s;
  }
  return v;
}

//ltoa function ignores sign
char *_ltoa(int32_t n, char *s) {
  char temp[8];
  uint8_t i = 0;

  //construct backward string of number
  while ((uint32_t)n > 0) {
    temp[i++] = ((uint32_t)n%10) + '0';
    n = ((uint32_t)n)/10;
  }
  //reverse string
  while (i > 0) {
    *s++ = temp[--i];
  }
  //terminate string
  *s = 0;
  return s;
}

//basic string copy n
static inline void _strcpy(char *d, const char *s) {
  uint8_t n=0;

  while (*s != '\0') {
    if (n++ >= MAX_STRING_SIZE)
      return; //destination @ max size
    *d++ = *s++;
  }
}

//basic string concatenation
void _concat(char *d, char *s) {
  uint8_t n=0;

  //find end of destination
  while(*d) {
    d++;
  }
  //copy source to destination
  while(*s && n<MAX_STRING_SIZE) {
    *d++ = *s++;
    n++;
  }
  //terminate destination
  *d = '\0';
}

fixed_t FP_StringToFixed(char *s) {
  fixed_t f, fpw, fpf, bit, r[15] = {
    0x2faf080, 0x17d7840, 0xbebc20, 0x5f5e10, 0x02faf08, 0x017d784, 0x0bebc2, 0x05f5e1,
    0x002faf1, 0x0017d78, 0x00bebc, 0x005f5e, 0x0002faf, 0x00017d8, 0x000bec //0x0005f6
  };
  uint8_t sign, i;
  char *p=s, temp[9] = "00000000";

  sign = 0;
  //separate whole & fraction portions
  while (*p != '.') {
    //check for negative sign
    if (*p == '-')
      sign = 1;
    //no decimal found, return integer as fixed point
    if (*p == '\0')
      return sign ? -(_atol(s)<<FP_FBITS) : (_atol(s)<<FP_FBITS);
    p++;
  }

  //whole part
  *p = '\0';
  fpw = (_atol(s)<<FP_FBITS);

  //pad fraction part with trailing zeros
  _strcpy(temp, (p + 1));
  //get fraction
  f = _atol(temp);
  //re-insert decimal point
  *p = '.';

  fpf = 0;
  bit = 0x4000;
  //convert base10 fraction to fixed point base2
  for (i=0; i<15; i++) {
    if (f - r[i] > 0) {
      f -= r[i];
      fpf += bit;
    }
    bit >>= 1;
  }

  //join fixed point whole and fractional parts
  return sign ? -(fpw + fpf) : (fpw + fpf);
}

void FP_FixedToString(fixed_t f, char *s) {
  fixed_t fp, bit=0x4000, r[16] = { 50000, 25000, 12500, 6250, 3125, 1563, 781, 391, 195, 98, 49, 24, 12, 6, 3 };
  int32_t d[5] = { 10000, 1000, 100, 10 };
  char *p=s, *sf, temp[12];
  uint8_t i;

  //zero?
  if (f == 0) {
    *p++ = '0';
    *p = 0;
    return;
  }

  //negate?
  if (f < 0) {
    *p++ = '-';
    f = -f;
  }

  //get whole part
  fp = ktoi(f);
  if (fp != 0)
    p = _ltoa(fp, p);
  else
    *p++ = '0';

  //get fractional part
  fp = FP_FRAC_PART(f);
  if (fp == 0) {
    *p = 0;   //terminate string
    return;
  }
  *p++ = '.'; //add decimal to end of s
  *p = 0;     //terminate string

  f = 0;
  //convert fraction base 2 to base 10
  for (i=0; i<15; i++) {
    if (fp & bit)
      f += r[i];
    bit >>= 1;
  }
  //temporary string storage space
  sf = temp;
  sf = ltoa(f, sf, 10);

  // if needed, add leading zeros to fractional portion
  for (i=0; i<4; i++) {
    if (f < d[i]) {
      *p++ = '0';
      *p = '\0';
    } else
      break;
  }

  //combine whole & fractional parts
  _concat(s, sf);
}

fixed_t FP_Multiply(fixed_t a, fixed_t b){
  uint32_t abs_a, ah, al;
  uint32_t abs_b, bh, bl;
  uint32_t prod_c, ch, cl;
  uint32_t prod_d, dh, dl;
  uint32_t prod_e, eh, el;
  uint32_t prod_f, fh, fl;
  uint32_t sum_0, sum_1;
  int32_t sign = 1;

  if(a < 0){
    sign *= -1;
  }
  if(b < 0){
    sign *= -1;
  }

  // Serial.print("a = "); Serial.println(a);
  // Serial.print("b = "); Serial.println(b);

  abs_a = (int32_t) FP_ABS(a);
  abs_b = (int32_t) FP_ABS(b);

  // Serial.print("abs_a = "); Serial.println(abs_a);
  // Serial.print("abs_b = "); Serial.println(abs_b);

  al = abs_a & FP_FMASK;
  ah = abs_a >> FP_FBITS;
  bl = abs_b & FP_FMASK;
  bh = abs_b >> FP_FBITS;

  // Serial.print("ah = "); Serial.println(ah);
  // Serial.print("al = "); Serial.println(al);
  // Serial.print("bh = "); Serial.println(bh);
  // Serial.print("bl = "); Serial.println(bl);


  //prod_c = al * bl;
  prod_c = al * bl;
  prod_d = al * bh;
  prod_e = ah * bl;
  prod_f = ah * bh;

  // Serial.print("prod_c = "); Serial.println(prod_c);
  // Serial.print("prod_d = "); Serial.println(prod_d);
  // Serial.print("prod_e = "); Serial.println(prod_e);
  // Serial.print("prod_f = "); Serial.println(prod_f);

  cl = prod_c & FP_FMASK;
  ch = prod_c >> FP_FBITS;
  dl = prod_d & FP_FMASK;
  dh = prod_d >> FP_FBITS;
  el = prod_e & FP_FMASK;
  eh = prod_e >> FP_FBITS;
  fl = prod_f & FP_FMASK;
  fh = prod_f >> FP_FBITS;

  // Serial.print("ch = "); Serial.println(ch);
  // Serial.print("cl = "); Serial.println(cl);
  // Serial.print("dh = "); Serial.println(dh);
  // Serial.print("dl = "); Serial.println(dl);
  // Serial.print("eh = "); Serial.println(eh);
  // Serial.print("el = "); Serial.println(el);
  // Serial.print("fh = "); Serial.println(fh);
  // Serial.print("fl = "); Serial.println(fl);


  sum_0 = dl + ch + el;
  sum_1 = dh + fl + eh;

  // Serial.print("sum_0 = "); Serial.println(sum_0);
  // Serial.print("sum_1 = "); Serial.println(sum_1);

  fixed_t prod = (fixed_t) (sum_0 + (sum_1 << FP_FBITS));

  // Serial.print("prod = "); Serial.println(prod);

  if(sign < 1){
    return -prod;
  }
  return prod;
}


fixed_t FP_Divide(fixed_t a, fixed_t b){
  int n_shift = 0;
  int d_shift;
  int sign = 1;
  uint32_t numerator, denominator, quotient;

  if(a < 0){
    sign *= -1;
  }
  if(b < 0){
    sign *= -1;
  }

  //Serial.print("sign = "); Serial.println(sign);

  numerator = (uint32_t)FP_ABS(a);
  denominator = (uint32_t)FP_ABS(b);
  /*
  Serial.print("n_shift = "); Serial.println(n_shift);
  Serial.print("numerator = "); Serial.println(numerator);
  Serial.print("denominator = "); Serial.println(denominator);
  */
  uint32_t cond1 = (numerator & 0xC0000000) == 0;
  uint32_t cond2 = (n_shift < FP_FBITS);
  //Serial.print("cond1 = "); Serial.println(cond1);
  //Serial.print("cond2 = "); Serial.println(cond2);

  //while((numerator & 0xC0000000 == 0) && (n_shift < FP_FBITS)){
  while(cond1 && cond2){
    n_shift++;
    numerator = numerator << 1;
    //Serial.print("n_shift = "); Serial.println(n_shift);
    //Serial.print("numerator = "); Serial.println(numerator);
    //Serial.print("denominator = "); Serial.println(denominator);
    cond1 = (numerator & 0xC0000000) == 0;
    cond2 = (n_shift < FP_FBITS);
    //Serial.print("cond1 = "); Serial.println(cond1);
    //Serial.print("cond2 = "); Serial.println(cond2);
  }

  d_shift = FP_FBITS - n_shift;
  //Serial.print("d_shift = "); Serial.println(d_shift);

  if(d_shift > 0){
    denominator = denominator >> d_shift;
  }
  //Serial.print("denominator = "); Serial.println(denominator);

  quotient = numerator / denominator;

  if(sign < 0){
    return (fixed_t)(-quotient);
  }
  return (fixed_t) quotient;
}

//Difference from ISO/IEC DTR 18037: using an uint8_t as second parameter according to microcontroller register size and maximum possible value
int32_t FP_Round(int32_t f, uint8_t n) {
  n = FP_FBITS - n;
  if (f >= 0)
    return (f&(0xFFFFFFFF<<n)) + ((f&(1<<(n - 1)))<<1);
  else
    return (f&(0xFFFFFFFF<<n)) - ((f&(1<<(n - 1)))<<1);
}


fixed_t _FP_SquareRoot(fixed_t val, int32_t Q) {
  fixed_t sval = 0;

  //convert Q to even
  if (Q & 0x01) {
    Q -= 1;
    val >>= 1;
  }
  //integer square root math
  for (uint8_t i=0; i<=30; i+=2) {
    if ((0x40000001>>i) + sval <= val) {
      val -= (0x40000001>>i) + sval;
      sval = (sval>>1) | (0x40000001>>i);
    } else {
      sval = sval>>1;
    }
  }
  if (sval < val)
    ++sval;
  //this is the square root in Q format
  sval <<= (Q)/2;
  //convert the square root to Q15 format
  if (Q < 15)
    return(sval<<(15 - Q));
  else
    return(sval>>(Q - 15));
}

/*****************************************************************************
* lookup a trig value from pre-calculated header files
*****************************************************************************/
fixed_t trig_lookup(float x, const int N, const float cutoffs[],
                    const int counts[], const fixed_t* values[]){
  float cmin = 0.0;
  float cmax = 0.0;
  int segment = 0;
  int count;
  float index;
  int idx_1, idx_2;
  fixed_t value_1, value_2;
  fixed_t result = (fixed_t)0;

  while(x > cutoffs[segment] && segment < N){
    cmin = cutoffs[segment];
    segment += 1;
  }
  if(segment < N){
    cmax = cutoffs[segment];
    count = counts[segment];
    index = count * (x - cmin) / (cmax - cmin);
    idx_1 = (int)floor(index);
    idx_2 = (int)ceil(index);
    value_1 = values[segment][idx_1];
    value_2 = values[segment][idx_2];

    result = (fixed_t)round(value_1 + (value_2 - value_1) * (index - idx_1));
  }
  return result;
}


/*****************************************************************************
* Computes the sine of a fixed point value (in degrees)
*****************************************************************************/
fixed_t FP_Sin(fixed_t src_angle){
  float f_angle = ktof((float)src_angle);
  float angle;
  float sign;
  fixed_t result;

  f_angle = f_angle - 360 * floor(f_angle / 360.0);

  if(f_angle < 0.0 || f_angle > 360.0){
    // error condition
    Serial.print("Error! Sine Angle should be between 0 and 360 but was:");
    Serial.println(f_angle);
  }

  if(f_angle < 90.0){
    angle = f_angle;
    sign = 1.0;
  }
  else if(f_angle < 180.0){
    angle = 180.0 - f_angle;
    sign = 1.0;
  }
  else if(f_angle < 270.0){
    angle = f_angle - 180.0;
    sign = -1.0;
  }
  else if(f_angle < 360.0){
    angle = 360.0 - f_angle;
    sign = -1.0;
  }
  else{
    angle = 0.0;
    sign = 0.0;
  }

  result = sign * trig_lookup(angle, sine_N, sine_cutoffs,
                              sine_counts, sine_values);
  return result;
}
