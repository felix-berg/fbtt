#pragma once

#include <iostream>

enum TerminalColor {
   BLACK = 30,
   RED, GREEN, YELLOW, BLUE, 
   MAGENTA, CYAN, WHITE, GRAY = 90, 
   BRIGHT_RED, BRIGHT_GREEN, 
   BRIGHT_YELLOW, BRIGHT_BLUE, 
   BRIGHT_MAGENTA, BRIGHT_CYAN, 
   BRIGHT_WHITE, 
   RESET = 0
};

enum TerminalStyle {
   NONE = 0, BOLD, ITALICS
};

const std::string resetTerminal = "\033[0m";

TerminalStyle g_terminalStyle = NONE;
TerminalColor g_terminalColor = WHITE;

void update_ostream_color_and_style(std::ostream & os)
{
   os << "\033[" << g_terminalStyle << ';' << std::to_string(int(g_terminalColor)) << 'm';
}

std::ostream & operator << (std::ostream & os, TerminalColor color)
{
#ifdef __unix__
   g_terminalColor = color;
   update_ostream_color_and_style(os);
#endif
   return os;
};

std::ostream & operator << (std::ostream & os, TerminalStyle style)
{
#ifdef __unix__
   g_terminalStyle = style;
   update_ostream_color_and_style(os);
#endif
   return os;
}