#include "Signalable.hpp"

// Checks if there is a signal in the queue
bool Magpie::Signalable::has_signal() {
    return signal_queue.size() > 0;
};

// Returns the first signal in the queue
Magpie::Signal Magpie::Signalable::read_from_signal_queue(){
    Signal sig = signal_queue.front();
    signal_queue.pop_front();
    return sig;
};

// Broadcasts an identical message to all members of this objects signal group
void Magpie::Signalable::broadcast_to_group(uint32_t sig_num) {
    for (auto it = signal_group.begin(); it != signal_group.end(); it++) {
        (*it)->signal_queue.emplace_back(object_id, group_id, sig_num);
    }
};

// Sends message to a specific signalable object
void Magpie::Signalable::send_signal(Signalable* receiver, uint32_t sig_num) {
    receiver->signal_queue.emplace_back(object_id, group_id, sig_num);
};