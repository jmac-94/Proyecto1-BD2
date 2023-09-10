#ifndef PROYECTO1_BD2_CONSTANTS_H
#define PROYECTO1_BD2_CONSTANTS_H

#include <iostream>

std::streamsize ROOT_SIZE = sizeof(int);
std::streamsize FDR_SIZE = sizeof(int);
std::streamsize HEADER_SIZE = ROOT_SIZE + FDR_SIZE;
int NULL_POS = -1;
int NOT_FL = -2;

#endif //PROYECTO1_BD2_CONSTANTS_H
