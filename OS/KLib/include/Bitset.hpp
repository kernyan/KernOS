#ifndef bitset_h
#define bitset_h

#include <common.hpp>

namespace KL
{
   template<size_t N>
   class Bitset
   {
      char m_Bits[N/8 + 1];

   public:
      inline bool Test(const size_t i)
      {
         return (m_Bits[i/8] >> (i%8)) & 1;
      }

      inline void Set(const size_t i)
      {
         char& Byte = m_Bits[i/8];
         Byte |= (1 << (i%8));
      }

      void Reset(const size_t i)
      {
         char& Byte = m_Bits[i/8];
         Byte &= !(1 << (i%8));
      }

      inline void Flip(const size_t i)
      {
         if (Test(i))
            Reset(i);
         else
            Set(i);
      }
   };

} // namespace KL


#endif // bitset_h
