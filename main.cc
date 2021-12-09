#include <iostream>
#include <cstdlib>
#include "RtMidi.h"
#include <map>
#include <vector>
// Platform-dependent sleep routines.
#if defined(WIN32)
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

// midiout.cpp
__inline__ void fib(uint64_t& x, uint64_t& y, uint64_t& out){
    out = x;
    y = ~(x >> 1);
    x = (x - y) & y;
}

void emit(uint64_t num) {

}

int main() {
    RtMidiOut *midiout = new RtMidiOut();
    std::vector<unsigned char> message;

    // Check available ports
    unsigned int nPorts = midiout->getPortCount();
    if (nPorts == 0) {
        std::cout << "No ports avaiable\n";
        delete midiout;
        return -1;
    }

    // open first available port
    midiout->openPort(0);
    // middle c = 60
    unsigned char C = 60;
    std::map<unsigned char, bool> on_notes;
    auto add_note = [&](unsigned char note) {
        message.push_back(0x90);
        message.push_back(note);
        message.push_back(128);
        on_notes[note] = true;
        midiout->sendMessage(&message);
    };

    auto clear_all_notes = [&]() {
        for (const auto& [note, _] : on_notes) {
            message.push_back(0x80);
            message.push_back(note);
            message.push_back(128);
            on_notes.erase(note);
        }
        midiout->sendMessage(&message);
    };

    auto add_bits_notes = [&](uint64_t num) {
        for (int i = 0; i < 64; i++) {
            int note = 60 + i;
            if (num & 1) {
                add_note(note);
            }
            num >>= 1;
        }
    };

    uint64_t x = 1;
    uint64_t y = 1;
    uint64_t num;
    while (true) {
        fib(x, y, num);
        add_bits_notes(num);
        SLEEP(500);
        clear_all_notes();
        
    }
    
  cleanup:
    delete midiout;

    return 0;
}
