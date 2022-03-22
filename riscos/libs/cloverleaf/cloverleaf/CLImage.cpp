//
// Created by lenz on 3/22/20.
//
#include <cstdlib>
#include <kernel.h>
#include <swis.h>
#include <tbx/colour.h>
#include <tbx/path.h>
#include <tbx/application.h>
#include <tbx/osgraphics.h>
#include "tinct.h"
#include "CLImage.h"
#include "CLImagePNGLoader.h"
#include "CLImageJPGLoader.h"
#include "Logger.h"

#define FILE_TYPE_PNG 0xB60
#define FILE_TYPE_JPEG 0xC85
#define FILE_TYPE_GIF 0x695

#define DATA_FORMAT_RGB ((os_mode_flags) 0x4u)
#define DATA_FORMAT BGR ((os_mode_flags) 0x0u)

unsigned int CLImage::initial_tinct_options = 0;

CLImage::CLImage() :
    _sprite_area(nullptr),
    _has_alpha(false),
    _width_px(0),
    _height_px(0) {
}

CLImage::CLImage(const std::string &file_name) : CLImage() {
    load(file_name);
}

CLImage::~CLImage() {
    if (_sprite_area) {
        free(_sprite_area);
    }
}

void CLImage::detect_rgb_mode() {
    int mode_flags = 0;
    xos_read_mode_variable(os_CURRENT_MODE, os_MODEVAR_MODE_FLAGS, &mode_flags,NULL);
    int data_format = (mode_flags & os_MODE_FLAG_DATA_FORMAT) >> os_MODE_FLAG_DATA_FORMAT_SHIFT;
    if (data_format == DATA_FORMAT_RGB) {
        Logger::info("RGB format detected. Set tinct_USE_OS_SPRITE_OP");
        initial_tinct_options = tinct_USE_OS_SPRITE_OP;
    } else {
        Logger::info("BGR format detected. Set tinct_ERROR_DIFFUSE");
        initial_tinct_options = tinct_ERROR_DIFFUSE;
    }
}

bool CLImage::load(const std::string &filename) {
    tbx::Path file_path(filename);

    if (_sprite_area) {
        free(_sprite_area);
        _sprite_area = nullptr;
    }

    switch (file_path.file_type()) {
        case FILE_TYPE_PNG:
            _sprite_area = CLImagePNGLoader::load(filename, &_width_px, &_height_px);
            _has_alpha = true;
            break;
        case FILE_TYPE_JPEG:
            _sprite_area = CLImageJPGLoader::load(filename, &_width_px, &_height_px);
            _has_alpha = false;
            break;
    }
    return (_sprite_area != nullptr);
}

void CLImage::plot(int left_x, int bottom_y,
                   tbx::Colour background_colour,
                   bool alpha, unsigned int tinct_options)
{
    if (_sprite_area == nullptr) {
        return;
    }

    _kernel_oserror *error;

    /*      Set up our flagword
    */
    tinct_options |= (initial_tinct_options | (background_colour & 0xffffff00));

    if (alpha) {
//        Logger::info("Tinct_PlotAlpha %d %d %dx%d", x, y - height(), width(), height());
        error = _swix(Tinct_PlotAlpha, _INR(2,4) | _IN(7),
                      ((unsigned char *) (_sprite_area)) + 16,
                      left_x, bottom_y,
                      tinct_options);
    } else {
//        Logger::info("Tinct_Plot %d:%d %dx%d", x, y - height(), width(), height());
        error = _swix(Tinct_Plot, _INR(2,4) | _IN(7),
                      ((unsigned char *) (_sprite_area)) + 16,
                      left_x, bottom_y,
                      tinct_options);
    }

    if (error) {
        Logger::error("CLImage::plot %s: 0x%x: %s",
              (alpha ? "alpha" : ""), error->errnum, error->errmess);
    }
}


void CLImage::plot_scaled(int left_x, int bottom_y,
                   tbx::Colour background_colour,
                   int req_width, int req_height,
                   bool alpha, unsigned int tinct_options)
{
    if (_sprite_area == nullptr) {
        return;
    }

    _kernel_oserror *error;

    /*      Set up our flagword
    */
    tinct_options |= (initial_tinct_options | (background_colour & 0xffffff00));

    if (alpha) {
//        Logger::info("Tinct_PlotScaledAlpha %d %d %dx%d", left_x, bottom_y, req_width, req_height);
        error = _swix(Tinct_PlotScaledAlpha, _INR(2,7),
                      ((unsigned char *) (_sprite_area)) + 16,
                      left_x, bottom_y,
                      req_width, req_height, tinct_options);
    } else {
//        Logger::info("Tinct_PlotScaled %d:%d %dx%d", left_x, bottom_y, req_width, req_height);
        error = _swix(Tinct_PlotScaled, _INR(2,7),
                      ((unsigned char *) (_sprite_area)) + 16,
                      left_x, bottom_y,
                      req_width, req_height, tinct_options);
    }

    if (error) {
        Logger::error("CLImage::plot_scaled %s: 0x%x: %s",
                     (alpha ? "alpha" : ""), error->errnum, error->errmess);
    }
}


tbx::UserSprite CLImage::plot_to_app_sprite(const std::string& sprite_name, tbx::Colour background_colour) {
    if (_sprite_area != nullptr) {
        tbx::SpriteArea *area = tbx::Application::instance()->sprite_area();
        tbx::UserSprite spr = area->get_sprite(sprite_name);
        Logger::debug("tbx::UserSprite CLImage::plot_to_app_sprite %s valid:%d", sprite_name.c_str(), spr.is_valid());
        if (spr.is_valid()) {

            int x, y, req_width, req_height,
                    spr_width = spr.width(),
                    spr_height = spr.height();
            if (width() > height()) {
                req_width = spr_width;
                req_height = height() * spr_width / width();
                x = 0,
                        y = ((spr_height - req_height) / 2);
            } else {
                req_height = spr_height;
                req_width = width() * spr_height / height();
                x = (spr_width - req_width) / 2;
                y = 0;
            }
            tbx::SpriteCapture capture = tbx::SpriteCapture(&spr, true);
            tbx::OSGraphics gr;
            gr.foreground(background_colour);
            gr.fill_rectangle(0, 0, spr_width, spr_height);
            plot_scaled(x, y, background_colour, req_width, req_height, has_alpha(), tinct_USE_OS_SPRITE_OP);
            capture.release();
            Logger::debug("tbx::UserSprite CLImage::plot_to_app_sprite captured %s", sprite_name.c_str());
            //plot_scaled(0, 0, background_colour, req_width, req_height, has_alpha(), tinct_USE_OS_SPRITE_OP);
            return spr;
        } else {
            Logger::error("CLImage::plot_to_app_sprite sprite name %s not found in the app sprite area",
                          sprite_name.c_str());
        }
    } else {
        Logger::error("CLImage::plot_to_app_sprite %s, source image does not loaded", sprite_name.c_str());
    }
    return tbx::UserSprite();
}

