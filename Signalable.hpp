#pragma once

#include <deque>
#include <vector>

namespace Magpie {
    class Signal {
    public:
        Signal(uint32_t _sender_id, uint32_t _group_id, uint32_t _sig_num): sender_id(_sender_id), group_id(group_id), sig_num(_sig_num) { }
        uint32_t sender_id;
        uint32_t group_id;
        uint32_t sig_num;
    };

    class Signalable {
    public:
        // Returns true if there are signals in the queue
        bool has_signal();

        // Returns the first signal in the queue
        Signal read_from_signal_queue();

        // Broadcasts an identical message to all members of this objects signal group
        void broadcast_to_group(uint32_t sig_num);

        // Sends messsage to a specific signalable object
        void send_signal(Signalable* receiver, uint32_t sig_num);

        // Defins how the play should react given a certain signal
        virtual void consume_signal() = 0;

    protected:
        uint32_t object_id;
        uint32_t group_id;
        // The "signal mailbox" for this object
        std::deque< Signal > signal_queue;
        // Objects that this object can automatically
        // broadcast messages to
        std::vector< Signalable* > signal_group;
    };
}