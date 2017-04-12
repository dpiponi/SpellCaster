#ifndef ANIMATED_H
#define ANIMATED_H

#include <deque>

using std::deque;

using namespace std::chrono;

inline double now() {
    static steady_clock::time_point start = steady_clock::now();
    return (steady_clock::now()-start).count()*steady_clock::period::num/static_cast<double>(steady_clock::period::den);
}

template<class X>
class Event {
public:
    Event(double t, const X &k) : time(t), key(k) { }
    double time;
    X key;
};

template<class X> class Animated {
    deque<Event<X>> events;
    double t_last;
    X x_last;
public:
    Animated() : t_last(now()), x_last(0) { }

    void addEvent(double t, const X &k) {
        if (events.size() == 0) {
            t_last = now();
        }
        events.push_back(Event<X>(t, k));
    }

    X get() {
        double t = now();
        if (events.size() > 0 && t >= events.front().time) {
            t_last = events.front().time;
            x_last = events.front().key;
            events.pop_front();
        }
        if (events.size() == 0) {
            return x_last;
        }
        double interval = events.front().time-t_last;
        if (interval > 0) {
            double frac = (t-t_last)/interval;
            double current = (1-frac)*x_last+frac*events.front().key;
            return current;
        } else {
            return events.front().key;
        }
    }
};

#endif
