#ifndef __UTIL_TIMEPAIR_HPP
#define __UTIL_TIMEPAIR_HPP

#include <sys/time.h>
#include <ctime>
#include <cmath>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace util
{

class TimePair
{
	struct timeval pair;

public:
	TimePair(time_t seconds, suseconds_t microseconds)
	{
		pair.tv_sec = seconds;
		pair.tv_usec = microseconds;
	}
  
  explicit TimePair(double duration)
  {
    pair.tv_sec = round(duration);
    pair.tv_usec = (duration - pair.tv_sec) * 1000000;
  }
  
  TimePair(const boost::posix_time::time_duration& duration)
  {
    pair.tv_usec = duration.total_microseconds() % 1000000;
    pair.tv_sec = duration.total_microseconds() / 1000000;
  }
  
  time_t Seconds() const { return pair.tv_sec; }
  suseconds_t Microseconds() const { return pair.tv_usec; }
  const struct timeval& Timeval() const { return pair; }
  struct timeval& Timeval() { return pair; }
  
  double ToDouble() const
  { return pair.tv_usec / 1000000.0 + pair.tv_sec; }
  
  static TimePair Seconds(time_t seconds)
  { return TimePair(seconds, 0); }
  static TimePair Microseconds(suseconds_t microseconds)
  { return TimePair(0, microseconds); }
};

std::string FormatDuration(const TimePair& timePair);

} /* util namespace */

#endif
