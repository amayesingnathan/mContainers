#pragma once

#include <cstdint> 
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

#include <sstream>

namespace mContainers {

    namespace Utils {

        bool IsPrime(size_t n)
        {
            // Corner cases
            if (n <= 1)  return false;
            if (n <= 3)  return true;

            // This is checked so that we can skip 
            // middle five numbers in below loop
            if (n % 2 == 0 || n % 3 == 0) return false;

            for (int i = 5; i * i <= n; i = i + 6)
                if (n % i == 0 || n % (i + 2) == 0)
                    return false;

            return true;
        }

        // Function to return the smallest
        // prime number greater than N
        size_t NextPrime(size_t N)
        {
            // Base case
            if (N <= 1)
                return 2;

            size_t prime = N;
            bool found = false;

            // Loop continuously until isPrime returns
            // true for a number greater than n
            while (!found) {
                prime++;

                if (IsPrime(prime))
                    found = true;
            }

            return prime;
        }

        template<typename Key>
        std::string KeyToString(const Key& key)
        {
            std::ostringstream os;
            os << key;
            return os.str();
        }

        template<typename Key>
        size_t SuperFastHash(const Key& key)
        {
            std::string keyStr = KeyToString(key);
            size_t len = keyStr.length();
            const char* data = keyStr.c_str();
            size_t hash = len, tmp;
            int rem;

            if (len <= 0 || data == NULL) return 0;

            rem = len & 3;
            len >>= 2;

            /* Main loop */
            for (; len > 0; len--) {
                hash += get16bits(data);
                tmp = (get16bits(data + 2) << 11) ^ hash;
                hash = (hash << 16) ^ tmp;
                data += 2 * sizeof(uint16_t);
                hash += hash >> 11;
            }

            /* Handle end cases */
            switch (rem) {
            case 3: hash += get16bits(data);
                hash ^= hash << 16;
                hash ^= ((signed char)data[sizeof(uint16_t)]) << 18;
                hash += hash >> 11;
                break;
            case 2: hash += get16bits(data);
                hash ^= hash << 11;
                hash += hash >> 17;
                break;
            case 1: hash += (signed char)*data;
                hash ^= hash << 10;
                hash += hash >> 1;
            }

            /* Force "avalanching" of final 127 bits */
            hash ^= hash << 3;
            hash += hash >> 5;
            hash ^= hash << 4;
            hash += hash >> 17;
            hash ^= hash << 25;
            hash += hash >> 6;

            return hash;
        }
    }
}