//
// Created by lenz on 6/14/20.
//

#include "CLSound.h"
#include <kernel.h>
#include <tbx/swixcheck.h>

void CLSound::play_mp3_file(const char* file, int volume) {
    _kernel_swi_regs regs;

    regs.r[0] = 0x4; // Transient
    regs.r[1] = reinterpret_cast<int>(file);
    regs.r[2] = volume; // volune
    regs.r[3] = 0;

    // AMPlayer_Play
    tbx::swix_check(_kernel_swi(0x52E00, &regs, &regs));
}
