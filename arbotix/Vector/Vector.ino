#include <fix.h>
#include <vector.h>
#include "Arduino.h"

void setup(void) {
  vector tmp;
  vector Ans;
  vector A(ftok(1.1), ftok(2.2), ftok(3.3));
  vector B(ftok(-2.0), ftok(0), ftok(1.7));
  vector C(FP_ONE, FP_ONE * 2, 0);

  Serial.begin(9600);
  delay(1000);

  Serial.println("vector A:");
  A.print();

  Serial.println("vector B:");
  B.print();

  Serial.println("vector C:");
  C.print();

  Serial.println("------------------------------------------------------------\n");

  Serial.println("Assignment: Ans = A");
  Ans = A;
  Ans.print();

  Serial.println("Addition: Ans = A + B");
  Ans = A + B;
  Ans.print();

  Serial.println("Addition: A += B");
  Ans = A;
  Ans += B;
  Ans.print();

  Serial.println("------------------------------------------------------------\n");

  Serial.println("Subtraction: Ans = A - B");
  Ans = A - B;
  Ans.print();

  Serial.println("Subtraction: Ans = B - A");
  Ans = B - A;
  Ans.print();

  Serial.println("Subtraction: A -= B");
  Ans = A;
  Ans -= B;
  Ans.print();

  Serial.println("------------------------------------------------------------\n");

  Serial.println("Scalar Multiply: Ans = A * 2.0");
  Ans = A * (FP_ONE * 2);
  Ans.print();

  Serial.println("Scalar Multiply: Ans = A * 3.0");
  Ans = A * 3.0;
  Ans.print();

  Serial.println("------------------------------------------------------------\n");

  Serial.println("Scalar Divide: Ans = A / 0.5");
  Ans = A / (FP_ONE / 2);
  Ans.print();

  Serial.println("Scalar Multiply: Ans = A / 3");
  Ans = A / 3;
  Ans.print();

  Serial.println("------------------------------------------------------------\n");

  Serial.println("Cross Product : Ans = A * B");
  // Ans = D * E;
  // Ans.print();

  Serial.println("------------------------------------------------------------\n");

  Serial.println("Get element A(0)");
  Serial.println(A(0));
  Serial.println("Get element A(1)");
  Serial.println(A(1));
  Serial.println("Get element A(2)");
  Serial.println(A(2));
}

void loop(void) { }
