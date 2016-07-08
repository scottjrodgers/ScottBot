#include "fix.h"

void setup(void) {
  volatile int32_t fix1, fix2, fix3;
  volatile float float1 = 2.33f;
  char s[12];

  Serial.begin(9600);
  delay(1000);

  //0.66 + 2.33
  fix1 = ftok(0.66);
  fix2 = ftok(float1);
  fix3 = fix1 + fix2;
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

  //square root of 30.25
  fix1 = FP_Sqrt(ftok(30.25));
  float1 = ktof(fix1);
  Serial.print("sqrt(30.25) = "); Serial.println(float1);



  //sine of 45 degrees
  // fix1 = FP_DegToRad(FP_FloatToFixed(45.0));
  // fix2 = FP_Sin(fix1);
  // FP_FixedToString(fix2, s);
  // Serial.print("Sin(45.0) = "); Serial.println(s);
}

void loop(void) { }
