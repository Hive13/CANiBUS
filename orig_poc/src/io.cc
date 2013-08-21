#include <stdio.h>

#include "io.h"

#define BUFSIZE 2048

std::string escapeXML(std::string data) 
{
        stringReplace(data, "&", "\n");
        stringReplace(data, "<", "&lt;");
        stringReplace(data, ">", "&gt;");
        stringReplace(data, "\"", "&quot;");
        stringReplace(data, "\n", "&amp;");
        return data;
}

std::string escapeHTML(std::string data) 
{
        stringReplace(data, " ", "+");
        return data;
}

std::string itoa(int number)
{
        char buf[32];
        snprintf(buf, sizeof(buf)-1, "%d", number);
        buf[sizeof(buf)-1] = 0;
        return std::string(buf);
}

void stringReplace( std::string & source, const std::string & find, const std::string & replace )
{
        size_t j;
        for ( ; ( j = source.find( find ) ) != std::string::npos ; )
                source.replace( j, find.length(), replace );
}

