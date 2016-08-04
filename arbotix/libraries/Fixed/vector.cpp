/*

Simple vector math class

*/

#include <Arduino.h>
#include <fix.h>
#include <vector.h>
#include <stdlib.h>
#include <string.h>

// method implementations

vector::vector(){
  for(int i=0; i<3; i++){
    data[i] = 0;
  }
}

vector::vector(fixed_t X, fixed_t Y, fixed_t Z){
  data[0] = X;
  data[1] = Y;
  data[2] = Z;
}

vector::vector(const vector& rhs){
  for(int i=0; i<3; i++){
    data[i] = rhs.data[i];
  }
}

vector::~vector(){
}

// assignment operator
vector& vector::operator=(const vector& rhs){
  for(int i=0; i<3; i++){
    data[i] = rhs.data[i];
  }
}

// addition operator
vector vector::operator+(const vector& rhs){
  vector result;
  for(int i=0;i<3;i++){
    result.data[i] = data[i] + rhs.data[i];
  }
  return result;
}

// in-place addition operator
vector& vector::operator+=(const vector& rhs){
  for(int i=0;i<3;i++){
    data[i] = data[i] + rhs.data[i];
  }
  return *this;
}

// subtraction operator
vector vector::operator-(const vector& rhs){
  vector result;
  for(int i=0;i<3;i++){
    result.data[i] = data[i] - rhs.data[i];
  }
  return result;
}

// in-place subtraction operator
vector& vector::operator-=(const vector& rhs){
  for(int i=0;i<3;i++){
    data[i] = data[i] - rhs.data[i];
  }
  return *this;
}

// vector multiplication operator
vector vector::operator*(const vector& rhs){
  vector result;
  fixed_t a1 = data[0];
  fixed_t a2 = data[1];
  fixed_t a3 = data[2];
  fixed_t b1 = rhs.data[0];
  fixed_t b2 = rhs.data[1];
  fixed_t b3 = rhs.data[2];
  return result;
}

// scalar multiplication
vector vector::operator*(const fixed_t rhs){
  vector result;
  for(int i=0;i<3;i++){
    result.data[i] = FP_Multiply(data[i], rhs);
  }
  return result;
}

vector vector::operator*(const double rhs){
  vector result;
  for(int i=0;i<3;i++){
    result.data[i] = (fixed_t)((data[i] * rhs) + 0.5);
  }
  return result;
}

vector vector::operator*(const float rhs){
  vector result;
  for(int i=0;i<3;i++){
    result.data[i] = (fixed_t)((data[i] * rhs) + 0.5);
  }
  return result;
}

vector vector::operator*(const int rhs){
  vector result;
  for(int i=0;i<3;i++){
    result.data[i] = (fixed_t)((data[i] * rhs));
  }
  return result;
}


// scalar division
vector vector::operator/(const fixed_t rhs){
  vector result;
  for(int i=0;i<3;i++){
    result.data[i] = FP_Divide(data[i], rhs);
  }
  return result;
}

vector vector::operator/(const double rhs){
  vector result;
  for(int i=0;i<3;i++){
    result.data[i] = (fixed_t)((data[i] / rhs) + 0.5);
  }
  return result;
}

vector vector::operator/(const float rhs){
  vector result;
  for(int i=0;i<3;i++){
    result.data[i] = (fixed_t)((data[i] / rhs) + 0.5);
  }
  return result;
}

vector vector::operator/(const int rhs){
  vector result;
  for(int i=0;i<3;i++){
    result.data[i] = (fixed_t)((data[i] / rhs) + 0.5);
  }
  return result;
}


fixed_t vector::operator()(const unsigned& element){
  return this->data[element];
}

const fixed_t vector::operator()(const unsigned& element) const{
  return this->data[element];
}

void vector::format(fixed_t num, char *str){
  char negative = ' ';
  if(num < 0){
    negative = '-';
    num = FP_ABS(num);
  }
  int temp = (int)(((100 * num) + 16384) / FP_ONE);
  int whole = (temp / 100);
  int frac = temp % 100;
  sprintf(str, " %c%1d.%02d", negative, whole, frac);
}

void vector::print(){
  float f;
  char str[16];
  for(int i=0;i<3;i++){
    fixed_t num = data[i];
    format(num, str);
    Serial.print(str);
  }
  Serial.println('\n');
}

void vector::set(const unsigned& element, fixed_t value){
  data[element] = value;
}

void vector::set(const unsigned& element, float value){
  data[element] = ftok(value);
}

void vector::set(const unsigned& element, double value){
  data[element] = ftok(value);
}
