#ifndef ANIMATED_H
#define ANIMATED_H

#include <deque>
#include <thread>

using std::deque;

using namespace std::chrono;
using namespace std::this_thread;

inline double now() {
    static steady_clock::time_point start = steady_clock::now();
    return (steady_clock::now()-start).count()*steady_clock::period::num/static_cast<double>(steady_clock::period::den);
}

inline void wait_until(double time) {
    while (now() < time) {
        std::this_thread::sleep_for(1ms);
    }
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

    void addEvent(double t) {
        addEvent(t, x_last); // XXX ???
    }

    void addEvent(double t, const X &k) {
        update(now());

        // Invalidate all later events
        Event<X> event(t, k);
        events.erase(std::upper_bound(events.begin(), events.end(), event,
            [](const Event<X> &a, const Event<X> &b) { return a.time < b.time; }), events.end());

        events.push_back(event);
    }

    X get() {
        double t = now();

        update(t);

        return x_last;
    }

    // Assuming t >= t_last
    void update(double t) {
        if (events.size() == 0) {
            t_last = t;
            return;
        }

        while (t > events.front().time) {
            t_last = events.front().time;
            x_last = events.front().key;
            events.pop_front();

            if (events.size() == 0) {
                return;
            }
        }

        double interval = events.front().time-t_last;
        if (interval > 0) {
            double frac = (t-t_last)/interval;
            double current = (1-frac)*x_last+frac*events.front().key;
            t_last = t;
            x_last = current;
        }
    }
};

#endif
