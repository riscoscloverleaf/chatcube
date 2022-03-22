//
// Created by lenz on 3/23/20.
//
#include <memory>
#include "CLImageCache.h"

class CLImageNode {
public:
    std::string key;
    CLImage *value;

    CLImageNode *prev, *next;

    CLImageNode(const std::string& k, CLImage *v): key(k), value(v), prev(NULL), next(NULL) {}
    ~CLImageNode() { delete value; }
};

class CLImageNodeDoublyLinkedList {
    CLImageNode *front, *rear;

    bool isEmpty() {
        return rear == NULL;
    }

public:
    CLImageNodeDoublyLinkedList(): front(NULL), rear(NULL) {}

    CLImageNode* add_node_to_head(const std::string& key, CLImage* value) {
        CLImageNode *node = new CLImageNode(key, value);
        if(!front && !rear) {
            front = rear = node;
        }
        else {
            node->next = front;
            front->prev = node;
            front = node;
        }
        return node;
    }

    void move_node_to_head(CLImageNode *node) {
        if(node==front) {
            return;
        }
        if(node == rear) {
            rear = rear->prev;
            rear->next = NULL;
        }
        else {
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }

        node->next = front;
        node->prev = NULL;
        front->prev = node;
        front = node;
    }

    void remove_rear_node() {
        if(isEmpty()) {
            return;
        }
        if(front == rear) {
            delete rear;
            front = rear = NULL;
        }
        else {
            CLImageNode *temp = rear;
            rear = rear->prev;
            rear->next = NULL;
            delete temp;
        }
    }
    CLImageNode* get_rear_node() {
        return rear;
    }

};

class LRUImagesCache{
    int capacity, size;
    CLImageNodeDoublyLinkedList *images_list;
    std::map<std::string, CLImageNode*> images_map;

public:
    LRUImagesCache(int capacity) {
        this->capacity = capacity;
        size = 0;
        images_list = new CLImageNodeDoublyLinkedList();
        images_map = std::map<std::string, CLImageNode*>();
    }

    CLImage* get(const std::string& key) {
        auto found = images_map.find(key);
        if(found==images_map.end()) {
            return nullptr;
        }
        CLImage* val = found->second->value;

        // move the node to front
        images_list->move_node_to_head(found->second);
        return val;
    }

    void put(const std::string& key, CLImage* value) {
        if(images_map.find(key)!=images_map.end()) {
            // if key already present, update value and move node to head
            images_map[key]->value = value;
            images_list->move_node_to_head(images_map[key]);
            return;
        }

        if(size >= capacity) {
            // remove rear node
            std::string &k = images_list->get_rear_node()->key;
            images_map.erase(k);
            images_list->remove_rear_node();
            size--;
        }

        // add new node to head to Queue
        CLImageNode *node = images_list->add_node_to_head(key, value);
        size++;
        images_map[key] = node;
    }

    ~LRUImagesCache() {
        std::map<std::string, CLImageNode*>::iterator i1;
        for(i1=images_map.begin();i1!=images_map.end();i1++) {
            delete i1->second;
        }
        delete images_list;
    }
};

static LRUImagesCache *lru_cache = nullptr;

void init_images_cache(int capacity) {
    delete lru_cache;
    lru_cache = new LRUImagesCache(capacity);
}

CLImage *load_cached_image(const std::string &filename) {
    CLImage *img = lru_cache->get(filename);
    if (img) {
        return img;
    }
    img = new CLImage();
    img->load(filename);
    if (img->is_valid()) {
        lru_cache->put(filename, img);
        return img;
    } else {
        delete img;
        return nullptr;
    }
}
