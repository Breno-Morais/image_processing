#ifndef COMM_IF
#define COMM_IF

#include "../core/video_processor.h"

class CommandInterface {
    public:
        CommandInterface(VideoProcessor& proc) : processor(proc) {}
        void handleInput(char key);
        void printPrompt();

    private:
        VideoProcessor& processor;
};

#endif /* COMM_IF */