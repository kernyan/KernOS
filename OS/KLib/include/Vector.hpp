#ifndef vector_hpp
#define vector_hpp

#include <common.hpp>
#include <memoryallocator.hpp>
#include <utilities.hpp>

namespace KL
{
   template<typename T>
   class Vector
   {
   public:
      T*     m_Elems;
      size_t m_Size;
      size_t m_Reserved;

      Vector() :
         m_Elems    (nullptr),
         m_Size     (0),
         m_Reserved (0)
      {
      }

      Vector(size_t N)
      {
         m_Elems    = (T*) KM::kmalloc(sizeof(T)*N);
         m_Size     = N;
         m_Reserved = N;
      }

      ~Vector()
      {
         KM::kfree((void*) m_Elems);
         m_Elems = nullptr;
      }

      T& operator[](size_t i)
      {
#ifdef DEBUG
         kassert(i <= m_Size, "Vector range out of bounds\n");
#endif
         return *(m_Elems + i);
      }

      void Pushback(T t)
      {
         if (m_Size == m_Reserved)
         {
            T* NewElems = (T*) KM::kmalloc(sizeof(T) * 2 * m_Size);

            for (size_t i = 0; i < m_Size; ++i) // TODO should use rep stosb for better efficiency, or use memcpy
            {
               *(NewElems + i) = *(m_Elems + i);
            }

            *(NewElems + m_Size++) = t;

            KM::kfree((void*) m_Elems);

            m_Elems     = NewElems;
            m_Reserved *= 2;
         }
         else
         {
            *(m_Elems + m_Size++) = t;
         }
      }

      T& Back()
      {
#ifdef DEBUG
         kassert(m_Size, "Attempting to access empty Vector\n");
#endif
         return *(m_Elems + m_Size - 1);
      }
   };
} // namespace KL


#endif // vector_hpp
