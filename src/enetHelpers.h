#pragma once
#include <X11/Xutil.h>
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

float FourByteCharToFloat(std::string in, int start) {
  std::string temp = in.substr(start, 4);
  // std::cout << "stoffing: " << temp << "\n";
  float result = std::stof(temp); 
  return result;
}

std::string ReadPlayerData(unsigned char *input) {
  std::string result = "";

  for (int i = 0; i < 13; i++) {
    result += static_cast<char>(input[i]);
  }

  return result;
}
std::string ReadBroadcastPlayerData(unsigned char *input) {
  char pNumChar = static_cast<char>(input[0]);
  int pNum = CharToInt(pNumChar);
  int byteCount = pNum*13 + 1;
  std::string result = "";
  for(int i = 0; i < byteCount; i++){
    result += static_cast<char>(input[i]);
  }
  return result;
}
