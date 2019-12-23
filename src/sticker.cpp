#include "sticker.h"
#include <iostream>
Sticker::Sticker(std::string name, std::string id)
: name(name)
, id(id)
{
    //std::cout << "Created sticker \"" << name << "\"" << " id: \""<<id<<"\"" << std::endl;
}


std::string& Sticker::getId()
{
    return id;
}

std::string& Sticker::getName()
{
    return name;
}