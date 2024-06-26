#include <vcl.h>
#pragma hdrstop

#include "aivsplayer.h"
#include "HomePage.h"
#include "Register.h"
#include "Login.h"
#include "gamehistory.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm5 *Form5;
//---------------------------------------------------------------------------

AnsiString positions = "";  // This will store the click sequence
std::vector<int> clickOrder; // Vector to keep track of click order

char current ;
char playertype;
AnsiString opponent= "AI";
char AImove  ;
char Playermove ;
int buttonNumber;
static int game=0;
//----------------------------------------------------------------------------
void insertGameResult(int userID, const AnsiString& winOrLose, const AnsiString& history, const AnsiString& positions ,const AnsiString& playertype,const AnsiString& opponent ) {
	try {
		int nextGameID = 1; // Default to 1 if there are no existing records
		int nextGameNumber = 1; // Default to 1 if there are no existing records

		// Check if the user ID exists in the GameResults table, if not, insert it
		Form5->GameresultsTable->SQL->Text = "SELECT ID FROM GameResults WHERE ID = :userID";
		Form5->GameresultsTable->Params->ParamByName("userID")->AsInteger = userID;
		Form5->GameresultsTable->Open();
		if (Form5->GameresultsTable->Eof) {
			// Insert the user ID into the GameResults table
			Form5->GameresultsTable->SQL->Text = "SELECT MAX(GameID) AS MaxGameID FROM GameResults";
			Form5->GameresultsTable->Open();
			if (!Form5->GameresultsTable->Eof) {
				nextGameID = Form5->GameresultsTable->FieldByName("MaxGameID")->AsInteger + 1;
			}

			Form5->GameresultsTable->Close();
			Form5->GameresultsTable->SQL->Text = "INSERT INTO GameResults (ID,GameID) VALUES (:userID,:gameID)";
			Form5->GameresultsTable->Params->ParamByName("userID")->AsInteger = userID;
			Form5->GameresultsTable->Params->ParamByName("gameID")->AsInteger = nextGameID;
			Form5->GameresultsTable->ExecSQL();
		}
		Form5->GameresultsTable->Close();

		// Get the current maximum GameID and gamenumber for the user
		Form5->GameresultsTable->SQL->Text = "SELECT MAX(GameID) AS MaxGameID FROM GameResults ";
		Form5->GameresultsTable->Open();
		if (!Form5->GameresultsTable->Eof) {
			nextGameID = Form5->GameresultsTable->FieldByName("MaxGameID")->AsInteger + 1;
		}
		Form5->GameresultsTable->Close();
		Form5->GameresultsTable->SQL->Text = "SELECT MAX(gamenumber) AS MaxGameNumber FROM GameResults WHERE ID = :userID";
		Form5->GameresultsTable->Params->ParamByName("userID")->AsInteger = userID;
		Form5->GameresultsTable->Open();
		if (!Form5->GameresultsTable->Eof) {
			//nextGameID = Form4->GameresultsTable->FieldByName("MaxGameID")->AsInteger + 1;
			nextGameNumber = Form5->GameresultsTable->FieldByName("MaxGameNumber")->AsInteger + 1;
		}

		Form5->GameresultsTable->Close();

		// Insert the new game result
		Form5->GameresultsTable->SQL->Text = "INSERT INTO GameResults (ID, gamenumber, GameID, WinOrLose, history, positions ,playertype ,opponent) VALUES (:userID, :gamenumber, :gameID, :winOrLose, :history, :positions ,:playertype,:opponent)";
		Form5->GameresultsTable->Params->ParamByName("userID")->AsInteger = userID;
		Form5->GameresultsTable->Params->ParamByName("gamenumber")->AsInteger = nextGameNumber;
		Form5->GameresultsTable->Params->ParamByName("gameID")->AsInteger = nextGameID;
		Form5->GameresultsTable->Params->ParamByName("winOrLose")->AsString = winOrLose;
		Form5->GameresultsTable->Params->ParamByName("history")->AsString = history;
		Form5->GameresultsTable->Params->ParamByName("positions")->AsString = positions;
		Form5->GameresultsTable->Params->ParamByName("playertype")->AsString = playertype;
		Form5->GameresultsTable->Params->ParamByName("opponent")->AsString = opponent;
		Form5->GameresultsTable->ExecSQL();

		ShowMessage("Game result inserted successfully");
	} catch (const Exception& e) {
		ShowMessage("Error inserting game result: " + e.Message);
	}
}


AnsiString serializeChars(AnsiString c1, AnsiString c2, AnsiString c3, AnsiString c4, AnsiString c5, AnsiString c6, AnsiString c7, AnsiString c8, AnsiString c9) {
	AnsiString result;
	result = c1; result += ','; result += c2; result += ','; result += c3;
	result += ','; result += c4; result += ','; result += c5; result += ',';
	result += c6; result += ','; result += c7; result += ','; result += c8;
	result += ','; result += c9;
	return result;
}

//---------------------------------------------------------------------------

bool TForm5::checkisWinner() {
	// Check rows
	if (Button1->Caption == Button2->Caption && Button1->Caption == Button3->Caption && !Button1->Caption.IsEmpty()) {

		Button1->Font->Style = TFontStyles() << fsBold;
		Button2->Font->Style = TFontStyles() << fsBold;
		Button3->Font->Style = TFontStyles() << fsBold;

		return true;
	}
	if (Button4->Caption == Button5->Caption && Button4->Caption == Button6->Caption && !Button4->Caption.IsEmpty()) {
		Button4->Font->Style = TFontStyles() << fsBold;
		Button5->Font->Style = TFontStyles() << fsBold;
		Button6->Font->Style = TFontStyles() << fsBold;
		return true;
	}
	if (Button7->Caption == Button8->Caption && Button7->Caption == Button9->Caption && !Button7->Caption.IsEmpty()) {

		Button7->Font->Style = TFontStyles() << fsBold;
		Button8->Font->Style = TFontStyles() << fsBold;
		Button9->Font->Style = TFontStyles() << fsBold;
		return true;
	}

	// Check columns
	if (Button1->Caption == Button4->Caption && Button1->Caption == Button7->Caption && !Button1->Caption.IsEmpty()) {

		Button1->Font->Style = TFontStyles() << fsBold;
        Button4->Font->Style = TFontStyles() << fsBold;
		Button7->Font->Style = TFontStyles() << fsBold;
		return true;
	}
	if (Button2->Caption == Button5->Caption && Button2->Caption == Button8->Caption && !Button2->Caption.IsEmpty()) {

        Button2->Font->Style = TFontStyles() << fsBold;
		Button5->Font->Style = TFontStyles() << fsBold;
		Button8->Font->Style = TFontStyles() << fsBold;
		return true;
	}
	if (Button3->Caption == Button6->Caption && Button3->Caption == Button9->Caption && !Button3->Caption.IsEmpty()) {

		Button3->Font->Style = TFontStyles() << fsBold;
		Button6->Font->Style = TFontStyles() << fsBold;
		Button9->Font->Style = TFontStyles() << fsBold;
		return true;
	}

	// Check diagonals
	if (Button1->Caption == Button5->Caption && Button1->Caption == Button9->Caption && !Button1->Caption.IsEmpty()) {

		Button1->Font->Style = TFontStyles() << fsBold;
		Button5->Font->Style = TFontStyles() << fsBold;
		Button9->Font->Style = TFontStyles() << fsBold;
		return true;
	}
	if (Button3->Caption == Button5->Caption && Button3->Caption == Button7->Caption && !Button3->Caption.IsEmpty()) {

		Button3->Font->Style = TFontStyles() << fsBold;
		Button5->Font->Style = TFontStyles() << fsBold;
		Button7->Font->Style = TFontStyles() << fsBold;
		return true;
	}

	return false;
}

vector<vector<char>> getBoardState() {
	vector<vector<char>> board(3, vector<char>(3, '-'));
	TButton* buttons[] = { Form5->Button1, Form5->Button2, Form5->Button3, Form5->Button4, Form5->Button5, Form5->Button6, Form5->Button7, Form5->Button8, Form5->Button9 };
	for (int i = 0; i < 9; ++i) {
		if (buttons[i]->Caption == 'X') {
			board[i / 3][i % 3] = 'X';
		} else if (buttons[i]->Caption == 'O') {
			board[i / 3][i % 3] = 'O';
		}
	}
	return board;
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



// Helper function to get the button number
int getButtonNumber(TButton* button) {
	if (button == Form5->Button1) return 1;
	if (button == Form5->Button2) return 2;
	if (button == Form5->Button3) return 3;
	if (button == Form5->Button4) return 4;
	if (button == Form5->Button5) return 5;
	if (button == Form5->Button6) return 6;
	if (button == Form5->Button7) return 7;
	if (button == Form5->Button8) return 8;
	if (button == Form5->Button9) return 9;
	return 0; // Should not happen
}

void applyAIMove(pair<int, int> move, char aiSymbol) {
	TButton* buttons[] = { Form5->Button1, Form5->Button2, Form5->Button3, Form5->Button4, Form5->Button5, Form5->Button6, Form5->Button7, Form5->Button8, Form5->Button9 };
	int index = move.first * 3 + move.second;
	buttons[index]->Caption = aiSymbol;
	// Record the button click
	buttonNumber = getButtonNumber(buttons[index]);
	clickOrder.push_back(buttonNumber);
}

__fastcall TForm5::TForm5(TComponent* Owner)
	: TForm(Owner)
{
	Label1->Caption = "Choose X or O";
	WinnerLabel->Caption = "";
	Button1->Hide();
	Button2->Hide();
	Button3->Hide();
	Button4->Hide();
	Button5->Hide();
	Button6->Hide();
	Button7->Hide();
	Button8->Hide();
	Button9->Hide();
	Button10->Hide();

	int idFromForm1 = ((TForm1*)Owner)->ID; // Access integer ID property from Form1
	int idFromForm6 = ((TForm6*)Owner)->gameID;

 }

void __fastcall TForm5::ButtonClick(TObject *Sender) {

	TButton* clickedButton = dynamic_cast<TButton*>(Sender);
	if (clickedButton->Caption.IsEmpty()) { // Ensure the button is not already clicked
		clickedButton->Caption = Playermove;

		// Record the button click
		buttonNumber = getButtonNumber(clickedButton);
		clickOrder.push_back(buttonNumber);
		// Check if current player wins
		if (checkisWinner()) {
			if (current == playertype) {
				WinnerLabel->Caption = "won";
				ShowMessage("You won!");
			} else {
				WinnerLabel->Caption = "lose";
				ShowMessage("AI won, You lost!");
			}
	Button1->Enabled = false;
	Button2->Enabled = false;
	Button3->Enabled = false;
	Button4->Enabled = false;
	Button5->Enabled = false;
	Button6->Enabled = false;
	Button7->Enabled = false;
	Button8->Enabled = false;
	Button9->Enabled = false;
	Form5->Button10->Show();
			return;
		}


	if (!checkisWinner() && !Button1->Caption.IsEmpty() && !Button2->Caption.IsEmpty() &&
		!Button3->Caption.IsEmpty() && !Button4->Caption.IsEmpty() && !Button5->Caption.IsEmpty() &&
		!Button6->Caption.IsEmpty() && !Button7->Caption.IsEmpty() && !Button8->Caption.IsEmpty() &&
		!Button9->Caption.IsEmpty()) {
		ShowMessage("The game is Draw");
		WinnerLabel->Caption = "draw";
	Button1->Enabled = false;
	Button2->Enabled = false;
	Button3->Enabled = false;
	Button4->Enabled = false;
	Button5->Enabled = false;
	Button6->Enabled = false;
	Button7->Enabled = false;
	Button8->Enabled = false;
	Button9->Enabled = false;
	Form5->Button10->Show();
		return;
	}

			// Switch back to AI
		if (current == 'X') {
			current = 'O';
		} else {
			current = 'X';
		}

	vector<vector<char>> board = getBoardState();
	pair<int, int> aiMove = find_best_move(board, AImove, 9); // Use appropriate depth
	applyAIMove(aiMove, AImove);

				// Check if AI wins
	if (checkisWinner()) {
			if (current != playertype) {
				WinnerLabel->Caption = "lose";
				ShowMessage("AI won, You lost!");
			}else {
				WinnerLabel->Caption = "won";
				ShowMessage("You won!");
			}
	Button1->Enabled = false;
	Button2->Enabled = false;
	Button3->Enabled = false;
	Button4->Enabled = false;
	Button5->Enabled = false;
	Button6->Enabled = false;
	Button7->Enabled = false;
	Button8->Enabled = false;
	Button9->Enabled = false;
	Form5->Button10->Show();
			return;
		}

		if (!checkisWinner() && !Button1->Caption.IsEmpty() && !Button2->Caption.IsEmpty() &&
		!Button3->Caption.IsEmpty() && !Button4->Caption.IsEmpty() && !Button5->Caption.IsEmpty() &&
		!Button6->Caption.IsEmpty() && !Button7->Caption.IsEmpty() && !Button8->Caption.IsEmpty() &&
		!Button9->Caption.IsEmpty()) {
		ShowMessage("The game is Draw");
		WinnerLabel->Caption = "draw";
	Button1->Enabled = false;
	Button2->Enabled = false;
	Button3->Enabled = false;
	Button4->Enabled = false;
	Button5->Enabled = false;
	Button6->Enabled = false;
	Button7->Enabled = false;
	Button8->Enabled = false;
	Button9->Enabled = false;
	Form5->Button10->Show();
		return;
	}
				// Switch back to Human
		if (current == 'X') {
			current = 'O';
		} else {
			current = 'X';
		}


} }


// Helper function to convert vector to AnsiString
AnsiString vectorToAnsiString(const std::vector<int>& vec) {
	AnsiString result = "";
	for (size_t i = 0; i < vec.size(); ++i) {
		if (i != 0) {
			result += ",";
		}
		result += AnsiString(vec[i]);
	}
	return result;
}

void __fastcall TForm5::Button10Click(TObject *Sender) {
	AnsiString chars = serializeChars(Button1->Caption, Button2->Caption, Button3->Caption, Button4->Caption, Button5->Caption, Button6->Caption, Button7->Caption, Button8->Caption, Button9->Caption);

	// Convert click order to AnsiString
	positions = vectorToAnsiString(clickOrder);

	insertGameResult(Form1->ID, WinnerLabel->Caption , chars, positions,playertype,opponent);

	// Clear click order for the next game
	clickOrder.clear();
	positions = "";

	Button10->Enabled = false;

}
//---------------------------------------------------------------------------


void __fastcall TForm5::Button13Click(TObject *Sender)
{
	this->Close();
	Form3->Show();
}
//---------------------------------------------------------------------------

void __fastcall TForm5::Button14Click(TObject *Sender)
{
	this->Close();
	Form6->Show();
}
//---------------------------------------------------------------------------

void __fastcall TForm5::Button15Click(TObject *Sender) {
	Button10->Hide();
	Form5->Button11->Show();
	Form5->Button12->Show();
		// Reset all buttons
	Button1->Caption = "";
	Button2->Caption = "";
	Button3->Caption = "";
	Button4->Caption = "";
	Button5->Caption = "";
	Button6->Caption = "";
	Button7->Caption = "";
	Button8->Caption = "";
	Button9->Caption = "";

	// Enable all buttons
	Button1->Enabled = true;
	Button2->Enabled = true;
	Button3->Enabled = true;
	Button4->Enabled = true;
	Button5->Enabled = true;
	Button6->Enabled = true;
	Button7->Enabled = true;
	Button8->Enabled = true;
	Button9->Enabled = true;
	Button10->Enabled = true;
	Button1->Hide();
	Button2->Hide();
	Button3->Hide();
	Button4->Hide();
	Button5->Hide();
	Button6->Hide();
	Button7->Hide();
	Button8->Hide();
	Button9->Hide();

	// Reset labels and other game state variables
	WinnerLabel->Caption = "";
	clickOrder.clear();
	positions = "";

	// Reset the current player to 'X'
	//current = 'X';
	//playertype = 'X';
	Label1->Caption = "Choose X or O";

	// Reset font properties for all buttons
	ResetButtonFonts();
}

void __fastcall TForm5::ResetButtonFonts() {
    // Reset font size and style for all buttons
    TFont *defaultFont = new TFont;
	defaultFont->Name = "Segoe UI";
	defaultFont->Size = -18;
	defaultFont->Style = TFontStyles();
	// Apply default font and width to all buttons
	TButton *buttons[] = { Button1, Button2, Button3, Button4, Button5, Button6, Button7, Button8, Button9 };
	for (TButton *button : buttons) {
		button->Font = defaultFont;
	}
	// Free the allocated font object
	delete defaultFont;
}
//------------------------------------------------------------------------------

 void __fastcall TForm5::Button11Click(TObject *Sender)
{
	// Initialize random seed
	//std::srand(std::time(0));
	current = 'X';
	playertype='X';
	AImove = 'O';
	Playermove = 'X';
	Form5->Button11->Hide();
	Form5->Button12->Hide();
	Form5->Button1->Show();
	Form5->Button2->Show();
	Form5->Button3->Show();
	Form5->Button4->Show();
	Form5->Button5->Show();
	Form5->Button6->Show();
	Form5->Button7->Show();
	Form5->Button8->Show();
	Form5->Button9->Show();
	Label1->Caption = "Human VS AI";

	if (game % 2 == 0) { // 50% chance for AI to make the first move
		vector<vector<char>> board = getBoardState();
		pair<int, int> aiMove = find_best_move(board, AImove, 9); // Use appropriate depth
		applyAIMove(aiMove, AImove);
	}
	game++;
	// Connect ButtonClick function to each button's OnClick event
	Button1->OnClick = ButtonClick;
	Button2->OnClick = ButtonClick;
	Button3->OnClick = ButtonClick;
	Button4->OnClick = ButtonClick;
	Button5->OnClick = ButtonClick;
	Button6->OnClick = ButtonClick;
	Button7->OnClick = ButtonClick;
	Button8->OnClick = ButtonClick;
	Button9->OnClick = ButtonClick;
}
//---------------------------------------------------------------------------

void __fastcall TForm5::Button12Click(TObject *Sender)
{
    // Initialize random seed
	//std::srand(std::time(0));

	current = 'O';
	playertype= 'O';
	AImove = 'X';
	Playermove = 'O';
	Form5->Button11->Hide();
	Form5->Button12->Hide();
	Form5->Button1->Show();
	Form5->Button2->Show();
	Form5->Button3->Show();
	Form5->Button4->Show();
	Form5->Button5->Show();
	Form5->Button6->Show();
	Form5->Button7->Show();
	Form5->Button8->Show();
	Form5->Button9->Show();
	Label1->Caption = "Human VS AI";

	if (game % 2 == 0) { // 50% chance for AI to make the first move
		vector<vector<char>> board = getBoardState();
		pair<int, int> aiMove = find_best_move(board, AImove, 9); // Use appropriate depth
		applyAIMove(aiMove, AImove);
	}
	game++;
	// Connect ButtonClick function to each button's OnClick event
	Button1->OnClick = ButtonClick;
	Button2->OnClick = ButtonClick;
	Button3->OnClick = ButtonClick;
	Button4->OnClick = ButtonClick;
	Button5->OnClick = ButtonClick;
	Button6->OnClick = ButtonClick;
	Button7->OnClick = ButtonClick;
	Button8->OnClick = ButtonClick;
	Button9->OnClick = ButtonClick;

}
