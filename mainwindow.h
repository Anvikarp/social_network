#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "user.h"
#include "network.h"
#include <QMainWindow>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // pre: none
    // post: shows (setVisible) and hides itmes on the ui when needed
    void display();

    // pre: none
    // post: checks if the name entered exists in the social network
    void login();

    // pre: none
    // post: go back to loggedInUser's profile page
    void profileButton();

    // pre: int row and column (need col for cellClicked func) of the cell that's been clicked on
    // post: click on a cell from friendsTable and go to their profile
    void friendsPage(int row, int col);

    // pre: none
    // post: add a friend connection between loggedInUser and displayed profile
    void addFriendProfile();

    // pre: int row and column of the cell that's been clicked on
    // post: add friend connection between loggedInUser and suggested friend
    void addSuggested(int row, int col);

    // pre: none
    // post: create a display a new post in the according profile
    void addPost();

    // pre: message id being liked, is the button clicked
    // post: changes the number of likes on the post
    void on_likeButton_clicked(int messageId, bool isLiked);

    // pre: the message id being reacted to, boolean for has reaction, button being clicked
    // post: changes the number of reactions on the post
    void on_reactButton_clicked(int messageId, bool hasReacted, QPushButton* button);

private:
    Ui::MainWindow *ui;
    Network network;
    User* loggedInUser = nullptr;
    User* displayUser = nullptr;
};
#endif // MAINWINDOW_H
