//
// Created by lenz on 3/9/20.
//

#include "CLGraphics.h"
#include "CLImageCache.h"
#include "CLUtf8.h"
#include <tbx/font.h>
#include <swis.h>
#include "Logger.h"

CLFontStyle::CLFontStyle(const char *_fontname, rufl_style _style, int _size) :
    fontname(_fontname),
    style(_style),
    size(tbx::os_to_points_16th(_size)),
    _font_height(0) {}

int CLFontStyle::get_font_height() {
    if (!_font_height) {
        int box[4];
        rufl_font_bbox(fontname, style, size, box);
//        Logger::debug("CLFontStyle::get_font_height %d p_to_o=%d",box[3] - box[1]);
        _font_height =  box[3] - box[1];
    }
    return _font_height;
}

void CLGraphics::draw_image(CLImage& img, int left_x, int bottom_y, tbx::Colour background_colour,
                            unsigned int tinct_options) {
//    Logger::debug("CLGraphics::draw_image %d,%d %d %d %d %d", left_x, bottom_y, offset_x(), offset_y(), left_x+offset_x(), bottom_y+offset_y());
    img.plot(left_x+offset_x(), bottom_y + offset_y(), background_colour, img.has_alpha(), tinct_options);
}

void CLGraphics::draw_image(const std::string &filename, int left_x, int bottom_y, tbx::Colour background_colour,
                            unsigned int tinct_options) {
    CLImage img;
    img.load(filename);
    if (img.is_valid()) {
//        Logger::debug("plot im = %s %d:%d", filename.c_str(), left_x, bottom_y);
        draw_image(img, left_x, bottom_y, background_colour, tinct_options);
    }

}

void CLGraphics::draw_cached_image(const std::string &filename, int left_x, int bottom_y, tbx::Colour background_colour,
                                   unsigned int tinct_options) {
    CLImage *img = load_cached_image(filename);
    if (img) {
//        Logger::debug("plot cached im = %s %d:%d", filename.c_str(), left_x, bottom_y);
        draw_image(*img, left_x, bottom_y, background_colour, tinct_options);
    }
}

void CLGraphics::draw_image_scaled(CLImage& img, int left_x, int bottom_y, tbx::Colour background_colour, int req_width, int req_height,
                            unsigned int tinct_options) {
    img.plot_scaled(left_x+offset_x(), bottom_y + offset_y(), background_colour, req_width, req_height, img.has_alpha(), tinct_options);
}

void CLGraphics::draw_image_scaled(const std::string &filename, int left_x, int bottom_y, tbx::Colour background_colour,
                            int req_width, int req_height,
                            unsigned int tinct_options) {
    CLImage img;
    img.load(filename);
    if (img.is_valid()) {
//        Logger::debug("plot_scaled im = %s %d:%d", filename.c_str(), left_x, bottom_y);
        draw_image_scaled(img, left_x, bottom_y, background_colour, req_width, req_height, tinct_options);
    }

}

void CLGraphics::draw_cached_image_scaled(const std::string &filename, int left_x, int bottom_y, tbx::Colour background_colour,
                                   int req_width, int req_height,
                                   unsigned int tinct_options) {
    CLImage *img = load_cached_image(filename);
    if (img) {
//        Logger::debug("plot cached scaled im = %s %d:%d", filename.c_str(), left_x, bottom_y);
        draw_image_scaled(*img, left_x, bottom_y, background_colour, req_width, req_height, tinct_options);
    }
}

void CLGraphics::draw_image_circled(int left_x, int bottom_y, int radius, const std::string& filename, tbx::Colour bgcolor)
{
    int diameter = radius + radius;
    tbx::DrawPath circle_path;

    // hack
    draw_cached_image_scaled(filename, left_x, bottom_y, bgcolor, diameter, diameter);

    circle_path.move(0,0);
    circle_path.line(diameter, 0);
    circle_path.line(diameter,diameter);
    circle_path.line(0, diameter);
    circle_path.close_line();
    circle_path.circle(radius, radius, radius);
    circle_path.end_path();
    foreground(bgcolor);
    fill(left_x, bottom_y, circle_path, tbx::WINDING_NON_ZERO, 1);
}


void CLGraphics::draw_circle(int x, int y, int radius, int line_thickness, tbx::Colour fill_color, tbx::Colour border_color)
{
    tbx::DrawPath circle_path;
    circle_path.circle(0, 0, radius);
    circle_path.end_path();
    foreground(fill_color);
    fill(x + radius, y + radius, circle_path, tbx::WINDING_NON_ZERO);

    if (line_thickness) {
        tbx::DrawCapAndJoin caps;
        caps.join(tbx::DrawCapAndJoin::BEVELLED_JOINS);
        foreground(border_color);
        stroke(x + radius, y + radius, circle_path, tbx::WINDING_NON_ZERO, 1, line_thickness, &caps);
    }
}

void CLGraphics::draw_rounded_box(int xmin, int ymin, int xmax, int ymax, int border_radius, int line_thickness, tbx::Colour fill_color, tbx::Colour border_color) {
    int width = xmax - xmin, height = ymax - ymin;
    tbx::DrawPath path;
    /*
     * draw:  top-right -> bottom-right -> bottom-left -> top-left corners
    */

    int bx1, by1, bx2, by2, xs, ys, xe, ye;
    int rkappa = (border_radius * 36914) >> 16; // radius * kappa (0.5522847498307933984022516322796)

    // First curve
    xs = width - border_radius;
    ys = height;
    path.move(xs, ys);
    xe = width;
    ye = height - border_radius;
    if (border_radius != 0) {
        bx1 = xs + rkappa;
        by1 = ys;
        bx2 = xe;
        by2 = ye + rkappa;
        path.bezier(bx1, by1, bx2, by2, xe, ye);
    }
    ye = border_radius;
    path.line(xe, ye);

    /* Second curve */
    xs = xe;
    ys = ye;
    xe = xs - border_radius;
    ye = 0;
    if (border_radius != 0) {
        bx1 = xs;
        by1 = ys - rkappa;
        bx2 = xe + rkappa;
        by2 = ye;
        path.bezier(bx1, by1, bx2, by2, xe, ye);
    }
    xe = border_radius;
    path.line(xe,ye);

    /* Third curve */
    xs = xe;
    ys = ye;
    ye = border_radius;
    xe = 0;
    if (border_radius != 0) {
        bx1 = xs - rkappa;
        by1 = ys;
        bx2 = xe;
        by2 = ye - rkappa;
        path.bezier(bx1, by1, bx2, by2, xe, ye);
    }
    ye = height - border_radius;
    path.line(xe,ye);

    /* Final curve */
    xs = xe;
    ys = ye;
    xe = border_radius;
    ye = height;
    if (border_radius != 0) {
        bx1 = xs;
        by1 = ys + rkappa;
        bx2 = xe - rkappa;
        by2 = ye;
        path.bezier(bx1, by1, bx2, by2, xe, ye);
    }

    /* Close subpath */
    path.close_line();
    path.end_path();

    foreground(fill_color);
    fill(xmin, ymin, path, tbx::WINDING_NON_ZERO);

    if (line_thickness) {
        foreground(border_color);
        tbx::DrawCapAndJoin caps;
        caps.join(tbx::DrawCapAndJoin::BEVELLED_JOINS);
        stroke(xmin, ymin, path, tbx::WINDING_NON_ZERO, 1, line_thickness, &caps);
    }
}

void CLGraphics::set_font_colour(tbx::Colour fore, tbx::Colour back)
{
    _kernel_swi_regs regs;
    regs.r[0] = 0; // current font
    regs.r[1] = (int)(unsigned)back;
    regs.r[2] = (int)(unsigned)fore;
    regs.r[3] = 14;

    // ColourTrans_SetFontColours
    _kernel_swi(0x4074F, &regs, &regs);
}

void CLGraphics::draw_text(int x, int y, const std::string& text, const CLFontStyle& font_style, tbx::Colour fcolor, tbx::Colour bcolor, bool background_rectangle)
{
    if (text.empty()) {
        return;
    }
    set_font_colour(fcolor, bcolor);
    if (background_rectangle && bcolor != tbx::Colour::no_colour) {
        int width;
        int ymax = ((CLFontStyle&)font_style).get_font_height() / 2 + y + 4;
        rufl_width(font_style.fontname, font_style.style, font_style.size, text.c_str(), text.size(), &width);
        foreground(bcolor);
        fill_rectangle(x - 2, y - 4, x + width + 2, ymax);
//        Logger::debug("CLGraphics::draw_text fill font_style=%d", font_style.style);
    }
    rufl_paint(font_style.fontname, font_style.style, font_style.size, text.c_str(), text.size(), offset_x() + x, offset_y() + y, rufl_BLEND_FONT);
}

void CLGraphics::draw_text_cut_to_width(int x, int y, int width, const std::string& text, const CLFontStyle& font_style, tbx::Colour fcolor, tbx::Colour bcolor, bool append_suffix)
{
    if (text.empty()) {
        return;
    }
//    Logger::debug("draw_text_cut_to_width txt [%s]", text.c_str());
    std::string str = cut_string_to_with(text, width, font_style, append_suffix);
//    Logger::debug("draw_text_cut_to_width txt1 [%s]", str.c_str());
    set_font_colour(fcolor, bcolor);
    rufl_paint(font_style.fontname, font_style.style, font_style.size, str.c_str(), str.size(), offset_x() + x, offset_y() + y, rufl_BLEND_FONT);
//    Logger::debug("draw_text_cut_to_width txt3 [%s]", str.c_str());
}

void CLGraphics::draw_text_centered(int x, int y, int width, const std::string& text, const CLFontStyle& font_style, tbx::Colour fcolor, tbx::Colour bcolor, bool append_suffix)
{
    if (text.empty()) {
        return;
    }
    std::string str = cut_string_to_with(text, width, font_style, append_suffix);
    int txt_width = get_text_width(str, font_style);
    x = (width - txt_width) / 2;
    set_font_colour(fcolor, bcolor);
    rufl_paint(font_style.fontname, font_style.style, font_style.size, str.c_str(), str.size(), offset_x() + x, offset_y() + y, rufl_BLEND_FONT);
}

int CLGraphics::get_text_width(const std::string& text, const CLFontStyle& font_style) {
    int width;
    if (text.empty()) {
        return 0;
    }
    rufl_width(font_style.fontname, font_style.style, font_style.size, text.c_str(), text.size(), &width);
    //Logger::debug("CLGraphics::get_text_width %d",width);
    return width;
}

int CLGraphics::split_text_to_width(const std::string& text, int width, const CLFontStyle& font_style, std::list<std::string>& lines) {
    if (text.empty()) {
        return 0;
    }
    int code;
    int actual_x;
    unsigned int len;
    size_t b_off = 0, pos = 0;
    char *txt = (char*)text.c_str();
    char *space;
    for (len = text.size(); len > 0; len -= b_off, txt += b_off, pos += b_off) {
        code = rufl_split(font_style.fontname, (rufl_style)font_style.style, font_style.size, txt, len, width, &b_off, &actual_x);
        if (code != rufl_OK) {
            if (code == rufl_FONT_MANAGER_ERROR)
                Logger::error("rufl_x_to_offset: 0x%x: %s",
                             rufl_fm_error->errnum,
                             rufl_fm_error->errmess);
            else
                Logger::error("rufl_x_to_offset: 0x%x", code);
            return 0;
        }
        for (space = txt; space < txt + b_off; space++) {
            if (*space == '\r' || *space == '\n')
                break;
        }
        if (space != txt + b_off) {
            /* Found newline; use it */
            lines.push_back(text.substr(pos, space - txt));
            /* CRLF / LFCR pair */
            if (*space == '\r' && *(space + 1) == '\n')
                space++;
            else if (*space == '\n' && *(space + 1) == '\r')
                space++;

            b_off = space + 1 - txt;

//                if (len - b_off == 0) {
//                    lines.push_back(text.substr(pos + b_off, space - txt));
//                    /* reached end of input => add last line */
//                    ta->lines[line_count].b_start =
//                            text + b_off - ta->text;
//                    ta->lines[line_count++].b_length = 0;
//                }
            continue;
        }
        if (len - b_off > 0) {
            /* find last space (if any) */
            for (space = txt + b_off; space > txt; space--)
                if (*space == ' ')
                    break;
            if (space != txt)
                b_off = space + 1 - txt;
        }
        lines.push_back(text.substr(pos, b_off));
    }
    return lines.size();
}

std::string CLGraphics::cut_string_to_with(const std::string& text, int width, const CLFontStyle& font_style, bool append_suffix) {
    size_t end_offset;
    int _actual_x;
    if (text.empty()) {
        return text;
    }
    int code = rufl_split(font_style.fontname, font_style.style, font_style.size,
                          text.c_str(), text.size(), width, &end_offset, &_actual_x);
//    Logger::debug("cut_string_to_with endoff=%d text=%s", end_offset, text.c_str());
    if (code != rufl_OK) {
        if (code == rufl_FONT_MANAGER_ERROR)
            Logger::error("rufl_split: 0x%x: %s",
                         rufl_fm_error->errnum,
                         rufl_fm_error->errmess);
        else
            Logger::error("rufl_split: 0x%x", code);
        return "";
    }
    if (end_offset < text.size()) {
        if (append_suffix && end_offset > 12) { // 12 is a max length of utf8 char (4 bytes) * 3
            return utf8_substr2(text.substr(0, end_offset), 0, -3) + "...";
        }
    }
    return text;
}


std::list<CLSpan> CLGraphics::split_text_to_spans(const std::string& text, int text_start, int text_len, int x, int box_width, const CLFontStyle& font_style) {
    std::list<CLSpan> result;
    int code, actual_x, len,
        width = box_width - x;
    size_t b_off = 0, pos = text_start;
    char* txt = (char*)text.c_str() + text_start;
    char* chr, *end_span;
    bool newline = false;
//    Logger::debug("split_text_to_spans start=%d, len=%d, box_width=%d, x=%d\ntext=%s\ntxt=%s",text_start, text_len, box_width, x, text.substr(text_start, text_len).c_str(), txt);
    for (len = text_len; len > 0; len -= b_off, txt += b_off, pos += b_off) {
        code = rufl_split(font_style.fontname, (rufl_style)font_style.style, font_style.size, txt, len, width, &b_off, &actual_x);
        if (code != rufl_OK) {
            if (code == rufl_FONT_MANAGER_ERROR)
                Logger::error("split_text_to_spans rufl_x_to_offset: 0x%x: %s",
                              rufl_fm_error->errnum,
                              rufl_fm_error->errmess);
            else
                Logger::error("split_text_to_spans rufl_x_to_offset: 0x%x", code);
            result.clear();
            return result;
        }
        width = box_width;
        end_span = txt + b_off;
        for (chr = txt; chr < end_span; chr++) {
            if (*chr == '\r' || *chr == '\n')
                break;
        }
        if (chr != end_span) {
            /* Found newline; use it */
//            Logger::debug("split_text_to_spans newline found start=%d len=%d", pos, chr - txt);
            result.emplace_back(CLSpan{
                    .start = pos,
                    .len = chr - txt,
                    .x = x,
                    .width = actual_x,
                    .newline = true
            });
            /* CRLF / LFCR pair */
            if (*chr == '\r' && *(chr + 1) == '\n')
                chr++;
            else if (*chr == '\n' && *(chr + 1) == '\r')
                chr++;

            b_off = chr + 1 - txt;
            x = 0;
            continue;
        }
        if (len - b_off > 0) {
            newline = true;
            /* find last space (if any) */
            for (chr = end_span; chr > txt; chr--) {
                if (*chr == ' ') break;
            }
            if (chr != txt) {
                b_off = chr + 1 - txt;
            } else {
                // don't cut in middle of word
                chr = txt + b_off;
                if (x > 0 && *chr != '\r' && *chr != '\n' && *chr != ' ') {
//                    Logger::debug("split_text_to_spans newline b_off=%d txt=%s chr=%c", b_off, txt, *chr);
                    b_off = 0;
                    result.emplace_back(CLSpan{
                            .start = 0,
                            .len = 0,
                            .x = 0,
                            .width = 0,
                            .newline = true
                    });
                    newline = false;
                    x = 0;
                    continue;
                }
            }
        }
//        Logger::debug("split_text_to_spans new span start=%d len=%d newline=%d", pos, static_cast<int>(b_off), newline);
        result.emplace_back(CLSpan{
                .start = pos,
                .len = static_cast<int>(b_off),
                .x = x,
                .width = actual_x,
                .newline = newline
        });
        newline = false;
        x = 0;
    }
    return result;
}