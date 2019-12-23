/**
 * 
 * Author: Torben Hellige (Bauteiltöter)
 * Date: 23.12.2019
 * 
 * License: MIT (See LICENSE file)
 * 
 */

#include "sessionmanager.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>
#include <memory>
#include <iostream>

namespace pt = boost::property_tree;

//The constructor defines the path and the name of the session configuration file, then it loads the file
SessionManager::SessionManager(std::string sessionname)
: sessionname(sessionname)
, token("")
{
    std::string home = getenv("HOME");
    filename=home+"/.config/tgconsolebot/"+sessionname+"-session.json";

    load();
}

//Load the configuration JSON 
void SessionManager::load()
{
    try 
    {
        pt::ptree tree;

        pt::read_json(filename, tree);

        //get sessionname and telegram token
        token = tree.get<std::string>("token");
        sessionname = tree.get<std::string>("name");

        //Read configured users
        for (auto t : tree.get_child("users")) {
            std::shared_ptr<User> newUser = std::make_shared<User>(t.second.get<std::string>("name"), t.second.get<uint64_t>("chatId"));
            userList.push_back(newUser);
        }
    }
    catch(std::exception& e)
    {
        std::cout << "could not read session configuration file: " << e.what() << std::endl;
        std::cout << "Creating new session" << std::endl;
        save();
    }
}

//Write the internal data into the json file
void SessionManager::save()
{
    pt::ptree tree;

    tree.add("token", token);
    tree.add("name",sessionname);

    for(const auto &s : userList)
    {
        pt::ptree fieldNode1;
        fieldNode1.add("name", s->user);
        fieldNode1.add("chatId",s->chatId);
        tree.add_child("users.user",fieldNode1);
    }

    pt::write_json(filename,tree);
}


void SessionManager::setToken(const std::string& token)
{
    this->token=token;
    save();
}

const std::string& SessionManager::getToken()
{
    return token;
}

bool SessionManager::tokenValid()
{
    return token!="";
}

const std::list<std::shared_ptr<User>>& SessionManager::getUserList()
{
    return userList;
}


void SessionManager::addUser(std::shared_ptr<User> user)
{
    userList.push_back(user);
    save();
}