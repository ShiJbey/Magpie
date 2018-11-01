//
// Created by York on 2018/10/31.
//

#ifndef MAGPIE_SIGNALQUEUE_H
#define MAGPIE_SIGNALQUEUE_H

#include <map>
#include <string>
#include <vector>

class Signal {
public:
    Signal(std::string msg, int object_id, int group_id): msg(std::move(msg)), object_id(object_id), group_id(group_id){}
    int sig_id;
    std::string msg;
    int object_id;
    int group_id;
};

class SignalQueue {
public:
    static SignalQueue& getInstance() {
        static SignalQueue instance;
        return instance;
    }

    SignalQueue(SignalQueue const& ) = delete;
    void operator=(SignalQueue const&) = delete;

    Signal* get(int object_id, int group_id);
    void send(Signal* signal);

private:
    SignalQueue() = default;
    std::vector<Signal*> signal_queue;
    std::map<std::pair<int, int>, bool> read_map;
};

#endif //MAGPIE_SIGNALQUEUE_H
