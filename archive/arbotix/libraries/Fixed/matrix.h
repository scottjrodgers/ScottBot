/***********************************************************************
* Matrix math for Arbotix
* (C) Rodgers Robotics
*
* Note:  Only handles matrices up to 4x4
*
************************************************************************/

#ifndef MATRIX_H
#define MATRIX_H

#include "Arduino.h"

class matrix
{
  private:
    unsigned R;
    unsigned C;
    fixed_t data[16];
    void format(fixed_t val, char *str);

  public:
    matrix();
    matrix(unsigned N);
    matrix(unsigned r, unsigned c);
    matrix(const matrix& rhs);

    ~matrix();

    matrix& operator=(const matrix& rhs);

    matrix operator+(const matrix& rhs);
    matrix& operator+=(const matrix& rhs);

    matrix operator-(const matrix& rhs);
    matrix& operator-=(const matrix& rhs);

    matrix operator*(const matrix& rhs);

    vector operator*(const vector& rhs);

    matrix operator*(const fixed_t rhs);
    matrix operator*(const float rhs);
    matrix operator*(const double rhs);

    matrix t();
    matrix inv();

    fixed_t operator()(const unsigned& row, const unsigned& col);
    const fixed_t operator()(const unsigned& row, const unsigned& col) const;

    void set(const unsigned& row, const unsigned& col, fixed_t value);
    void set(const unsigned& row, const unsigned& col, float value);
    void set(const unsigned& row, const unsigned& col, double value);

    unsigned nrows() const;
    unsigned ncols() const;

    void print();

    void make_rot_X(fixed_t angle);
    void make_rot_Y(fixed_t angle);
    void make_rot_Z(fixed_t angle);

    void make_translate(fixed_t x, fixed_t y, fixed_t z);
    void add_translation(fixed_t x, fixed_t y, fixed_t z);

    void make_translate(const matrix& rhs);
    void add_translation(const matrix& rhs);
};

#endif //FIX_H
