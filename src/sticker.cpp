/**
 * 
 * Author: Torben Hellige (Bauteiltöter)
 * Date: 23.12.2019
 * 
 * License: MIT (See LICENSE file)
 * 
 */


#include "sticker.h"
#include <iostream>
Sticker::Sticker(std::string name, std::string id)
: name(name)
, id(id)
{
}


std::string& Sticker::getId()
{
    return id;
}

std::string& Sticker::getName()
{
    return name;
}