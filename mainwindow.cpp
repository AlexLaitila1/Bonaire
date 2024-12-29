#include "mainwindow.hh"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    game_timer(new QTimer(this))
{
    ui->setupUi(this);
    resize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Initialize all the needed widgets to start the game.
    info_label = new QLabel("", this);
    line_edit = new QLineEdit(this);

    // Max length is the size of the board and quote marks (") both side.
    line_edit->setMaxLength(SIZE * SIZE + 2);

    // Line edit becomes enable when user have chose between random and own
    // input board.
    line_edit->setEnabled(false);

    random_board = new QPushButton("Random board seed", this);
    connect(random_board, &QPushButton::clicked,
            this, &MainWindow::initialize_random_board_click);

    own_board = new QPushButton("Own board input", this);
    connect(own_board, &QPushButton::clicked,
            this, &MainWindow::initialize_own_board_click);

    start_game = new QPushButton("Start game", this);
    connect(start_game, &QPushButton::clicked,
            this, &MainWindow::start_game_click);

    // Start game becomes enable when user have chose between random and own
    // input board.
    start_game->setEnabled(false);

    // Help window can be opened at any time.
    help_button = new QPushButton("Help", this);
    connect(help_button, &QPushButton::clicked,
            this, &MainWindow::handle_help_button_click);

    // Initialize all the board buttons
    init_board_buttons();

    // create new timer and initialize the timer to 00:00 and timer labels font
    // so it is easier to user see.
    game_timer = new QTimer(this);
    connect(game_timer, &QTimer::timeout, this, &MainWindow::onTimerTimeout);
    timerLabel = new QLabel(this);
    timerLabel->setText("00:00");

    QFont font = timerLabel->font();
    font.setPointSize(TIMER_FONT);
    timerLabel->setFont(font);

    // Set the central widget add a layout to the central widget and add all
    // the widgets to the layout.
    QWidget* central = new QWidget(this);
    gLayout = new QGridLayout(central);
    gLayout->addWidget(timerLabel,0,0);
    gLayout->addWidget(info_label, 1,0);
    gLayout->addWidget(line_edit, 2, 0);
    gLayout->addWidget(random_board, 0, 2);
    gLayout->addWidget(own_board, 1, 2);
    gLayout->addWidget(start_game, 2, 2);
    gLayout->addWidget(help_button, 3, 0);
    setCentralWidget(central);

    // Board layout is layout where the gameboard, pause, reset and separating
    // lines are.
    gLayout->addLayout(boardLayout, 4, 0, SIZE, SIZE + 2);
    setCentralWidget(central);

}

MainWindow::~MainWindow()
{
    // If game ongoing
    if(seconds != 0){
        delete gameBoard;
    }

    // If button is nullptr it has to be deleted separetly.
    delete last_clicked_button;

    delete ui;
}

void MainWindow::init_board_buttons(){

    // New layout where the gameboard, pause, reset and the lines hat separetas
    // the gameboard from other widgets.
    boardLayout = new QGridLayout();

    // Loop that creates the right size gameboard full of empty push buttons.
    for(unsigned int i = 0; i < SIZE; ++i){
        for(unsigned int j = 0; j < SIZE; ++j)
        {
            QString letter = "";
            QPushButton* pushButton = new QPushButton(letter, this);

            // Disabled till the game starts.
            pushButton->setEnabled(false);
            pushButton->setFixedWidth(BUTTON_WIDTH);
            pushButton->setFixedHeight(BUTTON_HEIGTH);

            // Add to the button to the vector, so user can add 0 or 1 later on
            // the game.
            board_buttons_.push_back(pushButton);

            // Add the button to the layout using coordinates i and j. We must
            // add one to the i so upper separating line gets it's own place.
            boardLayout->addWidget(pushButton, i + 1, j);

            connect(pushButton, &QPushButton::clicked,
                    this, &MainWindow::handle_button_clicks);
        }
    }

    // Initialize the lines to separate the game board from other widgets.
    bottom_line = new QFrame(this);
    bottom_line->setFrameShape(QFrame::HLine);
    bottom_line->setFrameShadow(QFrame::Sunken);
    boardLayout->addWidget(bottom_line, SIZE, 0, 2, SIZE);

    top_line = new QFrame(this);
    top_line->setFrameShape(QFrame::HLine);
    top_line->setFrameShadow(QFrame::Sunken);
    boardLayout->addWidget(top_line, 0, 0, 1, SIZE);

    // Using this we add 0 to the game board.
    button_zero = new QPushButton("0", this);

    boardLayout->addWidget(button_zero, SIZE + 1, NUMBER_OF_SYMBOLS - 1);
    button_zero->setFixedWidth(BUTTON_WIDTH);
    button_zero->setFixedHeight(BUTTON_HEIGTH);

    // Becomes available when game starts.
    button_zero->setEnabled(false);
    connect(button_zero, &QPushButton::clicked,
            this, &MainWindow::handle_zero_button_click);

    // Using this we add 1 to the game board.
    button_one = new QPushButton("1", this);

    boardLayout->addWidget(button_one, SIZE + 1, NUMBER_OF_SYMBOLS);
    button_one->setFixedWidth(BUTTON_WIDTH);
    button_one->setFixedHeight(BUTTON_HEIGTH);

    // Becomes available when game starts.
    button_one->setEnabled(false);
    connect(button_one, &QPushButton::clicked,
            this, &MainWindow::handle_one_button_click);

    // Reset button clears the game board and asks for new way to start the
    // game.
    reset_game = new QPushButton("Reset", this);

    boardLayout->addWidget(reset_game, SIZE + 1, SIZE - 1);
    reset_game->setFixedWidth(BUTTON_WIDTH);
    reset_game->setFixedHeight(BUTTON_HEIGTH);

    // New background so the button stands out and when game is over it doesn't
    // get coloured.
    reset_game->setStyleSheet("background-color: white; color: black;");

    // Becomes available when game starts.
    reset_game->setEnabled(false);
    connect(reset_game,&QPushButton::clicked,
            this, &MainWindow::handle_reset_button_click);

    // Pause so timer can be stopped when game is ongoing and all the board
    // buttons becomes disabled.
    pause_button = new QPushButton("Pause", this);

    boardLayout->addWidget(pause_button, SIZE + 1, 0);
    pause_button->setFixedWidth(BUTTON_WIDTH);
    pause_button->setFixedHeight(BUTTON_HEIGTH);

    // Becomes available when game starts.
    pause_button->setEnabled(false);
    connect(pause_button, &QPushButton::clicked,
            this, &MainWindow::handle_pause_button_click);
}

// Method will be called when gameboard class has approved the users input to
// start the game and program is ready to start the game. Method gets string of
// characters which need to be added to the game board to user to see.
void MainWindow::fill_initialized_board(QString board_characters)
{
    // Go thure all the board_buttons and add the characters from
    // board_characters.
    for(unsigned long int i = 0; i < board_buttons_.size(); ++i) {

        QString character = board_characters[i+1];
        board_buttons_[i]->setText(character);

        // If the character is blank user have to add 0 or 1 in it, so the
        // button has to be available.
        if(character == " "){
            board_buttons_[i]->setEnabled(true);
        }
    }

    // starts the timer.
    game_timer->start(1000);
}

void MainWindow::initialize_random_board_click() {
    // Open the line_edit and star_game button so user can add the integer
    // and start the game.
    line_edit->setEnabled(true);
    start_game->setEnabled(true);

    // Instruction text.
    info_label->setText(RANDOM_INT);


    // Also we need this so the start_game knows how to handle users input in
    // the line_edit.
    last_clicked_button = random_board;
}

void MainWindow::initialize_own_board_click() {
    // Open the line_edit and star_game button so user can add the integer
    // and start the game.
    line_edit->setEnabled(true);
    start_game->setEnabled(true);

    // Instruction text.
    const QString needed_chars = QString::number(SIZE * SIZE + 2);
    info_label->setText(OWN_INPUT + needed_chars);

    // Also we need this so the start_game knows how to handle users input in
    // the line_edit.
    last_clicked_button = own_board;
}

void MainWindow::start_game_click() {

    // Get the users input in line_edit and convert it into std::string.
    QString qstring_input = line_edit->text();
    std::string string_input = qstring_input.toStdString();

    // So we know in the end which buttons becomes enabled and disabled.
    bool start_successful = false;

    if( last_clicked_button == own_board){

        if( string_input.size() != SIZE*SIZE + 2){
            info_label->setText(WRONG_SIZE);
            return;
        }

        // Input size was ok we can create a gameBoard and initialize it.
        else {
            gameBoard = new GameBoard;
            if(gameBoard->fill_from_input(string_input)){
                start_successful = true;

                // Fill the user interface game board.
                fill_initialized_board(qstring_input);
                info_label->setText(GAME_STARTED);
            }

            // If input contained other than spaces, 0s and 1s.
            else{
                info_label->setText(BAD_INPUT);
                delete gameBoard;
            }
        }
    }

    else if(last_clicked_button == random_board){
        gameBoard = new GameBoard;
        std::string generated_board = gameBoard->fill_randomly(string_input);

        // fill_randomly returns empty string if the input was bad seed.
        if(!generated_board.empty()){

            if(gameBoard->fill_from_input(generated_board)){

                start_successful = true;
                QString board_chars = QString::fromStdString(generated_board);

                // Fill the user interface game board.
                fill_initialized_board(board_chars);
                info_label->setText(GAME_STARTED);
            }
        }
        else{
            info_label->setText(BAD_SEED);
            delete gameBoard;
        }

    }

    // When game starts we have to enable and disable bunch of buttons, so for
    // example user can not try to start a new game middle of old game.
    if(start_successful){
        line_edit->clear();
        line_edit->setEnabled(false);
        start_game->setEnabled(false);
        own_board->setEnabled(false);
        random_board->setEnabled(false);
        pause_button->setEnabled(true);
        reset_game->setEnabled(true);
        last_clicked_button = nullptr;

        // User can input already completed game, so we have to check this.
        if(gameBoard->is_game_over()){
            game_over();
        }
    }

}

void MainWindow::handle_button_clicks(){

    // Go thrue all the game board buttons and find the which one user clicked.
    for (unsigned long int i = 0; i < board_buttons_.size(); ++i)
    {
        if (board_buttons_[i] == sender())
        {
            // For adding new 0 or 1 we have store the button and it's index.
            last_clicked_button = board_buttons_[i];
            last_clicked_board_button_index = i;

            // User can choose what he want's to put inside the square.
            button_one->setEnabled(true);
            button_zero->setEnabled(true);
        }
    }

}

void MainWindow::handle_zero_button_click(){

    // Calculate the coordinates that correlates to the game board.
    int x = last_clicked_board_button_index % SIZE;
    int y = last_clicked_board_button_index / SIZE;

    // Try to add it on the gameBoard.
    if( gameBoard->add_symbol(x,y,'0') ) {
        last_clicked_button->setText("0");
        info_label->setText(ADD_OK);

        // Disable the button user added character
        last_clicked_button->setEnabled(false);

        // For point system keep track of the added symbols
        ++characters_added;
    }
    else {
        info_label->setText(ADD_NOT_OK);

        // For point system keep track of the added symbols
        ++wrong_characters_added;
    }

    // Check if that was the last missing element
    if(gameBoard->is_game_over()){
        game_over();
    }

    button_zero->setEnabled(false);
    button_one->setEnabled(false);
    last_clicked_button = nullptr;
}

void MainWindow::handle_one_button_click(){

    // Calculate the coordinates that correlates to the game board.
    int x = last_clicked_board_button_index % SIZE;
    int y = last_clicked_board_button_index / SIZE;

    // Try to add it on the gameBoard.
    if( gameBoard->add_symbol(x,y,'1') ) {
        last_clicked_button->setText("1");
        info_label->setText(ADD_OK);

        // Disable the button user added character
        last_clicked_button->setEnabled(false);

        // For point system keep track of the added symbols
        ++characters_added;
    }
    else {
        info_label->setText(ADD_NOT_OK);

        // For point system keep track of the added symbols
        ++wrong_characters_added;
    }

    if(gameBoard->is_game_over()){

        // Check if that was the last missing element
        game_over();
    }

    button_zero->setEnabled(false);
    button_one->setEnabled(false);
    last_clicked_button = nullptr;
}

void MainWindow::handle_pause_button_click()
{
    // Only pause and reset button can be clicked when pause is ongoing so we
    // know if the pause is ongoing.
    if(last_clicked_button != pause_button){

        // Store the fact that pause is ongoing and stop the timer
        last_clicked_button = pause_button;
        game_timer->stop();

        info_label->setText
            ("Game is paused till you press pause button again.\n"
                       "                       (reset possible)");

        // All the enabled buttons need to be disabled.
        for (auto button : board_buttons_) {
            if(button->isEnabled()){
                button->setEnabled(false);
            }
        }
    }

    // If pause was ongoing we need to stop the pause.
    else {

        info_label->setText("Game contines and timer started running again!");

        game_timer->start();
        last_clicked_button = nullptr;

        // Enable all the empty squares.
        for (auto button : board_buttons_) {
            if(button->text() == " "){
                button->setEnabled(true);
            }
        }
    }
}

void MainWindow::handle_help_button_click()
{
    // Open a new window that contains help text.
    QDialog *help_dialog = new QDialog(this);

    // Layout where the text will be placed.
    QVBoxLayout* help_layout = new QVBoxLayout(help_dialog);
    QString qhelp = QString::fromStdString(HELP_TEXT);
    QLabel* help_text = new QLabel(qhelp, help_dialog);

    help_layout->addWidget(help_text);
    help_dialog->show();
}

void MainWindow::game_over() {

    game_timer->stop();

    // If game over we need to disable all the buttons except "Reset"
    pause_button->setEnabled(false);
    help_button->setEnabled(false);
    info_label->setText("You won!");

    this->setStyleSheet("background-color: rgba(120, 252, 120, 100);");

    // Calculate the earned points
    characters_added *= 100;
    wrong_characters_added *= 50;
    int time_points = 0;

    int max_time_points = 10 * pow(10, NUMBER_OF_SYMBOLS);
    if(seconds < max_time_points){
        time_points = max_time_points - seconds;
    }

    // User will see the total_points in the opening window.
    int total_points = characters_added + time_points - wrong_characters_added;
    QString string_points = QString::number(total_points);

    // User will also see the time spent in the binairo.
    QString string_seconds = QString::number(seconds);

    // Create a new dialog with title "Game Over"
    QDialog* win_dialog = new QDialog(this);
    win_dialog->setWindowTitle("Game Over");

    // Create a layout and a label for the dialog
    QVBoxLayout* layout = new QVBoxLayout(win_dialog);
    QLabel* label = new QLabel("Congratulations, you won!", win_dialog);
    QLabel* timeLabel = new QLabel("Time spent: " + string_seconds +
                                       " seconds", win_dialog);
    QLabel* points = new QLabel("Based on the point system you got: "
                                + string_points + " points");
    layout->addWidget(label);
    layout->addWidget(timeLabel);
    layout->addWidget(points);

    label->setStyleSheet("background-color: white; color: black;");
    timeLabel->setStyleSheet("background-color: white; color: black;");
    points->setStyleSheet("background-color: white; color: black;");


    // Show the dialog
    win_dialog->show();

}
void MainWindow::handle_reset_button_click(){
    // When user deletes the game we need to delete the ongoing gameBoard
    delete gameBoard;

    // If the game was over we need to change the background colour to default.
    this->setStyleSheet("nullptr");

    // Also enable and disable some buttons so user can start a new game
    help_button->setEnabled(true);
    own_board->setEnabled(true);
    random_board->setEnabled(true);
    reset_game->setEnabled(false);
    pause_button->setEnabled(false);

    // Clear all the buttons.
    for (auto button : board_buttons_) {
        button->setText("");
        button->setEnabled(false);
    }

    // And reset the timer and collected data.
    wrong_characters_added = 0;
    characters_added = 0;
    seconds = 0;
    timerLabel->setText("00:00");
    game_timer->stop();
    info_label->setText("Game reset, select a new start for a new game");
}

void MainWindow::onTimerTimeout() {

    // Timer that increases one every second when game is ongoing.
    ++seconds;

    int minutes = seconds / 60;
    int remaining_seconds = seconds % 60;

    // Import the calculated minutes and seconds to the clock label.
    QString timeText = QString("%1:%2")
                           .arg(minutes, 2, 10, QChar('0'))
                           .arg(remaining_seconds, 2, 10, QChar('0'));

    timerLabel->setText(timeText);
}
