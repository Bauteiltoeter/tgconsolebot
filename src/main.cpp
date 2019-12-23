/**
 * Main logic of the tgconsolebot
 * 
 * Author: Torben Hellige (Bauteiltöter)
 * Date: 23.12.2019
 * 
 * License: MIT (See LICENSE file)
 * 
 */


#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <chrono>
#include <thread>
#include <tgbot/tgbot.h>
#include <boost/program_options.hpp>
#include <memory>

#include "stickermanager.h"
#include "sticker.h"
#include "sessionmanager.h"

namespace po = boost::program_options;


int main(int argc, char * argv[]) 
{
    StickerManager stickerManager;
    std::unique_ptr<SessionManager> sessionManager;

    std::string newToken="";
    std::string message="";
    std::string stickerId="";
    std::string stickerName="";
    std::string sessionName="";
    std::string fileName="";
    bool waitForNewUsers=false;
    bool addSticker=false;

    po::options_description desc("Allowed options: ");
    desc.add_options()
        ("help", "produce help message")
        ("message,m",     po::value<std::string>(), "Send message")
        ("sticker,s",     po::value<std::string>(),  "Send sticker (name)")
        ("file,f", po::value<std::string>(),"Send file")
        ("session,s", po::value<std::string>(), "Set session name")
        ("sid",     po::value<std::string>(),  "Send sticker (ID)")
        ("set-token", po::value<std::string>(), "set access token")
        ("add-user", "Wait for new useres")
        ("add-sticker", "Show sticker ID");

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
    }
    catch(const std::exception& e)
    {
        std::cerr  << e.what() << '\n';
        std::cout << desc << std::endl;
        return -1;
    }
    po::notify(vm);    

    if (vm.count("help")) 
    {
        std::cout << desc << "\n";
        return 1;
    }

    if(vm.count("add-user"))
    {
        std::cout << "Wait for new useres" << std::endl;
        waitForNewUsers=true;
    }

    if(vm.count("add-sticker"))
    {
        std::cout << "Showing sticker IDs" << std::endl << "Please send stickers";
        addSticker=true;
    }

    if(vm.count("session"))
    {
        sessionName = vm["session"].as<std::string>();
    }
    else
    {
        sessionName = "default";
        std::cout << "No sessionname set, using 'default'" << std::endl;
    }
    
    sessionManager = std::make_unique<SessionManager>(sessionName);


    if (vm.count("set-token")) 
    {
        newToken = vm["set-token"].as<std::string>();
    } 

    if(newToken != "")
    {
        sessionManager->setToken(newToken);
    }

    if(!sessionManager->tokenValid())
    {
        std::cout << "No valid token set, abort" << std::endl;
        return -1;
    }

    if(vm.count("file"))
    {
        fileName = vm["file"].as<std::string>();
    }
    
    if(vm.count("message"))
    {
        message = vm["message"].as<std::string>();
    }

    if(vm.count("sticker"))
    {
        stickerName = vm["sticker"].as<std::string>();
    }

    if(vm.count("sid"))
    {
        stickerId = vm["sid"].as<std::string>();
    }

    //Check if the user wants to do nothing
    if(!waitForNewUsers && !addSticker && message=="" && stickerId == "" && stickerName == "" && fileName=="")
    {
        std::cout << "No action specified" << std::endl;
        return -1;
    }
    
    //userinput done, create telegram bot
    std::unique_ptr<TgBot::Bot> bot = nullptr;

    try 
    {
        bot = std::make_unique<TgBot::Bot>(sessionManager->getToken());
    }
    catch(const std::exception& e)
    {
        std::cout << "Error while creating the telegram bot!" << std::endl;
        std::cerr << e.what() << '\n';
        return -1;
    }

    //Do we need to responde on user input or are we just sending data out?
    if(waitForNewUsers || addSticker)
    {
        //we have to register the callback for new useres
        if(waitForNewUsers)
        {
            bot->getEvents().onCommand("start", [&bot,&sessionManager](TgBot::Message::Ptr message)
            {
                std::cout << "Got new user, id:"  << message->chat->id << std::endl;
                //Create and add user
                std::shared_ptr<User> newUser = std::make_shared<User>(message->chat->username,message->chat->id);
                sessionManager->addUser(newUser);
            });
        }

        //We have to wait for new stickers
        if(addSticker)
        {
            bot->getEvents().onAnyMessage([&bot,&stickerManager](const TgBot::Message::Ptr msg){
                if(msg->sticker)
                {
                    std::cout << "Got message with sticker id: " << msg->sticker->fileId << std::endl;
                    std::cout << "Please enter name for this sticker: ";
                    std::string stickername;

                    std::cin >> stickername;
                    std::cout << std::endl << "New name:" << stickername << std::endl;

                    //Create sticker and store in stickerManager
                    std::shared_ptr<Sticker> newSticker = std::make_shared<Sticker>(stickername,msg->sticker->fileId);
                    stickerManager.addSticker(newSticker);     
                }
                else
                {
                    std::cout << "Got message with invalid sticker id" << std::endl;
                }

            }); 
        }

        try 
        {
            //start endless loop
            printf("Bot username: %s\n", bot->getApi().getMe()->username.c_str());
            bot->getApi().deleteWebhook();
            
            TgBot::TgLongPoll longPoll(*bot);


            while (true) 
            {
                longPoll.start();
            }
        } 
        catch (std::exception& e) 
        {
            printf("error: %s\n", e.what());
        }
    }
    else
    {
        //We do not wait for telegram input, we are in fire and forget-mode

        bool messageSend=false;
        int errorCounter=0;
        std::string error="";
        do
        {
            try {
                //A previous try failed, send an error message to all users
                if(errorCounter>0)
                {
                    for(auto &u : sessionManager->getUserList())
                        bot->getApi().sendMessage(u->chatId,"Last message failed to send, retry number "+std::to_string(errorCounter) + " was successfull. Last error: \"" + error+"\""); //Karrn
                }

                //send a message if requested
                if(message!="")
                {
                    for(auto &u : sessionManager->getUserList())
                        bot->getApi().sendMessage(u->chatId,message);
                }

                //send a sticker by ID if requested
                if(stickerId!="")
                { 
                    for(auto &u : sessionManager->getUserList())
                        bot->getApi().sendSticker(u->chatId,stickerId);
                }

                //send a sticker by name if requested
                if(stickerName!="")
                {
                    std::shared_ptr<Sticker> s = stickerManager.getStickerByName(stickerName); //sticker id lookup
                    if(s)
                    {
                        for(auto &u : sessionManager->getUserList())
                            bot->getApi().sendSticker(u->chatId,s->getId());
                    }   
                }

                //send a file if requested
                if(fileName!="")
                {
                    TgBot::InputFile::Ptr f = TgBot::InputFile::fromFile(fileName,"");
                    for(auto &u : sessionManager->getUserList())
                            bot->getApi().sendDocument(u->chatId,f);
                }

                messageSend = true;
            }
            catch (std::exception& e) 
            {
                printf("error: %s\n", e.what());
                error=e.what();
                sleep(5);
                errorCounter++;
            }
        }
        while(!messageSend && errorCounter < 6); //try for up to 30s

    }
    

    return 0;
}
