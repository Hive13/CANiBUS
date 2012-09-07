#ifndef __CANIBUSD_IO_H__
#define __CANIBUSD_IO_H__

#include <string>

std::string escapeHTML(std::string data);
std::string escapeXML(std::string data);
std::string itoa(int number);
void stringReplace( std::string & source, const std::string & find, const std::string & replace );

#endif
