/*

Simple matrix math class

*/

#include "matrix.h"
#include "fix.h"
#include "Arduino.h"
#include <stdlib.h>
#include <string.h>

#define idx(r,c) (r*C+c)

// method implementations

matrix::matrix(unsigned N){
  R = N;
  C = N;
  for(int i=0; i<R; i++){
    for(int j=0; j<C; j++){
      data[idx(i,j)] = 0;
    }
  }
  for(int i=0; i<N; i++){
    data[idx(i,i)] = FP_ONE;
  }
}

matrix::matrix(){
  unsigned N = 4;
  R = N;
  C = N;
  for(int i=0; i<R; i++){
    for(int j=0; j<C; j++){
      data[idx(i,j)] = 0;
    }
  }
  for(int i=0; i<N; i++){
    data[idx(i,i)] = FP_ONE;
  }
}

matrix::matrix(unsigned r, unsigned c){
  R = r;
  C = c;
  for(int i=0; i<R; i++){
    for(int j=0; j<C; j++){
      data[idx(i,j)] = 0;
    }
  }
}

matrix::matrix(const matrix& rhs){
  R = rhs.R;
  C = rhs.C;
  for(int i=0; i<R * C; i++){
    data[i] = rhs.data[i];
  }
}

matrix::~matrix(){
}

// assignment operator
matrix& matrix::operator=(const matrix& rhs){
  R = rhs.R;
  C = rhs.C;
  for(int i=0;i<R*C;i++){
    data[i] = rhs.data[i];
  }
}

// addition operator
matrix matrix::operator+(const matrix& rhs){
  matrix result(R,C);
  if(R != rhs.R || C != rhs.C){
    Serial.println("Error Row and Column size mismatch");
    return result;
  }
  for(int i=0;i<R*C;i++){
    result.data[i] = data[i] + rhs.data[i];
  }
  return result;
}

// in-place addition operator
matrix& matrix::operator+=(const matrix& rhs){
  if(R != rhs.R || C != rhs.C){
    Serial.println("Error Row and Column size mismatch");
    return *this;
  }
  for(int i=0;i<R*C;i++){
    data[i] = data[i] + rhs.data[i];
  }
  return *this;
}

// subtraction operator
matrix matrix::operator-(const matrix& rhs){
  matrix result(R,C);
  if(R != rhs.R || C != rhs.C){
    Serial.println("Error Row and Column size mismatch");
    return result;
  }
  for(int i=0;i<R*C;i++){
    result.data[i] = data[i] - rhs.data[i];
  }
  return result;
}

// in-place subtraction operator
matrix& matrix::operator-=(const matrix& rhs){
  if(R != rhs.R || C != rhs.C){
    Serial.println("Error Row and Column size mismatch");
    return *this;
  }
  for(int i=0;i<R*C;i++){
    data[i] = data[i] - rhs.data[i];
  }
  return *this;
}

// matrix multiplication operator
matrix matrix::operator*(const matrix& rhs){
  matrix result(R, rhs.C);
  if(C != rhs.R){
    Serial.println("Error Matrix Mult ncols(A) != nrows(B)");
    return result;
  }
  
  char str[16];
  /*
  Serial.println(R);
  Serial.println(C);
  Serial.println(rhs.R);
  Serial.println(rhs.C);
  */
  for(int i=0;i<R;i++){
    for(int j=0;j<rhs.C;j++){
      for(int k=0;k<C;k++){
        fixed_t left_val = data[idx(i,k)];
        fixed_t right_val = rhs(k,j);
        fixed_t product = FP_Multiply(left_val, right_val);
        
        result.set(i,j, result(i,j) + product);
        /*
        Serial.print("(");
        Serial.print(i);
        Serial.print(",");
        Serial.print(j);
        Serial.print(",");
        Serial.print(k);
        Serial.print(") += ");
        format(left_val, str);
        Serial.print(str);       
        Serial.print(" * ");
        format(right_val, str);
        Serial.print(str);
        Serial.print(" (");
        format(product, str);
        Serial.print(str);
        Serial.print(") ");        
        Serial.print(" -> ");
        format(result.data[idx(i,j)], str);
        Serial.println(str);
        */
      }
    }
  }
  return result;
}

// scalar multiplication
matrix matrix::operator*(const fixed_t rhs){
  matrix result(R,C);
  for(int i=0;i<R*C;i++){
    result.data[i] = FP_Multiply(data[i], rhs);
  }
  return result;
}

matrix matrix::operator*(const double rhs){
  matrix result(R,C);
  for(int i=0;i<R*C;i++){
    fixed_t val = data[i];
    fixed_t adj = (fixed_t)((val * rhs) + 0.5);
    result.data[i] = (fixed_t)((data[i] * rhs) + 0.5);
  }
  return result;
}

matrix matrix::operator*(const float rhs){
  matrix result(R,C);
  for(int i=0;i<R*C;i++){
    result.data[i] = (int)((data[i] * rhs) + 0.5);
  }
  return result;
}

fixed_t matrix::operator()(const unsigned& row, const unsigned& col){
  return this->data[row * this->C + col];
}

const fixed_t matrix::operator()(const unsigned& row, const unsigned& col) const{
  return this->data[row * this->C + col];
}

matrix matrix::t(){
  matrix result(C,R);
  for(int i=0;i<R;i++){
    for(int j=0;j<C;j++){
      result.set(j,i, data[idx(i,j)]);
    }
  }
  return result;
}

matrix matrix::inv(){
  Serial.println("Can't invert yet");
  return *this;
}

unsigned matrix::nrows() const{
  return R;
}

unsigned matrix::ncols() const{
  return C;
}

void matrix::format(fixed_t num, char *str){
  char negative = ' ';
  if(num < 0){
    negative = '-';
    num = FP_ABS(num);
  }
  int whole = (num / FP_ONE);
  int frac = (int)(((100 * FP_FRAC_PART(FP_ABS(num))) + 16384) / FP_ONE);
  sprintf(str, " %c%1d.%02d", negative, whole, frac);
}

void matrix::print(){
  float f;
  char str[16];
  for(int i=0;i<R;i++){
    for(int j=0;j<C;j++){
      fixed_t num = data[idx(i,j)];
      format(num, str);
      Serial.print(str);
    }
    Serial.println();
  }
  Serial.println();
}

void matrix::set(const unsigned& row, const unsigned& col, fixed_t value){
  data[idx(row, col)] = value;
}

void matrix::set(const unsigned& row, const unsigned& col, float value){
  data[idx(row, col)] = ftok(value);
}

void matrix::set(const unsigned& row, const unsigned& col, double value){
  data[idx(row, col)] = ftok(value);
}
