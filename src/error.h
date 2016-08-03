/*
 * Connor Kuehl
 * connorkuehl95@gmail.com
 *
 * error.h contains the error function prototypes used in
 * error.cpp. 
 */

#ifndef CHIP8_ERROR_H_
#define CHIP8_ERROR_H_

#include <string>

void abortChip8(const std::string&);
void printChip8Error(const std::string&);

#endif
