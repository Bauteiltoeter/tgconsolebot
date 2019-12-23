#ifndef SESSIONMANAGER_H__
#define SESSIONMANAGER_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <iostream>
struct User {
    std::string user;
    uint64_t chatId;

    User(std::string user, uint64_t chatId) : user(user), chatId(chatId) {std::cout << "Created user " << user << "; id: " << chatId << std::endl;};
};

class SessionManager
{
public:
    SessionManager(std::string sessionname);

    void setToken(const std::string& token);
    const std::string& getToken();

    bool tokenValid();
    void addUser(std::shared_ptr<User> user);

    const std::list<std::shared_ptr<User>>& getUserList();
private:
    void load();
    void save();

    std::string sessionname;
    std::string token;
    std::string filename;

    std::list<std::shared_ptr<User>> userList;
};

#endif