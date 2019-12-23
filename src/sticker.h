/**
 * The Sticker contains all sticker-related data, the unique id and the name
 * 
 * Author: Torben Hellige (Bauteiltöter)
 * Date: 23.12.2019
 * 
 * License: MIT (See LICENSE file)
 * 
 */


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