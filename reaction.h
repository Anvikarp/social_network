#ifndef REACTION_H
#define REACTION_H

#include<string>

class Reaction{

    private:
    int userId_; // ID of the user who made this reaction
    std::string reactionType_; // type of reaction as a string

    public:

    // CONSTRUCTORS

    // pre: none
    // post: create an empty reaction
    Reaction();

    // pre: the user who made the reaction and their reaction
    // post: create a reaction with the values
    Reaction(int userId, std::string reactionType); 

    // GETTERS 

    // pre: none
    // post: return the user who gave the reaction
    int getUserId(); 

    // pre: none
    // post: return the reaction type
    std::string getReactionType(); 

    // pre: the reaction in string form
    // post: change the reaction type to reactionType
    void setReactionType(std::string reactionType);

    // pre: none
    // post: returns the emoji representation of the string
    std::string getReactionEmoji(); 
};

#endif 