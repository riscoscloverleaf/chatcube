//
// Created by lenz on 3/22/20.
//

#ifndef ROCHAT_CLIMAGE_H
#define ROCHAT_CLIMAGE_H

#include <oslib/osspriteop.h>
#include <cloverleaf/tinct.h>
#include <tbx/colour.h>
#include <tbx/sprite.h>
#include <string>

class CLImage {
protected:
    int _width_px, _height_px;
    bool _has_alpha;
    osspriteop_area *_sprite_area;
    static unsigned int initial_tinct_options;
public:
    CLImage();
    CLImage(const std::string& file_name);
    ~CLImage();

    static void detect_rgb_mode();

    bool load(const std::string& filename);

    inline osspriteop_area *get_area_pointer() const { return _sprite_area; }
    inline osspriteop_header *get_sprite_pointer() const { return (osspriteop_header *)_sprite_area + 16; }
    inline tbx::UserSprite get_user_sprite() { return tbx::SpriteArea((tbx::OsSpriteAreaPtr)_sprite_area, false).get_sprite(
                reinterpret_cast<tbx::OsSpritePtr> (_sprite_area + 16)); }
    inline int width_px() const { return _width_px; }
    inline int height_px() const { return _height_px; }
    inline int width() const { return _width_px * 2; }
    inline int height() const { return _height_px * 2; }
    inline bool is_valid() { return _sprite_area != nullptr; }
    inline bool has_alpha() { return _has_alpha; };

    void plot(int left_x, int bottom_y,
               tbx::Colour background_colour,
               bool alpha, unsigned int tinct_options);
    void plot_scaled(int left_x, int bottom_y,
              tbx::Colour background_colour,
              int req_width, int req_height,
              bool alpha, unsigned int tinct_options);

    // plot (redirect output) to sprite in the App SpriteArea
    tbx::UserSprite plot_to_app_sprite(const std::string& sprite_name, tbx::Colour background_colour);
};

#endif //ROCHAT_CLIMAGE_H
