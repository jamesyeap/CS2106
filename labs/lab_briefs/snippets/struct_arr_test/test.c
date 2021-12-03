#include <stdio.h>

struct TEST {
  int attrA;
  int attrB;
}

struct TEST testArr**;

int main()
{
  testArr = malloc(sizeof(struct TEST) * 5);

  // test modify
  testArr[1].attrA = 2;

  return 0;
}
