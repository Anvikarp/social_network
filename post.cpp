#include "post.h"

#include <string>

// POST

// CONSTRUCTORS

Post::Post(): profileId_(0), messageId_(-1), authorId_(0), message_(""), likes_(0) {}

Post::Post(int profileId, int authorId, std::string message, int likes):
    profileId_(profileId), messageId_(-1), authorId_(authorId), message_(message), likes_(likes) {}


// GETTERS

int Post::getMessageId(){
    return messageId_; 
}

int Post::getProfileId(){
    return profileId_; 
}

int Post::getAuthorId(){
    return authorId_; 
}

std::string Post::getMessage(){
    return message_;
}

int Post::getLikes(){
    return likes_; 
}

// note: don't use virtual here, invalid outside of class def
std::string Post::getURL(){
    return ""; 
}

void Post::setMessageId(int id){
    messageId_ = id; 
}

std::string Post::toString() {
    std::string result = message_;
    
    // always add like and reaction summary
    result += " (";
    
    // add like count
    result += std::to_string(likes_) + " Like";
    if (likes_ != 1) result += "s";
    
    // add separator if reactions exist
    if (!reactions_.empty()) {
        result += " | ";
        
        // add reaction summary
        std::map<std::string, int> summary;
        for (Reaction* reaction : reactions_) {
            std::string type = reaction->getReactionType();
            summary[type]++;
        }
        
        bool first = true;
        for (const auto& pair : summary) {
            if (!first) result += " ";
            first = false;
            
            // get emoji for reaction type
            Reaction temp(0, pair.first);
            result += temp.getReactionEmoji() + " " + std::to_string(pair.second);
        }
    }
    
    result += ")";
    
    return result;
}


// LINKPOST

// CONSTRUCTORS 
LinkPost::LinkPost(): Post(), url_("") {}

LinkPost::LinkPost(int profileId, int authorId, std::string message, int likes, std::string url):
    Post(profileId, authorId, message, likes), url_(url) {}
    
// GETTER
std::string LinkPost::getURL(){
    return url_; 
}

std::string LinkPost::toString() {
    std::string result = getMessage() + "\n" + getURL();
    
    // always add like and reaction summary
    int numLikes = getLikes();
    std::vector<Reaction*> postReactions = getReactions();
    
    result += " (";
    
    // add like count
    result += std::to_string(numLikes) + " Like";
    if (numLikes != 1) result += "s";
    
    // add separator if reactions exist
    if (!postReactions.empty()) {
        result += " | ";
        
        // add reaction summary
        std::map<std::string, int> summary;
        for (Reaction* reaction : postReactions) {
            std::string type = reaction->getReactionType();
            summary[type]++;
        }
        
        bool first = true;
        for (const auto& pair : summary) {
            if (!first) result += " ";
            first = false;
            
            // get emoji for reaction type
            Reaction temp(0, pair.first);
            result += temp.getReactionEmoji() + " " + std::to_string(pair.second);
        }
    }
    
    result += ")";
    
    return result;
}

// FUNCTIONS FOR LIKES


void Post::addLike(int userId){
    // check if user already liked
    if (likedByUsers_.count(userId) > 0) {
        return; 
    }
    
    // add to set and increment counter
    likedByUsers_.insert(userId);
    likes_++;
} 


void Post::removeLike(int userId){
    // check if user has liked
    if (likedByUsers_.count(userId) == 0) {
        return; 
    }
    
    // remove from set and decrement counter
    likedByUsers_.erase(userId);
    likes_--;
}


std::set<int> Post::getLikedByUsers(){
    return likedByUsers_; 
}

// FUNCTIONS FOR REACTIONS


void Post::addReaction(int userId, std::string reactionType){
    if (hasUserReacted(userId)) {
        return;
    }

    Reaction* r  = new Reaction(userId, reactionType);
    reactions_.push_back(r);

}


void Post::removeReaction(int userId){
    for (int i = 0; i < reactions_.size(); i++) {
        if (reactions_[i]->getUserId() == userId) {
            delete reactions_[i]; // free memory
            reactions_.erase(reactions_.begin() + i); // gives the index
            return;
        }
    }
}


Reaction* Post::getUserReaction(int userId){
    for (int i = 0; i < reactions_.size(); i++) {
        if (reactions_[i]->getUserId() == userId){
            return reactions_[i];
        }
    }
    return nullptr;
}

void Post::changeReaction(int userId, std::string newReactionType){
    Reaction* existing = getUserReaction(userId);

    if (existing != nullptr) {
        // user has already reacted, change it
        existing->setReactionType(newReactionType);
    } else {
        // user hasn't reacted yet, add new reaction
        addReaction(userId, newReactionType);
    }
}


std::vector<Reaction*> Post::getReactions(){
    return reactions_; 
}


int Post::getReactionCountByType(std::string reactionType){
    int count = 0; 
    for (Reaction* r: reactions_){
        if (r->getReactionType() == reactionType)
            count++;
    }
    return count; 
}

bool Post::hasUserReacted(int userId){
    for (Reaction* reaction : reactions_) {
        if (reaction->getUserId() == userId) {
            return true;
        }
    }
    return false;
}
