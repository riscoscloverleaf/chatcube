//
// Created by lenz on 6/14/20.
//

#ifndef ROCHAT_CLSOUND_H
#define ROCHAT_CLSOUND_H

#include <string>

class CLSound {
public:
    static void play_mp3_file(const char *file, int volume = -1);
};


#endif //ROCHAT_CLSOUND_H
