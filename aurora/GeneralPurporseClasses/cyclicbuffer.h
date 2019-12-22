#ifndef BUFFER
#define BUFFER
#include <stdint.h>

/**
* Cyclic buffer with 15 elements.
*/
class CyclicBuffer{
  public:
    CyclicBuffer();
    /**
    * Returns next value avaible. If none found 0.
    */
    uint8_t get();
    void put(uint8_t);
  private:
    uint8_t buffer[16];
    uint8_t readIndex = 0;
    uint8_t writeIndex = 0;
};

#endif