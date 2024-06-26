#include "tictactoe.h"
#include <iostream>
#include <cctype>
#include <limits>

using namespace std;

void print_board(vector<vector<char>>& board) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cout << board[i][j];
            if (j < 2)
                cout << " | ";
        }
        cout << endl;
        if (i < 2)
            cout << "---------" << endl;
    }

}

void addchar(vector<vector<char>>& board, int spot, char playerSymbol) {
    int row = (spot - 1) / 3;
    int col = (spot - 1) % 3;

    if (board[row][col] == '-') {
        board[row][col] = playerSymbol;
        print_board(board);
    } else {
        cout << "Invalid spot, Try another one :";
        cin >> spot;
        addchar(board, spot, playerSymbol);
    }
}

bool checkwin(vector<vector<char>>& board) {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != '-')
            return true;
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != '-')
            return true;
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != '-')
        return true;
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != '-')
        return true;

    return false;
}

void PlayerVsPlayer() {
    int spot;
    char playAgain;
    char player1Symbol, player2Symbol;

    do {
        vector<vector<char>> board(3, vector<char>(3, '-'));
        print_board(board);
        bool check = false;

        cout << "Player 1, choose your symbol (X or O): ";
        cin >> player1Symbol;
        player1Symbol = toupper(player1Symbol);
        player2Symbol = (player1Symbol == 'X') ? 'O' : 'X';
        cout << "Player 2 uses " << player2Symbol << endl;

        char currentPlayerSymbol = player1Symbol;
        for (int round = 1; round <= 9; ++round) {
            cout << "Enter the spot you would like to add your character in: ";
            cin >> spot;
            while (spot > 9 || spot <= 0) {
                cout << "Invalid move. Try again: ";
                cin >> spot;
            }
            addchar(board, spot, currentPlayerSymbol);
            check = checkwin(board);
            if (check)
                break;
            else
                currentPlayerSymbol = (currentPlayerSymbol == player1Symbol) ? player2Symbol : player1Symbol;
        }

        if (check)
            cout << "The Winner is " << currentPlayerSymbol << endl;
        else
            cout << "The game is a tie" << endl;

        cout << "Do you want to play again (Y/N): ";
        cin >> playAgain;
    } while (playAgain == 'y' || playAgain == 'Y');
}
int evaluate(vector<vector<char>>& board, char player) {
	char opponent = (player == 'X') ? 'O' : 'X';
        // Checking rows for a win or loss
    for (int row = 0; row < 3; ++row) {
        if (board[row][0] == board[row][1] && board[row][1] == board[row][2]) {
			if (board[row][0] == player)
                return 10;
			else if (board[row][0] == opponent)
                return -10;
        }
    }
    // Checking columns for a win or loss
    for (int col = 0; col < 3; ++col) {
        if (board[0][col] == board[1][col] && board[1][col] == board[2][col]) {
			if (board[0][col] == player)
                return 10;
			else if (board[0][col] == opponent)
                return -10;
        }
    }
    // Checking main diagonal for a win or loss
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
		if (board[0][0] == player)
            return 10;
		else if (board[0][0] == opponent)
            return -10;
    }
    // Checking the other diagonal for a win or loss
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
		if (board[0][2] == player)
            return 10;
		else if (board[0][2] == opponent)
            return -10;
    }

    // If no one wins
	return 0;

}

struct Node {
    vector<vector<char>> state;
    vector<Node*> children;
    int score;
    bool isMaximizingPlayer;


};

bool game_over(vector<vector<char>>& board) {
        // Check rows for a win
    for (int row = 0; row < 3; ++row) {
        if (board[row][0] == board[row][1] && board[row][1] == board[row][2] && board[row][0] != '-')
            return true;
    }

    // Check columns for a win
    for (int col = 0; col < 3; ++col) {
        if (board[0][col] == board[1][col] && board[1][col] == board[2][col] && board[0][col] != '-')
            return true;
    }

    // Check diagonals for a win
    if ((board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != '-') ||
        (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != '-'))
        return true;

    // Check if the board is full (draw)
    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 3; ++col) {
            if (board[row][col] == '-')
                return false; // Game is not over yet
        }
    }

    // If none of the above conditions are met, the game is not over
	return true;

}

// Define a function to get all possible moves
vector<pair<int, int>> get_possible_moves(vector<vector<char>>& board) {
    vector<pair<int, int>> possible_moves;
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            if (board[i][j] == '-') {
                possible_moves.push_back({i, j});
            }
        }
    }
    return possible_moves;
}

Node* buildTree(vector<vector<char>>& board, char player, bool isMaximizingPlayer, int depth) {
   // Node* node = new Node;
   Node* node = new Node;
    node->state = board;
    node->isMaximizingPlayer = isMaximizingPlayer;

	if (depth == 0 || game_over(node->state)) {
		node->score = evaluate(node->state, player);
        return node;
    }

    vector<pair<int, int>> possible_moves = get_possible_moves(node->state);
    for (auto move : possible_moves) {
        vector<vector<char>> new_board = node->state;
        new_board[move.first][move.second] = (isMaximizingPlayer) ? player : (player == 'X' ? 'O' : 'X');
        Node* child = buildTree(new_board, player, !isMaximizingPlayer, depth - 1);
        node->children.push_back(child);
    }

    return node;
}

int alphaBeta(Node* node, int depth, int alpha, int beta, bool isMaximizingPlayer) {
	if (depth == 0 || game_over(node->state)) {
        return node->score;
    }

    if (isMaximizingPlayer) {
        int value = INT_MIN;
        for (Node* child : node->children) {
            value = max(value, alphaBeta(child, depth - 1, alpha, beta, false));
            alpha = max(alpha, value);
            if (beta <= alpha)
                break;
        }
        return value;
    } else {
        int value = INT_MAX;
        for (Node* child : node->children) {
            value = min(value, alphaBeta(child, depth - 1, alpha, beta, true));
            beta = min(beta, value);
            if (beta <= alpha)
                break;
        }
        return value;
    }
}

pair<int, int> find_best_move(vector<vector<char>>& board, char player, int max_depth) {
    Node* root = buildTree(board, player, true, max_depth);
    int bestScore = INT_MIN;
    pair<int, int> bestMove;
    for (Node* child : root->children) {
		int score = alphaBeta(child, max_depth, INT_MIN, INT_MAX, false);
        if (score > bestScore) {
            bestScore = score;
            // Assuming each child represents a move, you can get the move from the state
            // In this example, let's assume the child state has only one difference from the parent state
            for (int i = 0; i < board.size(); ++i) {
                for (int j = 0; j < board[i].size(); ++j) {
                    if (board[i][j] != child->state[i][j]) {
                        bestMove = {i, j};
                        break;
                    }
                }
            }
        }
    }
    return bestMove;
}

void PlayerVsAI() {
    char playAgain;
    char currentPlayerSymbol;
    char playerSymbol = 'O';
    char aiSymbol = 'X';
    char startingPlayer = 'P';

    do {
        vector<vector<char>> board(3, vector<char>(3, '-'));
        print_board(board);
        currentPlayerSymbol = (startingPlayer == 'P') ? playerSymbol : aiSymbol;

        int spot;
        for (int round = 1; round <= 9; ++round) {
            if (currentPlayerSymbol == playerSymbol) {
                cout << "Enter the spot you would like to add your character in: ";
                cin >> spot;
                while (spot > 9 || spot <= 0) {
                    cout << "Invalid move. Try again: ";
                    cin >> spot;
                }
                addchar(board, spot, currentPlayerSymbol);

                if (evaluate(board,currentPlayerSymbol) == -10) {
                    cout << "You win!" << endl;
                    break;
                }
            } else {
                auto ai_move = find_best_move(board,aiSymbol,1000);
                board[ai_move.first][ai_move.second] = aiSymbol;
                cout << "AI's move:" << endl;
                print_board(board);

                if (evaluate(board,aiSymbol) == 10) {
                    cout << "AI wins!" << endl;
                    break;
                }
            }
            if (game_over(board)) {
                cout << "It's a draw!" << endl;
                break;
            }
            currentPlayerSymbol = (currentPlayerSymbol == playerSymbol) ? aiSymbol : playerSymbol;
        }

        startingPlayer = (startingPlayer == 'P') ? 'A' : 'P';

        cout << "Do you want to play again (Y/N): ";
        cin >> playAgain;
    } while (playAgain == 'y' || playAgain == 'Y');
}
int get_next_player_move(const std::vector<int>& moves, int& current_move_index) {
    if (current_move_index < moves.size()) {
        return moves[current_move_index++];
    }
    return -1; // No more moves
}
