// Copyright (c) 2010 Olivier Crave. All rights reserved.

#include "Body.h"

bool Body::isTouched(const oboylib::Vector2 pos) const
{
  if ((mPos - pos).magnitude() < mRadius)
    return true;
  else
    return false;
}