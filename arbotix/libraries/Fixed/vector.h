/***********************************************************************
* Vector math for Arbotix
* (C) Rodgers Robotics
*
* Note:  Only handles 3-element vectors
*
************************************************************************/

#ifndef VECTOR_H
#define VECTOR_H

#include "Arduino.h"

class vector
{
  private:
    fixed_t data[3];
    void format(fixed_t val, char *str);

  public:
    vector();
    vector(fixed_t X, fixed_t Y, fixed_t Z);
    vector(const vector& rhs);

    ~vector();

    vector& operator=(const vector& rhs);

    vector operator+(const vector& rhs);
    vector& operator+=(const vector& rhs);

    vector operator-(const vector& rhs);
    vector& operator-=(const vector& rhs);

    vector operator*(const vector& rhs);

    vector operator*(const fixed_t rhs);
    vector operator*(const float rhs);
    vector operator*(const double rhs);
    vector operator*(const int rhs);

    vector operator/(const fixed_t rhs);
    vector operator/(const float rhs);
    vector operator/(const double rhs);
    vector operator/(const int rhs);

    vector norm();

    fixed_t operator()(const unsigned& element);
    const fixed_t operator()(const unsigned& element) const;

    void set(const unsigned& element, fixed_t value);
    void set(const unsigned& element, float value);
    void set(const unsigned& element, double value);

    void print();

};

#endif
