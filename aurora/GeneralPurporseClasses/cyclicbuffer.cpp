#include "cyclicbuffer.h"
#include <iostream>

CyclicBuffer::CyclicBuffer() {

}

uint8_t CyclicBuffer::get() {
  if(this->readIndex == this->writeIndex) {
    return 0;
  }
  uint8_t temp = this->buffer[this->readIndex];
  this->readIndex = (this->readIndex + 1) % 16;
  return temp;
}

void CyclicBuffer::put(uint8_t value) {
  this->buffer[this->writeIndex] = value;
  this->writeIndex = (this->writeIndex + 1) % 16;

  if(this->readIndex == this->writeIndex){
    this->readIndex = (this->readIndex + 1) % 16;
  }
}