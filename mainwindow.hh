#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include "gameboard.hh"
#include <cmath>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>
#include <QDialog>
#include <QGridLayout>
#include <QFrame>

// MainWindow size
const int WINDOW_WIDTH = 580;
const int WINDOW_HEIGHT = 700;

const QString RANDOM_INT = "Enter random integer as a seed to generate the "
                           "board,\n"
                           "                    integer must be between 1-50.";
const QString OWN_INPUT = "Enter characters space, 0 or 1, starting and ending"
            " with \". \n                Needed amount of characters: ";
const QString GAME_STARTED = "Game started, good luck!";
const QString BAD_INPUT = "Bad input, input must contain only "
                          "spaces, 0s and 1s.";
const QString WRONG_SIZE = "Input wrong size, try again.";
const QString BAD_SEED = "Bad seed, input a new seed.";
const QString ADD_OK = "Add succesfull!";
const QString ADD_NOT_OK = "Can't add";

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


private slots:


    // Initialization for all the different button clicks
    void initialize_random_board_click();
    void initialize_own_board_click();
    void start_game_click();
    void handle_button_clicks();
    void handle_reset_button_click();
    void handle_zero_button_click();
    void handle_one_button_click();
    void handle_pause_button_click();

    // Opens new window to show instructions for user.
    void handle_help_button_click();

    // Timer that starts when game starts
    void onTimerTimeout();

    // If game over, slot opens new window that shows points and time spent
    void game_over();

private:
    Ui::MainWindow *ui;

    // All the needed buttons
    QPushButton* random_board;
    QPushButton* own_board;
    QPushButton* start_game;
    QPushButton* reset_game;
    QPushButton* button_zero;
    QPushButton* button_one;
    QPushButton* pause_button;
    QPushButton* help_button;

    // Button to track the users last clicked button used in add number and
    // when choosing way to start the game.
    QPushButton* last_clicked_button = nullptr;

    // Game board from "gameboard.hh"
    GameBoard* gameBoard;

    // Info label tells user instructions or if user does something you can
    // see what hapend here. TimerLabel has the time of the game timer.
    QLabel* info_label;
    QLabel *timerLabel;

    // Line_edit is where user puts their random seed or their own input board.
    QLineEdit* line_edit;

    // Timer that starts when game starts
    QTimer* game_timer;

    // Lines to separate the gameboard from other buttons.
    QFrame* bottom_line;
    QFrame* top_line;

    // gLayout is the main layout and boardLayout is all the board buttons and
    // pause and reset buttons
    QGridLayout* gLayout;
    QGridLayout* boardLayout;

    // Timers seconds
    int seconds = 0;

    // Index of the last clicked button needed when trying to add new 0 or 1
    // on the board.
    int last_clicked_board_button_index;

    // Point calculator needs these
    int characters_added = 0;
    int wrong_characters_added = 0;

    // Standard button size for board buttons and button zero and button one.
    const int BUTTON_WIDTH = 50;
    const int BUTTON_HEIGTH = 50;

    // Standard font size for timer
    const int TIMER_FONT = 40;

    // Vector of all the board buttons that represents the board for user
    std::vector<QPushButton*> board_buttons_;

    // Initialize the buttons and add them to the vector.
    void init_board_buttons();

    // When user has chosen the way of starting the game correctly we fill all
    // the known 0s and 1s to the board for user to see.
    // param: board_character, users input or randomly created board that will
    // fill all the known squares of the board.
    void fill_initialized_board(QString board_characters);
};
#endif // MAINWINDOW_HH
