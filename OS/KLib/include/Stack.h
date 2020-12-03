#ifndef stack_h
#define stach_h

#include <common.h>

namespace KL
{


const size_t CAPACITY = 100;

template <typename T, size_t N = CAPACITY>
class Stack
{
  T elem[N];
  int Pos {-1};

public:

  Stack()
  {
    //memset (elem, 0, sizeof(T) * N);
  }

  void insert(T Val)
  {
    elem[++Pos] = Val;
  }

  T top()
  {
    return elem[Pos];
  }

  void pop()
  {
    --Pos;
  }

  template <typename T2>
  Stack& operator=(const Stack<T2> &Rhs)
  {
    //memset(elem, 0, sizeof(T) * N);

    for (size_t i = 0; i <= Rhs.Pos; ++i) {
      insert (Rhs.elem[i]);
    }
  }

  template<typename, size_t>
  friend class Stack;
};





} // namespace KL


#endif // stack_h
