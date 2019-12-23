#include "stickermanager.h"
#include <exception>

StickerManager::StickerManager()

{
    std::string home = getenv("HOME");
    filename=home+"/.config/tgconsolebot/sticker.json";
    load();
    save();
}

void StickerManager::load()
{
    // Create empty property tree object
    pt::ptree tree;

    // Parse the XML into the property tree.
    try 
    {
        pt::read_json(filename, tree);


        for (auto t : tree.get_child("stickers")) {
            std::shared_ptr<Sticker> newSticker = std::make_shared<Sticker>(t.second.get<std::string>("name"), t.second.get<std::string>("id"));
            stickerset.push_back(newSticker);
        }
    }
    catch(std::exception& e)
    {
        std::cout << "could not read sticker configuration file: " << e.what() << std::endl;
    }
}

void StickerManager::save()
{
    // Create an empty property tree object.
    pt::ptree tree;


    for(const auto &s : stickerset)
    {
        pt::ptree fieldNode1;
        fieldNode1.add("name", s->getName());
        fieldNode1.add("id",s->getId());
        tree.add_child("stickers.sticker",fieldNode1);
    }

    pt::write_json(filename,tree);
}


void StickerManager::addSticker(std::shared_ptr<Sticker> s)
{
    if(getStickerById(s->getId()) != nullptr)
    {
        std::cout << "Sticker id already in list" << std::endl;
        return;
    }


    if(getStickerByName(s->getName()) != nullptr)
    {
        std::cout << "Sticker name already in list" << std::endl;
        return;
    }

    stickerset.push_back(s);
    save();

}


std::shared_ptr<Sticker> StickerManager::getStickerById(std::string id)
{
    for(std::shared_ptr<Sticker> s : stickerset)
    {

        if( s->getId() == id)
            return s;
    }

    std::cout << "Sticker not found" << std::endl;
    return nullptr;
}


std::shared_ptr<Sticker> StickerManager::getStickerByName(std::string name)
{
    for(std::shared_ptr<Sticker> s : stickerset)
    {

        if( s->getName() == name)
            return s;
    }

    std::cout << "Sticker not found" << std::endl;
    return nullptr;
}