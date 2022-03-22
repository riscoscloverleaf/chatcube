//
// Created by lenz on 3/25/20.
//

#ifndef ROCHAT_SCREENSHOOTUI_H
#define ROCHAT_SCREENSHOOTUI_H

enum ScreenshootType {
    FULL,
    AREA,
    HIDE_FULL,
    HIDE_AREA,
    WINDOW,
};

void screenshoot(ScreenshootType screenshoot_type);

#endif //ROCHAT_SCREENSHOOTUI_H
