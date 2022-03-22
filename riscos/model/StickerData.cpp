//
// Created by lenz on 2/3/20.
//

#include "StickerData.h"

void StickerData::update_from_json(const cJSON *json) {
    pic = get_string_value(json, "pic");
    pic_small = get_string_value(json, "pic_small");
    name = get_string_value(json, "name");
}

void StickerGroupData::update_from_json(const cJSON *json) {
    pic_small = get_string_value(json, "pic_small");
    name = get_string_value(json, "name");

    cJSON *json_item;
    const cJSON *json_items  = get_json_array(json, "items");
    items.clear();
    items.reserve(cJSON_GetArraySize(json_items));
    cJSON_ArrayForEach(json_item, json_items) {
        StickerData sticker;
        sticker.update_from_json(json_item);
        items.push_back(sticker);
    }
}