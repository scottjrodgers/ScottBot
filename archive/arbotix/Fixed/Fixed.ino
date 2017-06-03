#include "fix.h"

void setup(void) {
  volatile fixed_t fix1, fix2, fix3, fix4, fix5;
  volatile float float1 = 2.33f;
  char s[12];

  Serial.begin(9600);
  delay(1000);

  //0.66 + 2.33
  fix1 = ftok(0.66);
  fix2 = ftok(float1);
  fix3 = fix1 + fix2;
  fix4 = ftok(2.0);
  fix5 = ftok(3.0);

  FP_FixedToString(fix3, s);
  Serial.print("0.66 + 2.33 = "); Serial.println(s);

  //2.33 - 0.66
  fix3 = fix2 - fix1;
  FP_FixedToString(fix3, s);
  Serial.print("2.33 - 0.66 = "); Serial.println(s);

  //2.33*0.66
  fix3 = FP_Multiply(fix2, fix1);
  FP_FixedToString(fix3, s);
  Serial.print("2.33 * 0.66 = "); Serial.println(s);

  //2.33/0.66
  fix3 = FP_Divide(fix2, fix1);
  FP_FixedToString(fix3, s);
  Serial.print("2.33 / 0.66 = "); Serial.println(s);

  //2.33/0.66
  fix3 = FP_Divide(fix1, fix2);
  FP_FixedToString(fix3, s);
  Serial.print("0.66 / 2.33 = "); Serial.println(s);

  //2.0 * 3.0
  fix3 = FP_Multiply(fix4, fix5);
  FP_FixedToString(fix3, s);
  Serial.print("0.66 / 2.33 = "); Serial.println(s);

  //square root of 30.25
  fix1 = FP_Sqrt(ftok(30.25));
  float1 = ktof(fix1);
  Serial.print("sqrt(30.25) = "); Serial.println(float1);

  for(int angle = 0; angle <= 360; angle += 15){
    fix1 = FP_Sin(ftok((float)angle));
    FP_FixedToString(fix1, s);
    Serial.print("sine(");
    Serial.print(angle);
    Serial.print(") = "); Serial.println(s);
  }

  for(int angle = 0; angle <= 360; angle += 15){
    fix1 = FP_Cos(ftok((float)angle));
    FP_FixedToString(fix1, s);
    Serial.print("cosine(");
    Serial.print(angle);
    Serial.print(") = "); Serial.println(s);
  }
}

void loop(void) { }
