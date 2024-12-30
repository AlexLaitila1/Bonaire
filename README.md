My second major project, a binairo game with a GUI using C++.

Rules:
 - Every row and colum must contain equal amount of 0s and 1s
 - No more than two similar numbers can be adjacent horizontally or vertically.

How to play:
When program is running choose between "random board seed" and "own board input.
If you chose random board input integer between 1-50 and play start game, else
nothing happens. If you chose "own board input" if the board is 6x6 input 36
characters starting and ending with quote ("). Then program checks if the input
is valid and fills the board leaving all the empty squares available. When you
want to input 0 or 1 in a square click the square and then click 0 or 1 below
the board, then program checks if the add is valid and then adds the 0 or 1 to
the square. If needed you can pause the timer or reset the game at anytime you
want. If needed press "help" to get the rules of the game or how to play. If
you can solve the binairo new winning screen opens that shows you how long it
took you to solve and how many points you got. After winning you can press
"Reset" to play again.

About the program:
There is class GameBoard that contains all the data about the board and the
MainWindow only shows the state of the game, so GameBoard knows when adding is
possible and MainWindow only knows which buttons have been clicked. Also, all
the time only needed push buttons are available, like "Reset" is always
available, but "Start game" is only available after choosing starting method.
And there is a pushButton "last_clicked_button" that keeps track of the users
clicking and that is used when adding 0 or 1 to the board. Also, all the
buttons are coded not added using mainwindow.ui.
