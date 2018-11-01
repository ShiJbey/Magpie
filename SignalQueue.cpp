//
// Created by York on 2018/10/31.
//

#include "SignalQueue.h"

Signal* SignalQueue::get(int object_id, int group_id) {
    for (Signal* s : signal_queue) {
        if (s->object_id == object_id || s->group_id == group_id) {
            if (read_map.find(std::make_pair(s->sig_id, object_id)) == read_map.end()) {
                read_map[std::make_pair(s->sig_id, object_id)] = true;
                return s;
            }
        }
    }
    return nullptr;
}

void SignalQueue::send(Signal* signal) {
    signal_queue.push_back(signal);
}