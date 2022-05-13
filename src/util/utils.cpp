#include "utils.h"

std::ostream& operator <<(std::ostream& os, const Timer& t) {
    std::ostringstream os_s;
    os_s << "[ " << t.elapsed() << " ]";
    os << os_s.str();
    return os;
}

Timer tstamp;
std::ostream& log(std::ostream& os) {
    os << tstamp;
    return os;
}

