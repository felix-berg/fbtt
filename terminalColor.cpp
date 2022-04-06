#include "terminalColor.hpp"

#include <string>

fbtt::TerminalStyle g_terminalStyle = fbtt::TerminalStyle::NONE;
fbtt::TerminalColor g_terminalColor = fbtt::TerminalColor::WHITE;

void update_ostream_color_and_style(std::ostream & os)
{
   os << "\033[" << static_cast<int>(g_terminalStyle) 
      << ';'     << static_cast<int>(g_terminalColor) 
      << 'm';
}

std::ostream & fbtt::operator << (std::ostream & os, fbtt::TerminalColor color)
{
#ifdef __unix__
   g_terminalColor = color;
   update_ostream_color_and_style(os);
#endif
   return os;
};

std::ostream & fbtt::operator << (std::ostream & os, fbtt::TerminalStyle style)
{
#ifdef __unix__
   g_terminalStyle = style;
   update_ostream_color_and_style(os);
#endif
   return os;
}