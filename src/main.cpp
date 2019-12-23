#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <chrono>
#include <thread>

#include <tgbot/tgbot.h>


#include <boost/program_options.hpp>
#include "stickermanager.h"
#include "sticker.h"
#include "sessionmanager.h"
#include <memory>
using namespace std;
//using namespace TgBot;

double temperature;

namespace po = boost::program_options;


int main(int argc, char * argv[]) 
{
    StickerManager sm;
    std::unique_ptr<SessionManager> sessionManager;

    std::string newToken="";
    std::string message="";
    std::string stickerId="";
    std::string stickerName="";
    std::string sessionName="";
    std::string fileName="";
    bool waitForNewUsers=false;
    bool addSticker=false;

    po::options_description desc("Allowed options");
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
        cout << desc << endl;
        return -1;
    }
    po::notify(vm);    

    if (vm.count("help")) 
    {
        cout << desc << "\n";
        return 1;
    }

    if(vm.count("add-user"))
    {
        cout << "Wait for new useres" << endl;
        waitForNewUsers=true;
    }

    if(vm.count("add-sticker"))
    {
        cout << "Showing sticker IDs" << endl << "Please send stickers";
        addSticker=true;
    }




    if(vm.count("session"))
    {
        sessionName = vm["session"].as<std::string>();
    }
    else
    {
        sessionName = "default";
        cout << "No sessionname set, using 'default'" << endl;
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
        cout << "No valid token set, abort" << endl;
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

    if(!waitForNewUsers && !addSticker && message=="" && stickerId == "" && stickerName == "" && fileName=="")
    {
        cout << "No message specified" << endl;
        return -1;
    }
    
    

    TgBot::Bot* bot;

    try 
    {
        bot = new TgBot::Bot(sessionManager->getToken());
    }
    catch(const std::exception& e)
    {
        cout << "handler!" << endl;
        std::cerr << e.what() << '\n';
        return -1;
    }

    

    

    if(waitForNewUsers || addSticker)
    {
        if(waitForNewUsers)
        {
            bot->getEvents().onCommand("start", [&bot,&sessionManager](TgBot::Message::Ptr message)
            {
                cout << "Got new user, id:"  << message->chat->id << endl;

                std::shared_ptr<User> newUser = std::make_shared<User>(message->chat->username,message->chat->id);
                sessionManager->addUser(newUser);
            });
        }

        if(addSticker)
        {
            bot->getEvents().onAnyMessage([&bot,&sm](const TgBot::Message::Ptr msg){
                if(msg->sticker)
                {
                    cout << "Got message with sticker id: " << msg->sticker->fileId << endl;
                    cout << "Please enter name for this sticker: ";
                    std::string stickername;

                    cin >> stickername;
                    cout << endl << "New name:" << stickername << endl;

                    std::shared_ptr<Sticker> newSticker = std::make_shared<Sticker>(stickername,msg->sticker->fileId);
                    sm.addSticker(newSticker);
                    
                    
                }
                else
                    cout << "Got message with invalid sticker id" << endl;

            }); 
        }

        try 
        {
            printf("Bot username: %s\n", bot->getApi().getMe()->username.c_str());
            bot->getApi().deleteWebhook();
            
            TgBot::TgLongPoll longPoll(*bot);


            while (true) 
            {
                longPoll.start();
            }
        } 
        catch (exception& e) 
        {
            printf("error: %s\n", e.what());
        }
    }
    else
    {
        std::cout << "Send message " << message << endl;

        bool messageSend=false;
        int seconds=0;
        std::string error="";
        do
        {
            try {



                if(seconds>0)
                {
                    

                    for(auto &u : sessionManager->getUserList())
                        bot->getApi().sendMessage(u->chatId,"Last message failed to send, retry number "+std::to_string(seconds) + " was successfull. Last error: \"" + error+"\""); //Karrn
                    
                  }

                if(message!="")
                {
                    for(auto &u : sessionManager->getUserList())
                        bot->getApi().sendMessage(u->chatId,message);
                }

                if(stickerId!="")
                {
                    
                    for(auto &u : sessionManager->getUserList())
                        bot->getApi().sendSticker(u->chatId,stickerId);
                }

                if(stickerName!="")
                {
                    std::shared_ptr<Sticker> s = sm.getStickerByName(stickerName);
                    if(s)
                    {
                        for(auto &u : sessionManager->getUserList())
                            bot->getApi().sendSticker(u->chatId,s->getId());
                    }   
                }

                if(fileName!="")
                {
                    TgBot::InputFile::Ptr f = TgBot::InputFile::fromFile(fileName,"");
                    for(auto &u : sessionManager->getUserList())
                            bot->getApi().sendDocument(u->chatId,f);
                }

                messageSend = true;
            }
            catch (exception& e) 
            {
                printf("error: %s\n", e.what());
                error=e.what();
                sleep(2);
                seconds++;
            }
        }
        while(!messageSend);

    }
    

    return 0;
}
