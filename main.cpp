#include <string>
#include <chrono>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>

#include "Yet-another-hook-library/hook.h"
#include "mini/ini.h"
#include "samp.hpp"

using namespace std::chrono;

unsigned long getMilliseconds() {
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count() % 1000;
}

struct msinfo {
    std::vector<unsigned long> c;
    std::size_t index;

    msinfo() : c{ getMilliseconds() }, index(0) {};
};

std::map<time_t, msinfo> times;
time_t last;

mINI::INIFile file("TimeFormat.ini");
mINI::INIStructure ini;

size_t strftime_hooked(char* &ptr, size_t &maxsize, const char* &format, const tm* &timeptr) {
    // get or create vector with ms
    time_t index = mktime(const_cast<tm*>(timeptr));
    auto it = times.find(index);
    if (it == times.end()) {
        msinfo ms;
        times.emplace(index, ms);
        it = times.find(index);
    }
    msinfo& cur = (*it).second;

    // add new ms.
    if (last) {
        if (last == index && cur.index >= cur.c.size()) {
            cur.c.push_back(getMilliseconds());
        }
        else if(last != index) {
            times[last].index = 0;
        }
    }

    // formatting ms in %03d.
    std::stringstream strms;
    strms << std::setfill('0') << std::setw(3) << cur.c[cur.index++];

    // replace %MM to ms.
    std::string format_(ini["Main"]["Format"]);
    size_t msexist = format_.find("%MM");
    if (msexist != std::string::npos)
        format_.replace(msexist, 3, strms.str());

    last = index;
    return strftime(ptr, maxsize, format_.c_str(), timeptr);
}

class time_format {
public:
    time_format() {
        SampVersion version = DetermineSampVersion();
        if (version == SampVersion::SAMP_UNKNOWN) return;
        file.read(ini);

        static hook strftime_hook(reinterpret_cast<decltype(strftime)*>(samp_addr(strftime_addr[version])), strftime_hooked);

        uintptr_t *spaces = spaces_addr[version];
        char* space = reinterpret_cast<char*>(samp_addr(spaces[0]));
        detail::memory_protect::unprotect_guard unprot0(space, 1);
        *space = std::stoi(ini["Main"]["space"]);

        space = reinterpret_cast<char*>(samp_addr(spaces[1]));
        detail::memory_protect::unprotect_guard unprot1(space, 1);
        *space = std::stoi(ini["Main"]["space"]);
    }
} time_format;