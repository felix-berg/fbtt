#pragma once

#include <iostream>

namespace fbtt {
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
   
   std::ostream & operator << (std::ostream & os, TerminalColor color);
   std::ostream & operator << (std::ostream & os, TerminalStyle style);
};

