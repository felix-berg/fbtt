#pragma once

#include <iostream>

namespace fbtt {
   /** Enumeration for terminal color. Can be used on an ostream to output text in different colors on
    * unix-based systems. */
   enum TerminalColor {
      BLACK = 30,
      RED, GREEN, YELLOW, BLUE, 
      MAGENTA, CYAN, WHITE, GRAY = 90, 
      BRIGHT_RED, BRIGHT_GREEN, 
      BRIGHT_YELLOW, BRIGHT_BLUE, 
      BRIGHT_MAGENTA, BRIGHT_CYAN, 
      BRIGHT_WHITE
   };

   /** Enumeration for terminal color. Can be used on an ostream to output text in different styles on
    * unix-based systems. */
   enum TerminalStyle {
      NONE = 0, BOLD, ITALICS
   };

   TerminalStyle g_terminalStyle = TerminalStyle::NONE; // don't change
   TerminalColor g_terminalColor = TerminalColor::WHITE; // don't change

   void updateOstreamColorAndStyle(std::ostream & os)
   {
      // unix escape code \033
      os << "\033[" << static_cast<int>(g_terminalStyle) 
         << ';'     << static_cast<int>(g_terminalColor) 
         << 'm';
   }

   std::ostream & operator << (std::ostream & os, TerminalColor color)
   {
   #ifdef __unix__
      g_terminalColor = color;
       updateOstreamColorAndStyle(os);
   #endif
      return os;
   };

   std::ostream & operator << (std::ostream & os, TerminalStyle style)
   {
   #ifdef __unix__
      g_terminalStyle = style;
       updateOstreamColorAndStyle(os);
   #endif
      return os;
}
};

