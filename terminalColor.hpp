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
      BRIGHT_WHITE, 
      RESET = 0
   };

   /** Enumeration for terminal color. Can be used on an ostream to output text in different styles on
    * unix-based systems. */
   enum TerminalStyle {
      NONE = 0, BOLD, ITALICS
   };
   
   // changing of terminal color & style on given ostream
   std::ostream & operator << (std::ostream & os, TerminalColor color);
   std::ostream & operator << (std::ostream & os, TerminalStyle style);
};

