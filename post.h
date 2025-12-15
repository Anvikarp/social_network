#ifndef POST_H
#define POST_H

#include<string>
#include<set>
#include <map>
#include <vector>
#include "reaction.h"

class Post{

    private: 

    int messageId_; 
    int profileId_; 
    int authorId_; 
    std::string message_; 
    int likes_; 
    std::set<int> likedByUsers_; // user ids who have liked the post
    std::vector<Reaction*> reactions_; // all emoji reactions to the post

    public: 

    // pre: none
    // post: constructs empty Post object
    Post(); 

    // pre: int profileId, int authorId is the id of the person who made the post, 
    // string message is the post, int likes
    // post: Post object with all values custom filled and messageId = -1
    Post(int profileId, int authorId, std::string message, int likes);


    // GETTERS

    // pre: none
    // post: messageId_
    int getMessageId();

    // pre: none
    // post: profileId_
    int getProfileId();

    // pre: none
    // post: authorId_
    int getAuthorId();

    // pre: none
    // post: message_
    std::string getMessage();

    // pre: none
    // post: likes_
    int getLikes();

    // pre: none
    // post: return an empty string
    virtual std::string getURL();

    //SETTER

    // pre: new id
    // post: changes messageId_
    void setMessageId(int id);
    
   
    //pre: none
    // post: return string ”[message_] ([likes_] likes, reaction summary )”
    virtual std::string toString();

    // FUNCTIONS FOR LIKES

    // pre: the user who liked
    // post: adds a like
    void addLike(int userId); 

    // pre: user who liked
    // post: remove the like
    void removeLike(int userId);

    // pre: none
    // post: returns all users who liked the post
    std::set<int> getLikedByUsers();

    // FUNCTIONS FOR REACTIONS

    // pre: user who reacted and their reaction
    // post: adds reaction
    void addReaction(int userId, std::string reactionType);

    // pre: user who reacted
    // post: removes reaction
    void removeReaction(int userId);

    // pre: user who reacted
    // post: returns user's reaction
    Reaction* getUserReaction(int userId);

    // pre: user's id and new reaction
    // post: change user's reaction
    void changeReaction(int userId, std::string newReactionType);

    // pre: none
    // post: returns all reactions for the post
    std::vector<Reaction*> getReactions();

    // pre: reaction type
    // post: count of the reaction type
    int getReactionCountByType(std::string reactionType);
    
    // pre: user to check
    // post: checks if the user has reacted
    bool hasUserReacted(int userId); 


};

class LinkPost: public Post{

    private:
    std::string url_; 

    public:

    // pre: none
    // post: creates a LinkPost object
    LinkPost(); 

    // pre: same as Post constructor + url 
    // post: LinkPost object custom filled  
    LinkPost(int profileId, int authorId, std::string message, int likes, std::string url); 
    
    // pre: none
    // post: get url
    std::string getURL();

    // pre: none
    // post: returns [message_] (url: [url_]) ([likes_] likes)
    std::string toString();
};

#endif
