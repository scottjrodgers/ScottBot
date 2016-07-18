#include "fix.h"
#include "matrix.h"
#include "Arduino.h"

void setup(void) {
  matrix X, tmp;
  matrix Ans;
  matrix A(2);
  matrix B(2,2);
  matrix C(2,3);
  matrix V(3,1);
  matrix D(2,2);
  matrix E(2,2);

  Serial.begin(9600);
  delay(1000);

  Serial.println("Matrix X:");
  X.print();

  Serial.println("Matrix A:");
  A.print();

  Serial.println("Matrix B:");
  B.print();

  Serial.println("Matrix C:");
  C.print();

  Serial.println("Vector V:");
  V.print();

  Serial.println("------------------------------------------------------------\n");

  // update Matrix 1
  A.set(0, 1, FP_ONE * 3);
  A.set(1, 0, 2.5);
  A.set(1, 1, fixed_t(FP_ONE * 0.75));

  // update Matrix 2
  B.set(0,0, 4.0);
  B.set(0,1,-0.5);
  B.set(1,0, 0.9);
  B.set(1,1, 1.1);

  // update Matrix 3
  C.set(0,0, 1.0);
  C.set(0,1, 0.0);
  C.set(0,2, 0.9);
  C.set(1,0, 0.1);
  C.set(1,1, -1.4);
  C.set(1,2, 0.125);

  // update vector
  V.set(0,0, 2.0);
  V.set(1,0, 3.0);
  V.set(2,0, 4.0);
  
  for(int i=0;i<2;i++){
    for(int j=0;j<2;j++){
      D.set(i,j, 2.0);
      E.set(i,j, 3.0);
    }
  }

  Serial.println("Matrix A -- updated:");
  A.print();

  Serial.println("Matrix B -- updated:");
  B.print();

  Serial.println("Matrix C -- updated:");
  C.print();

  Serial.println("Vector V -- updated:");
  V.print();

  Serial.println("Matrix D -- updated:");
  D.print();

  Serial.println("Matrix E -- updated:");
  E.print();

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

  Serial.println("Scalar Multiply: Ans = X * 2.0");
  Ans = X * (FP_ONE * 2);
  Ans.print();

  Serial.println("Scalar Multiply: Ans = X * 3.0");
  Ans = X * 3.0;
  Ans.print();

  Serial.println("------------------------------------------------------------\n");

  Serial.println("Transpose: Ans = t(C)");
  Ans = C.t();
  Ans.print();

  Serial.println("Transpose: Ans = t(V)");
  Ans = V.t();
  Ans.print();

  Serial.println("------------------------------------------------------------\n");

  Serial.println("Matrix Multiply: Ans = D * E");
  Ans = D * E;
  Ans.print();

  Serial.println("Matrix Multiply: Ans = A * B");
  Ans = A * B;
  Ans.print();

  Serial.println("Matrix Multiply: Ans = B * A");
  Ans = B * A;
  Ans.print();

  Serial.println("Matrix Multiply: Ans = C * V");
  Ans = C * V;
  Ans.print();

  Serial.println("------------------------------------------------------------\n");

  Serial.println("Matrix Multiply: Ans = V * t(V)");
  Ans = V * V.t();
  Ans.print();

  Serial.println("Matrix Multiply: Ans = t(V) * V");
  Ans = V.t() * V;
  Ans.print();

  Serial.println("Matrix Multiply: Ans = C * t(C)");
  Ans = C * C.t();
  Ans.print();

  Serial.println("Matrix Multiply: Ans = t(C) * C");
  Ans = C.t() * C;
  Ans.print();

  Serial.println("------------------------------------------------------------\n");

  Serial.println("Get cell: X(0,0)");
  Serial.println(X(0,0));
  Serial.println("Get cell: X(0,1)");
  Serial.println(X(0,1));
  Serial.println("Get cell: X(1,0)");
  Serial.println(X(1,0));
  Serial.println("Get cell: X(1,1)");
  Serial.println(X(1,1));

  Serial.println("------------------------------------------------------------\n");

  Serial.println("nrows(C)");
  Serial.println(C.nrows());
  Serial.println("ncols(C)");
  Serial.println(C.ncols());
/* */
}

void loop(void) { }
