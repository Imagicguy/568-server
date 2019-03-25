#ifndef __XML_H__
#define __XML_H__

#include <cstring>
#include <iostream>

class XML {
private:
  std::string text;

public:
  XML(std::string XML_text) : text(XML_text) {}
};
#endif
