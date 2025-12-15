#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <string>
#include <vector>

#include "user.h"
#include "post.h"
#include "reaction.h"


class Network{

private:
std::vector<User*> users_; 

// each user and their posts
std::vector<std::vector<Post*>> posts_; 

// pre: current vertex cur, boolean vertex of visited vertices, vector of group
// post: does DGS on user cur and finds all users connect to it
void DFSHelper(int cur, std::vector<bool>& visited, std::vector<int>& group); 



public:

// default constructor
// pre: none
// post: creates an empty Network with no users
Network(); 

// pre: id is a valid index in users_ (0 <= id < users_.size())
// post: returns a pointer to the User with the given id
User* getUser(int id); 

// pre: user is a valid, non-null User pointer
// post: appends user to the end of users_
void addUser(User* user); 

// pre: s1 and s2 (first and last) are valid names of users in the network
// post: if both users exist, adds a friendship connection between them;
//       returns 0 on success, -1 if either user is not found
int addConnection(std::string s1, std::string s2); 

// pre: s1 and s2 (first and last) are valid names of users in the network
// post: if both users exist and are connected, removes the connection;
//       returns 0 on success, -1 if either user is not found
int deleteConnection(std::string s1, std::string s2);

// pre: a valid string name (first and last)
// post: returns the id associated with the users' name 
//       or -1 if user doesn't exist
int getId(std::string name);

// pre: none
// post: returns the number of users in the network
int numUsers(); 

// pre: fname points to a valid input file
// post: network is loaded with users and friends from the file
void readUsers(const char* fname); 

// pre: fname points to a valid file
// post: transfer the users and friends from network to file
void writeUsers(const char* fname);

// pre: User 'from' and 'to'
// post: return a vector containing the shortest path between the given vertices
std::vector<int> shortestPath(int from, int to); 

// pre: starting user, the distance, reference parameter to which will be set to the user found
// post: vector of ids of the users on the shortest path from to to, 
// return empty vector set to to -1 if the user wasn't found
std::vector<int> distanceUser(int from, int& to, int distance); 

// pre: User who and reference parameter score which will be set to the highest score found
// post: return a list of friend suggestions for who, they must not already be friend
// and they will have the highest scores (number of mutual friends) 
// return empty vector and score = 0 if no suggestions
std::vector<int> suggestFriends(int who, int& score); 

// pre: none
// post: comput connected components of the network
//  (group of users all connected to each other, finding groups within the graph)
std::vector<std::vector<int> > groups(); 

// pre: a new post object
// post: add the new post to to the vector and assign a messageId
void addPost(Post* post);

// pre: user id
// post: return the posts of user id given
std::vector<Post*> getPosts(int id); 

// pre: a post
// post: return [author name] wrote: [toString]” to be displayed for post
std::string postDisplayString(Post* post); 

// pre: id or the person and the count of posts
// post: return a string with the most recent howMany number posts for profileId
std::string getPostsString(int profileId, int howMany); 

// pre: file to read from
// post: read the file
int readPosts(char* fname); 

// pre: two posts 
// post: helper func for writePosts, comparison for custom sorting by messageId
static bool comparePosts(Post* a, Post* b);

// pre: file to read to
// post: write to the file
int writePosts(char* fname); 

// FUNCTIONS FOR LIKES

// pre: id of post liked by userId
// post: adds a like to the post
void addLikeToPost(int messageId, int userId);

// pre: id of post liked by userId
// post: removes a like from the post
void removeLikeFromPost(int messageId, int userId);

// FUNCTIONS FOR REACTIONS

// pre: id of post liked by userId and reaction given
// post: adds the reaction
void addReactionToPost(int messageId, int userId, std::string reactionType);

// pre: id of post liked by userId
// post: removes the reaction
void removeReactionFromPost(int messageId, int userId);

// pre: id of post liked by userId and new reaction
// post: changes the reaction
void changeReactionOnPost(int messageId, int userId, std::string newReactionType);

// pre: id of the post
// post: helper function to get post by messageId
Post* getPostByMessageId(int messageId);

};
#endif
