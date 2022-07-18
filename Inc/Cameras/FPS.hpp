#ifndef __FPS_HPP
#define __FPS_HPP

#include <boost/date_time/posix_time/posix_time.hpp>

class FPS {
public:
    FPS();
    double Tick();
private:
    boost::posix_time::ptime lastTick;
    double avg = 0;
};

FPS::FPS() {
    lastTick = boost::posix_time::microsec_clock::local_time();
}

double FPS::Tick() {
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration dur = now -lastTick;
    auto timeDur = dur.total_microseconds();
    lastTick = now;

    avg = 0.9 * avg + 0.1 * (1000000 / static_cast<double>(timeDur));
    return avg;
}

#endif //__FPS_HPP
