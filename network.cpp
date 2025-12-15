#include "network.h"
#include "user.h"

#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>  // for reverse
#include <queue>

// default constructor
Network::Network(): users_() {}

// get a user from id
User* Network::getUser(int id){
    if (id < 0 || id > users_.size()-1) return nullptr;
    return users_[id];
}

// add a user to the network
void Network::addUser(User* user){
    if (user != nullptr) {
        users_.push_back(user);
    }
}

// adds a connections between 2 users
int Network::addConnection(std::string s1, std::string s2){
    int id1 = getId(s1);
    int id2 = getId(s2);

    // if they don't exist in the network
    if (id1 == -1 || id2 == -1)
        return -1;

    // add edges to both user ids
    users_[id1] -> addFriend(id2);
    users_[id2] -> addFriend(id1);

    return 0;
}

int Network::deleteConnection(std::string s1, std::string s2){
    int id1 = getId(s1);
    int id2 = getId(s2);

    // if they don't exist in the network
    if (id1 == -1 || id2 == -1)
        return -1;

    // delete edges from both user ids
    users_[id1] -> deleteFriend(id2);
    users_[id2] -> deleteFriend(id1);

    return 0;
}

// loop through the users_ vector, get the name of the users and check if it's right
int Network::getId(std::string name){
    for (int i =0; i < users_.size(); i++){
        if (users_[i]->getName() == name)
            return i;
    }
    // user not found in network
    return -1;
}

// get the size of the network
int Network::numUsers(){
    return users_.size();
}

// get the users and friendships from file to network
void Network::readUsers(const char* fname){
    // read the file
    std::ifstream myfile(fname);

    // throw error if it's not opening
    if(!myfile.is_open()){
        std::cerr << "Error. Could not read file " << fname << std::endl;
        return;
    }

    // make the users_ vector empty so it can be freshly filled with the file info
    users_.clear();
    std::string myline;
    int numUsers;

    std::getline(myfile, myline);
    // reading only the first integer
    std::stringstream(myline) >> numUsers;

    for (int i = 0; i < numUsers; i++){
        int id, year, zip;
        std::string name;
        std::set<int> friends;

        // get id (no leading tab)
        std::getline(myfile, myline);
        // reads as integer
        std::stringstream(myline) >> id;

        // get name
        std::getline(myfile, myline);
        if(myline.size() && myline[0] == '\t')
            myline = myline.substr(1); // remove leading tab
        // store the name without the tab
        name = myline;

        // get year
        std::getline(myfile, myline);
        if(myline.size() && myline[0] == '\t')
            myline = myline.substr(1);
        std::stringstream(myline) >> year;

        // get zip
        std::getline(myfile, myline);
        if(myline.size() && myline[0] == '\t')
            myline = myline.substr(1);
        std::stringstream(myline) >> zip;

        // get friend list
        std::getline(myfile, myline);
        if(myline.size() && myline[0] == '\t')
            myline = myline.substr(1);
        // don't know how many friends in the list so read until not more int
        std::stringstream ss(myline);
        int f;
        while (ss >> f)
            friends.insert(f);

        // save the new user
        User* utemp = new User(id, name, year, zip, friends);
        users_.push_back(utemp);
    }
    myfile.close();
}

// put the users and friendships from network to file
void Network::writeUsers(const char* fname){

    std::ofstream myfile(fname);

    // throw error if it's not opening
    if(!myfile.is_open()){
        std::cerr << "Error. Could not write to file" << fname << std::endl;
        return;
    }

    myfile << users_.size() << std::endl;

    for (int i = 0; i < users_.size(); i++){
        User* utemp = users_[i];

        myfile << utemp->getId() << std::endl;
        // add tabs to follow format
        myfile << '\t' << utemp->getName() << std::endl;
        myfile << '\t' << utemp->getYear() << std::endl;
        // fill with 0s first and take in only 5 characters
        myfile << '\t' << std::setfill('0') << std::setw(5) << utemp->getZip() << std::endl;

        // write the friend list
        myfile << "\t";
        std::set<int>& friends = utemp->getFriends();
        // traverse the set
        int count = 0;
        for(std::set<int>::iterator it = friends.begin(); it != friends.end(); ++it){
            // add a space before each friend id except first one
            if (count > 0) myfile << " ";
            // dereference the iterator and get the actual friend id
            myfile << *it;
            count++;
        }
        myfile << std::endl;
    }
    myfile.close();
}

// shortest path between to users
std::vector<int> Network::shortestPath(int from, int to){
    // handle invalid input
    if (from < 0 || from >= users_.size() || to < 0 || to >= users_.size()) {
        return {};
    }

    std::queue<int> q;
    std::vector<bool> visited (users_.size(), false);
    std::vector<int> prev(users_.size(), -1);

    visited[from] = true;
    q.push(from);

    // bfs
    while (q.size() >0){
        int cur = q.front();
        q.pop();
        // users_[cur] is User*, need to loop through friends which are stored in std::set<int>
        for (auto neighbor: users_[cur]->getFriends()){
            if(!visited[neighbor]){
                prev[neighbor] = cur;
                visited[neighbor] = true;
                q.push(neighbor);
            }
        }
    }
    // if to was never reached, return empty vector
    if (!visited[to]) return {};

    std::vector<int> output;
    int cur = to;
    while(cur != -1){
        output.push_back(cur);
        cur = prev[cur];
    }
    std:: reverse(output.begin(), output.end());
    return output;
}

// return the path of two users that have the given distance
std::vector<int> Network::distanceUser(int from, int& to, int distance){

    // handle invalid start
    if (from < 0 || from >= users_.size()) {
        to = -1;
        return {};
    }

    // BFS
    std::queue<int> q;
    std::vector<bool> visited (users_.size(), false);
    std::vector<int> dist(users_.size(), -1);

    q.push(from);
    visited[from] = true;
    dist[from] = 0;

    while (q.size() > 0){
        int cur = q.front();
        q.pop();
        for(int neighbor : users_[cur]->getFriends()){
            if(!visited[neighbor]){
                visited[neighbor] = true;
                dist[neighbor] = dist[cur]+1;
                q.push(neighbor);

                // if the distance has been reached, get the shortest path
                if (dist[neighbor] == distance) {
                    to = neighbor;
                    return shortestPath(from, neighbor);
                }
            }
        }
    }

    to = -1;
    return {};
}

// suggest friends with the most mutual friend connections
std::vector<int> Network::suggestFriends(int who, int& score){

    std::vector<int> suggestions;
    score = 0;

    // make sure that who is a valid id
    if(who < 0 || who >= users_.size()) return suggestions;

    std::set<int> whofriends = users_[who]->getFriends();

    for (int i = 0; i < users_.size(); i++){
        // skip who and who's friends
        if (i == who || whofriends.count(i)) continue;

        std::set<int> tempfriends = users_[i]->getFriends();
        int curscore = 0;

        for (int f : tempfriends)
            if (whofriends.count(f)) curscore++;

        if (curscore > 0) {
            if (curscore > score) {
                // reset list if there are higher scores
                score = curscore;
                suggestions.clear();
                suggestions.push_back(i);
            } else if (curscore == score) {
                // add all users with same high score
                suggestions.push_back(i);
            }
        }

    }
    return suggestions;
}

// get the groups within the graph
std::vector<std::vector<int>> Network::groups() {

    std::vector<std::vector<int>> allGroups;
    std::vector<bool> visited(users_.size(), false);

    for (int i = 0; i < users_.size(); i++) {
        if (!visited[i]) {
            std::vector<int> group;
            DFSHelper(i, visited, group);
            // add the groups made by DFShelper
            allGroups.push_back(group);
        }
    }

    return allGroups;
}

// helper function for groups()
void Network::DFSHelper(int cur, std::vector<bool>& visited, std::vector<int>& group) {
    visited[cur] = true;
    // starting cur's group
    group.push_back(cur);

    for (int neighbor : users_[cur]->getFriends()) {
        if (!visited[neighbor]) {
            // recursively check the members of the group
            DFSHelper(neighbor, visited, group);
        }
    }
}

// add post and update messageId_
void Network::addPost(Post* post){
    int profileId = post->getProfileId();

    // calculate the messageId_ by getting the total posts right now
    int id = 0;
    for (const auto& user_posts : posts_) {
        id += user_posts.size();
    }

    post->setMessageId(id);

    // avoid problems with size_t by casting to int
    if (profileId >= static_cast<int>(posts_.size()))
        posts_.resize(profileId+1);

    posts_[profileId].push_back(post);
}


// return the posts of the user id given
std::vector<Post*> Network::getPosts(int id){
    if (id >= 0 && id <= static_cast<int>(posts_.size()-1))
        return posts_[id];
    return {};
}

// return a string for a post
std::string Network::postDisplayString(Post* post){

    int authorId = post->getAuthorId();

    // invalid id
    if (authorId < 0 || authorId >= static_cast<int>(users_.size()))
        return "Anon wrote: " + post->toString();

    return users_[authorId]->getName() + " wrote: " + post->toString();
}

std::string Network::getPostsString(int profileId, int howMany){
    // invalid id
    if (profileId < 0 || profileId >= static_cast<int>(posts_.size())) return "";

    int totalPosts = static_cast<int>(posts_[profileId].size());

    int start = std::max(0, totalPosts - howMany);

    std::string res;

    // show most recent posts first
    for (int i= totalPosts-1; i >= start; i--){
        res += postDisplayString(posts_[profileId][i]) + "\n\n";
    }
    return res;
}

// reading the posts from a file
int Network::readPosts(char* fname){
    std::ifstream fin(fname);

    if(!fin.is_open()){
        std::cerr << "Error. Could not read file " << fname << std::endl;
        return -1;
    }

    // clear the posts_ vector
    posts_.clear();

    int numPosts;
    fin >> numPosts;
    fin.ignore();

    for (int i = 0; i < numPosts; i++){
        std::string message, url, reactionLine;
        int messageId, profileId, authorId, likes;

        // read messageId
        fin >> messageId;
        fin.ignore();

        // read message
        getline(fin, message);
        if (message.size() && message[0] == '\t')
            message = message.substr(1);

        // read profileId, authorId, likes
        fin >> profileId;
        fin >> authorId;
        fin >> likes;
        fin.ignore();

        // read URL line (empty for regular posts)
        getline(fin, url);
        if (url.size() && url[0] == '\t')
            url = url.substr(1);

        // create the post
        Post* p = nullptr;
        if (url.empty()) {
            p = new Post(profileId, authorId, message, likes);
        } else {
            p = new LinkPost(profileId, authorId, message, likes, url);
        }
        p->setMessageId(messageId);

        // read the reaction line (line 8 in format)
        getline(fin, reactionLine);
        if (reactionLine.size() && reactionLine[0] == '\t')
            reactionLine = reactionLine.substr(1);

        // parse reactions: format is "4 love 1 wow 2 sad"
        if (!reactionLine.empty()) {
            std::stringstream ss(reactionLine);
            int count;
            std::string reactionType;

            // Loop continues as long as it can successfully read an int (count) and a string (type)
            while (ss >> count >> reactionType) {
                // Create 'count' number of reactions with placeholder user IDs
                for (int j = 0; j < count; j++) {
                    // Using a unique negative ID for each placeholder reaction
                    p->addReaction(-(messageId * 1000 + j), reactionType);
                }
            }
        }

        // make sure the profile can fit
        if (profileId >= static_cast<int>(posts_.size()))
            posts_.resize(profileId + 1);
        
        // add to posts
        posts_[profileId].push_back(p);
    }
    
    fin.close();
    return 0;
}

bool Network::comparePosts(Post* a, Post* b) {
    return a->getMessageId() < b->getMessageId();
}

// write posts to file
int Network::writePosts(char* fname){
    std::ofstream fout(fname);

    if(!fout.is_open()){
        std::cerr << "Error. Could not write to file " << fname << std::endl;
        return -1;
    }

    std::vector<Post*> allposts;
    for (size_t i = 0; i < posts_.size(); i++)
        for (size_t j = 0; j < posts_[i].size(); j++)
            allposts.push_back(posts_[i][j]);

    std::sort(allposts.begin(), allposts.end(), comparePosts);

    // write total posts
    fout << allposts.size() << "\n";
    
    for (size_t i = 0; i < allposts.size(); i++){
        // write messageId
        fout << allposts[i]->getMessageId() << "\n";
        
        // write message
        fout << "\t" << allposts[i]->getMessage() << "\n";
        
        // write profileId
        fout << "\t" << allposts[i]->getProfileId() << "\n";
        
        // write authorId
        fout << "\t" << allposts[i]->getAuthorId() << "\n";
        
        // write likes
        fout << "\t" << allposts[i]->getLikes() << "\n";

        // write URL (empty line for regular post, URL for LinkPost)
        if (allposts[i]->getURL().empty())
            fout << "\t\n";
        else
            fout << "\t" << allposts[i]->getURL() << "\n";

        // write reactions in the format: "4 love 1 wow 2 sad"
        fout << "\t";
        
        // get counts for each reaction type
        std::vector<std::string> reactionTypes = {"love", "wow", "sad", "angry", "laughing"};
        bool firstReaction = true;
        
        for (const std::string& type : reactionTypes) {
            int count = allposts[i]->getReactionCountByType(type);
            if (count > 0) {
                if (!firstReaction) {
                    fout << " ";  // add space between reactions
                }
                fout << count << " " << type;
                firstReaction = false;
            }
        }
        
        fout << "\n"; 
    }
    
    fout.close();
    return 0;
}

// FUNCTIONS FOR LIKES

void Network::addLikeToPost(int messageId, int userId){
    Post* post = getPostByMessageId(messageId);
    if (post != nullptr) {
        post->addLike(userId);
    }
}

void Network::removeLikeFromPost(int messageId, int userId){
    Post* post = getPostByMessageId(messageId);
    if (post != nullptr) {
        post->removeLike(userId);
    }
}

// FUNCTIONS FOR REACTIONS

void Network::addReactionToPost(int messageId, int userId, std::string reactionType){
    Post* post = getPostByMessageId(messageId);
    if (post != nullptr) {
        post->addReaction(userId, reactionType);
    }
}

void Network::removeReactionFromPost(int messageId, int userId){
    Post* post = getPostByMessageId(messageId);
    if (post != nullptr) {
        post->removeReaction(userId);
    }
}

void Network::changeReactionOnPost(int messageId, int userId, std::string newReactionType){
    Post* post = getPostByMessageId(messageId);
    if (post != nullptr) {
        post->changeReaction(userId, newReactionType);
    }
}

// helper function to find post by messageId
Post* Network::getPostByMessageId(int messageId) {
    // search through all users' posts
    for (size_t i = 0; i < posts_.size(); i++) {
        for (size_t j = 0; j < posts_[i].size(); j++) {
            if (posts_[i][j]->getMessageId() == messageId) {
                return posts_[i][j];
            }
        }
    }
    return nullptr;
}
