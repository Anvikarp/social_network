#include "reaction.h"

#include <string>

// CONSTRUCTORS 

Reaction::Reaction(){
    userId_ = -1;
    reactionType_ = ""; 
}

Reaction::Reaction(int userId, std::string reactionType){
    userId_ = userId;
    reactionType_ = reactionType; 
} 

// GETTERS 

int Reaction::getUserId(){
    return userId_; 
}


std::string Reaction::getReactionType(){
    return reactionType_; 
}

std::string Reaction::getReactionEmoji() {
    
    if (reactionType_ == "love") {
        return u8"❤️";
    } else if (reactionType_ == "laughing") {
        return u8"😂";
    } else if (reactionType_ == "wow") {
        return u8"😮";
    } else if (reactionType_ == "sad") {
        return u8"😢";
    } else if (reactionType_ == "angry") {
        return u8"😠";
    } else {
        // return the original string 
        return reactionType_;
    }
}

// SETTERS

void Reaction::setReactionType(std::string reactionType){
    reactionType_ = reactionType; 
}
