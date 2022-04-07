#include "test.hpp"

using namespace fbtt;

/** @returns True, if test failed, false otherwise */
bool TestResult::test_failed() const
{
   return !(statusCode == PASSED ||
            statusCode == NOT_RUN);
}

/** @returns test status. */
std::string TestResult::status() const
{
   switch (statusCode) {
      case PASSED:
         return "passed";
      case NOT_RUN:
         return "hasn't been executed";
      case ASSERTION_FAILURE:
         return "failed in assertion";
      case UNEXPECTED_ERROR:
         return "threw unexpected error";
      case DIDNT_THROW_EXPECTED:
         return "didn't throw expected error";
      default:
         return "unknown failure";
   };

}

/** @returns Formatted report of test run. */
std::string TestResult::report() const
{
   return "TEST \"" + testName + "\" " + (test_failed() ? "✕" : "✓") + " " + status() + ". " + (test_failed() ? "Reason: " + failString : "");
}

std::ostream & fbtt::operator << (std::ostream & os, const TestResult & res)
{
   os <<  "TEST " << TerminalStyle::BOLD
      << TerminalColor::BRIGHT_BLUE << res.testName << ' '
      << (res.test_failed() ? TerminalColor::RED : TerminalColor::GREEN)
      << res.status();

   if (res.test_failed()) {
      os << TerminalStyle::NONE << TerminalColor::WHITE
         << "\n      Reason: "
         << TerminalColor::YELLOW
         << res.failString;
   }

   return os << '\n'<< TerminalStyle::NONE  << TerminalColor::WHITE;
}