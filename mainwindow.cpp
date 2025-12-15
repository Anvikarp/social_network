#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMenu>
#include <QPoint>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // start the network
    network.readUsers("users.txt");
    network.readPosts("posts.txt");

    // start on the login page, first page of the stack
    ui->stackedWidget->setCurrentIndex(0);

    // connect the loginButton
    connect(ui->loginButton,
            &QPushButton::clicked,
            this,
            &MainWindow::login);

    // connect the profile page stuff
    connect(ui->myProfileButton,
            &QPushButton::clicked,
            this,
            &MainWindow::profileButton);

    // connect the cells of friends table so friend pages can come up
    connect(ui->friendsTable,
            &QTableWidget::cellClicked,
            this,
            &MainWindow::friendsPage);

    // connect the cells of suggested table so friend pages can come up
    connect(ui->suggestionsTable,
            &QTableWidget::cellClicked,
            this,
            &MainWindow::addSuggested);

    // connect the add friend button shown on non-friend pages
    connect(ui->addFriendButton,
            &QPushButton::clicked,
            this,
            &MainWindow::addFriendProfile);

    // connect the add post button
    connect(ui->addPostButton,
            &QPushButton::clicked,
            this,
            &MainWindow::addPost);

}


void MainWindow::display() {

    // not successfully logged in yet
    if (loggedInUser == nullptr) {
        ui->stackedWidget->setCurrentIndex(0);
        return;
    }

    // check if viewing loggedInUser's profile
    bool isMyProfile = (loggedInUser->getId() == displayUser->getId());

    if (isMyProfile)
        ui->profileLabel->setText("My Profile");
    else
        ui->profileLabel->setText(QString::fromStdString(displayUser->getName()) +
                                  "'s Profile");

    // showing friends of user in the table
    ui->friendsTable->clearContents();
    ui->friendsTable->setColumnCount(1); //1 column just for friends
    ui->friendsTable->setHorizontalHeaderLabels(QStringList() << "Friends"); // col label

    // get friends ids and insert friend User into table
    std::set<int>& friends = displayUser->getFriends();
    ui->friendsTable->setRowCount(friends.size());
    int row = 0;
    for (int friendId : friends) {
        User* friendUser = network.getUser(friendId);
        // valid users only
        if (friendUser) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::fromStdString(friendUser->getName()));
            ui->friendsTable->setItem(row, 0, item); //int, row col, QTableWidgetItem* item
            row++;
        }
    }
    // adjusts with to be as long as the longest item name
    ui->friendsTable->resizeColumnsToContents();

    // showing 5 most recent posts
    std::vector<Post*> posts = network.getPosts(displayUser->getId());
    int numPostsToShow = std::min((int)posts.size(), 5);

    ui->postsTable->clearContents();
    ui->postsTable->setColumnCount(3);
    ui->postsTable->setHorizontalHeaderLabels(QStringList() << "Post" << "Like" << "React");
    ui->postsTable->setRowCount(numPostsToShow);

    int totalPosts = posts.size();
    int startIdx = std::max(0, totalPosts - 5);

    for (int i = 0; i < numPostsToShow; i++) {
        int postIdx = totalPosts - 1 - i;
        Post* post = posts[postIdx];

        std::string postString = network.postDisplayString(post);
        int messageId = post->getMessageId();

        // column 0 is the post itself
        QTableWidgetItem* postItem = new QTableWidgetItem(QString::fromStdString(postString));
        postItem->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
        ui->postsTable->setItem(i, 0, postItem);

        // column 1 is like
        QPushButton* likeButton = new QPushButton(this);
        bool isLiked = post->getLikedByUsers().count(loggedInUser->getId());
        likeButton->setText(isLiked ? "Unlike" : "Like");

        likeButton->setFixedWidth(70);
        likeButton->setFixedHeight(30);

        connect(likeButton, &QPushButton::clicked, this, [this, messageId, isLiked]() {
            this->on_likeButton_clicked(messageId, isLiked);
        });
        ui->postsTable->setCellWidget(i, 1, likeButton);

        // column2 is reaction
        QPushButton* reactButton = new QPushButton(this);
        Reaction* userReaction = post->getUserReaction(loggedInUser->getId());
        bool hasReacted = userReaction != nullptr;
        if (userReaction) {
            reactButton->setText(QString::fromStdString(userReaction->getReactionType()));
        } else {
            reactButton->setText("React");
        }

        reactButton->setFixedWidth(70);
        reactButton->setFixedHeight(30);

        connect(reactButton, &QPushButton::clicked, this, [this, messageId, hasReacted, reactButton]() {
            this->on_reactButton_clicked(messageId, hasReacted, reactButton);
        });
        ui->postsTable->setCellWidget(i, 2, reactButton);
    }

    ui->postsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    // Set the button columns to resize to fit their content
    ui->postsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->postsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    ui->postsTable->setWordWrap(true);
    ui->postsTable->resizeRowsToContents();


    // show/hide the "My Profile" button
    ui->myProfileButton->setVisible(!isMyProfile);

    // only show suggestions on My Profile
    ui->suggestionsTable->setVisible(isMyProfile);
    if (isMyProfile)
    {
        int score = 0;
        // ids of suggested
        std::vector<int> suggestions = network.suggestFriends(loggedInUser->getId(), score);

        ui->suggestionsTable->clearContents();
        ui->suggestionsTable->setColumnCount(2);
        ui->suggestionsTable->setHorizontalHeaderLabels(QStringList() << "Suggested Friend" << "Mutual Friends");
        ui->suggestionsTable->setRowCount(suggestions.size());

        for (size_t i = 0; i < suggestions.size(); i++) {
            User* suggestedUser = network.getUser(suggestions[i]);
            if (suggestedUser) {
                QTableWidgetItem *nameItem = new QTableWidgetItem(QString::fromStdString(suggestedUser->getName()));
                ui->suggestionsTable->setItem(i, 0, nameItem);
                QTableWidgetItem *scoreItem = new QTableWidgetItem(QString::number(score));
                ui->suggestionsTable->setItem(i, 1, scoreItem);
            }
        }
        ui->suggestionsTable->resizeColumnsToContents();
    }
    // add friends using the button (not from suggested)
    ui->addFriendButton->setVisible(!isMyProfile);
    if (!isMyProfile) {
        // check if already friends
        bool alreadyFriends = loggedInUser->getFriends().count(displayUser->getId());
        ui->addFriendButton->setText(alreadyFriends ? "Already Friends" : "Add Friend");
        ui->addFriendButton->setEnabled(!alreadyFriends);
    }
}

// login valid users only
void MainWindow::login()
{
    QString name = ui->enterName->toPlainText();

    int id = network.getId(name.toStdString());

    if (id != -1)
    {
        loggedInUser = network.getUser(id);
        displayUser = loggedInUser;
        // clearn any previous stuff for a fresh login page
        ui->enterName->clear();
        ui->messageBox->clear();

        // go to profile page upon successful login
        ui->stackedWidget->setCurrentIndex(1);
        display();
    }
    else
    {
        ui->messageBox->setText("Unsuccessful Login. Enter a valid name.");
    }
}

void MainWindow::profileButton()
{
    // make sure that they're logged in
    if (loggedInUser != nullptr)
    {
        displayUser = loggedInUser;
        display();
    }
}

// click on the friend and go to their page
void MainWindow::friendsPage(int row, int col)
{
    QTableWidgetItem* item = ui->friendsTable->item(row, 0);
    // valid cell
    if (item) {
        QString name = item->text();
        int id = network.getId(name.toStdString());

        if (id != -1) {
            displayUser = network.getUser(id);
            display();
        }
    }

}

void MainWindow::addFriendProfile()
{
    // make sure there was a sucessful login all the time
    if (loggedInUser == nullptr || displayUser == nullptr)
        return;

    network.addConnection(loggedInUser->getName(), displayUser->getName());
    display();

}

// add friend connection when clicking on suggested table cell
void MainWindow::addSuggested(int row, int col)
{
    if (loggedInUser == nullptr) return;

    QTableWidgetItem* item = ui->suggestionsTable->item(row, 0);
    if (item) {
        QString name = item->text();

        network.addConnection(loggedInUser->getName(), name.toStdString());

        display();
    }

}

void MainWindow::addPost()
{
    if (loggedInUser == nullptr || displayUser == nullptr) return;

    QString message = ui->newPost->toPlainText();
    if (!message.isEmpty()) {
        // Post(int profileId, int authorId, std::string message, int likes)
        Post* np = new Post(displayUser->getId(), loggedInUser->getId(), message.toStdString(), 0);

        network.addPost(np);

        ui->newPost->clear();

        display();
    }

}

void MainWindow::on_likeButton_clicked(int messageId, bool isLiked)
{
    if (loggedInUser == nullptr) return;

    int userId = loggedInUser->getId();

    if (isLiked) {
        network.removeLikeFromPost(messageId, userId);
    } else {
        network.addLikeToPost(messageId, userId);
    }

    display();
}

void MainWindow::on_reactButton_clicked(int messageId, bool hasReacted, QPushButton* button)
{
    if (loggedInUser == nullptr) return;

    int userId = loggedInUser->getId();

    if (hasReacted) {
        network.removeReactionFromPost(messageId, userId);
        display();
        return;
    }

    // need to use a menu to show all types of reactions
    QMenu *reactionMenu = new QMenu(this);

    // use the reaction type as the action name and the emoji as the text
    QAction *loveAction = reactionMenu->addAction(QString::fromUtf8("❤️ Love"));
    loveAction->setProperty("reactionType", "love");
    QAction *laughingAction = reactionMenu->addAction(QString::fromUtf8("😂 Laughing"));
    laughingAction->setProperty("reactionType", "laughing");
    QAction *wowAction = reactionMenu->addAction(QString::fromUtf8("😮 Wow"));
    wowAction->setProperty("reactionType", "wow");
    QAction *sadAction = reactionMenu->addAction(QString::fromUtf8("😢 Sad"));
    sadAction->setProperty("reactionType", "sad");
    QAction *angryAction = reactionMenu->addAction(QString::fromUtf8("😠 Angry"));
    angryAction->setProperty("reactionType", "angry");

    // connect to handle the menu item click
    connect(reactionMenu, &QMenu::triggered, this, [this, messageId, userId](QAction *action) {
        std::string reactionType = action->property("reactionType").toString().toStdString();
        network.addReactionToPost(messageId, userId, reactionType);
        display();
    });

    // show the menu at the button's position
    reactionMenu->popup(button->mapToGlobal(QPoint(0, button->height())));
}


MainWindow::~MainWindow()
{
    // before it closes, add the changes to the files
    network.writeUsers("users.txt");
    network.writePosts("posts.txt");
    delete ui;
}
