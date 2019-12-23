#ifndef STICKER_H__
#define STICKER_H__

#include <string>

class Sticker {
public: 
    Sticker(std::string name, std::string id);

    std::string& getId();
    std::string& getName();

private:
    std::string name;
    std::string id;
};

#endif