#include "Animation.h"

Animation::Animation(String name) : m_name(name)
{
}

String Animation::getName()
{
  return m_name;
}
