#include "map_constrain.h"

double map(double value, double min_val, double max_val, double new_min_val,
           double new_max_val) {
  return (value - min_val) * (new_max_val - new_min_val) / (max_val - min_val) +
         new_min_val;
}

double constrain(double value, double min_value, double max_value) {
  if (value < min_value) {
    return min_value;
  } else if (value > max_value) {
    return max_value;
  } else {
    return value;
  }
}

int module(int expression) {
  if (expression < 0) {
    return -expression;
  } else {
    return expression;
  }
}

int previous_element(int value, int last_value) {
  int prev_e = value - 1;
  if (prev_e < 0) {
    return  last_value;
  } else {
    return prev_e;
  }
}