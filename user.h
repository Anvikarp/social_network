#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include <set>


class User{

private:

    int id_; 
    std::string name_; 
    int year_; 
    int zip_; 
    std::set<int> friends_; 


public:

    // default constructor
    // pre: none
    // post: creates a User filled with arbitrary values
    User();

    // primary constructor that takes in 5 args
    // pre: id, year, and zip are valid values;
    //      name is a non-empty string containing first and last name;
    //      friends is a valid set of user IDs.
    // post: creates an User with all 5 values defined
    User(int id, std::string name, int year, int zip, std::set<int> friends); 

    //GETTERS

    // pre: none
    // post: returns the User's id
    int getId() const; 

    // pre: none
    // post: returns the User's name
    std::string getName() const; 

    // pre: none
    // post: returns the User's birth year
    int getYear() const; 

    // pre: none
    // post: returns the User's zip code
    int getZip() const; 

    // pre: none
    // post: returns a reference to the User's friend list
    std::set<int>& getFriends();

    // pre: id is a valid user
    // post: inserts the id if it doesn't exist in the set
    void addFriend(int id);

    // pre: id is a valid user
    // post: deletes the id if it already exists in the set
    void deleteFriend(int id);

};
#endif
