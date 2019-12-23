/**
 * The StickerManager contains all sticker-related data. It contains a list of stickers and can search for names and ids
 * 
 * Author: Torben Hellige (Bauteiltöter)
 * Date: 23.12.2019
 * 
 * License: MIT (See LICENSE file)
 * 
 */


#ifndef STICKERMANAGER_H__
#define STICKERMANAGER_H__

#include <sticker.h>
#include <list>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>
#include <memory>
namespace pt = boost::property_tree;

class StickerManager 
{
public:
    StickerManager();

    
    void addSticker(std::shared_ptr<Sticker> s);
    std::shared_ptr<Sticker> getStickerById(std::string id);
    std::shared_ptr<Sticker> getStickerByName(std::string name);

private:
    void load();
    void save();

    std::string filename;

    std::list<std::shared_ptr<Sticker>> stickerset;
};

#endif