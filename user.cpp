#include "user.h"

// default constructor
User::User() : id_(0), name_("Clara"), year_(1999), zip_(95053), friends_() {}

// primary constructor
User::User(int id, std::string name, int year, int zip, std::set<int> friends)
    : id_(id), name_(name), year_(year), zip_(zip), friends_(friends) {}

// GETTERS

int User::getId() const {
    return id_;
}

std::string User::getName() const {
    return name_;
}

int User::getYear() const {
    return year_;
}

int User::getZip() const {
    return zip_;
}

std::set<int>& User::getFriends() {
    return friends_;
}

// insert friend into set
void User::addFriend(int id) {
    friends_.insert(id);  // set ignores duplicates
}

// erase friend from set
void User::deleteFriend(int id) {
    friends_.erase(id);   // erase does nothing if id not present
}
