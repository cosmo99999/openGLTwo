#pragma once
#include <cstring>
#include <enet/enet.h>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <string>

char IntToChar(int in) {
  char result = in + '0';
  return result;
}

int CharToInt(char in) {
  int result = in - '0';
  return result;
}

float FourByteCharToFloat(char *in, int start) {
  std::string temp = "";
  temp += in[start] +
          in[start + 1] +
          in[start + 2] +
          in[start + 3];
  const char* tempChar = temp.c_str();
  float result = std::stof(tempChar); 
  return result;
}

char *ReadPlayerData(unsigned char *input) {
  char *data = new char[13];

  for (int i = 0; i < 13; i++) {
    data[i] = static_cast<char>(data[i]);
  }

  return data;
}
char *ReadBroadcastPlayerData(unsigned char *input) {
  char pNumChar = static_cast<char>(input[0]);
  int pNum = CharToInt(pNumChar);
  int bytes = (13 * pNum) + 1;
  char *data = new char[bytes];

  for (int i = 0; i < bytes; i++) {
    data[i] = static_cast<char>(data[i]);
  }

  return data;
}
