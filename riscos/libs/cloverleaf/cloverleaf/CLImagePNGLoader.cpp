//
// Created by lenz on 3/22/20.
//

#include <cstdlib>
#include <png.h>
#include <cloverleaf/Logger.h>
#include "CLImagePNGLoader.h"

/**
 * cl_png_warning -- callback for libpng warnings
 */
static void cl_png_warning(png_structp png_ptr, png_const_charp warning_message)
{
    Logger::info(warning_message);
}

/**
 * cl_png_error -- callback for libpng errors
 */
static void cl_png_error(png_structp png_ptr, png_const_charp error_message)
{
    Logger::error(error_message);
    longjmp(png_jmpbuf(png_ptr), 1);
}

static void cl_png_setup_transforms(png_structp png_ptr, png_infop info_ptr)
{
    int bit_depth, color_type, intent;
    double gamma;

    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);

    /* Set up our transformations */
    if (color_type == PNG_COLOR_TYPE_PALETTE) {
        png_set_palette_to_rgb(png_ptr);
    }

    if ((color_type == PNG_COLOR_TYPE_GRAY) && (bit_depth < 8)) {
        png_set_expand_gray_1_2_4_to_8(png_ptr);
    }

    if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(png_ptr);
    }

    if (bit_depth == 16) {
        png_set_strip_16(png_ptr);
    }

    if (color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb(png_ptr);
    }

    if (!(color_type & PNG_COLOR_MASK_ALPHA)) {
        png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
    }

    /* gamma correction - we use 2.2 as our screen gamma
     * this appears to be correct (at least in respect to !Browse)
     * see http://www.w3.org/Graphics/PNG/all_seven.html for a test case
     */
    if (png_get_sRGB(png_ptr, info_ptr, &intent)) {
        png_set_gamma(png_ptr, 2.2, 0.45455);
    } else {
        if (png_get_gAMA(png_ptr, info_ptr, &gamma)) {
            png_set_gamma(png_ptr, 2.2, gamma);
        } else {
            png_set_gamma(png_ptr, 2.2, 0.45455);
        }
    }

    png_read_update_info(png_ptr, info_ptr);
}

static png_bytep *calc_row_pointers(unsigned char *buffer, int width, int height)
{
    size_t rowstride = width * 4;
    png_bytep *row_ptrs;
    int hloop;

    row_ptrs = static_cast<png_bytep *>(malloc(sizeof(png_bytep) * height));

    if (row_ptrs != nullptr) {
        for (hloop = 0; hloop < height; hloop++) {
            row_ptrs[hloop] = buffer + (rowstride * hloop);
        }
    }

    return row_ptrs;
}

osspriteop_area* CLImagePNGLoader::load(const std::string& filename, int* width_ptr, int* height_ptr)
{
    png_structp png_ptr;
    png_infop info_ptr;
    png_infop end_info_ptr;
    png_uint_32 width, height;
    osspriteop_area* sprite_area = nullptr;
    volatile png_bytep * volatile row_pointers = nullptr;
    FILE *f = nullptr;

    *height_ptr = 0;
    *width_ptr = 0;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
                                     cl_png_error, cl_png_warning);
    if (png_ptr == nullptr) {
        return nullptr;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == nullptr) {
        png_destroy_read_struct(&png_ptr, nullptr, nullptr);
        return nullptr;
    }

    end_info_ptr = png_create_info_struct(png_ptr);
    if (end_info_ptr == nullptr) {
        png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
        return nullptr;
    }

    /* setup error exit path */
    if (setjmp(png_jmpbuf(png_ptr))) {
        /* cleanup and bail */
        goto png_cache_convert_error;
    }

    /* read from file */
    f = fopen(filename.c_str(), "rb");

    png_init_io(png_ptr, f);

    /* ensure the png info structure is populated */
    png_read_info(png_ptr, info_ptr);

    /* setup output transforms */
    cl_png_setup_transforms(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);

    /* Claim the required memory for the converted PNG */;
    sprite_area = create_sprite_area(width, height);
    if (sprite_area == nullptr) {
        /* cleanup and bail */
        goto png_cache_convert_error;
    }

    row_pointers = calc_row_pointers(sprite_buffer(sprite_area), width, height);

    if (row_pointers != nullptr) {
        png_read_image(png_ptr, (png_bytep *) row_pointers);
        *width_ptr = width;
        *height_ptr = height;
    } else {
        free(sprite_area);
        sprite_area = nullptr;
    }

png_cache_convert_error:

    /* cleanup png read */
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info_ptr);

    if (row_pointers != nullptr) {
        free((png_bytep *) row_pointers);
    }

    if (f) {
        fclose(f);
    }

    return sprite_area;
}
