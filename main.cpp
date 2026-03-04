#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <string>

using namespace std;

enum Suit { HEARTS, DIAMONDS, CLUBS, SPADES };

struct Card {
    int rank; // 1 - 13
    Suit suit;
    bool faceUp;

    string toString() const {
        if (!faceUp) return "XX";

        string r;
        if (rank == 1) r = "A";
        else if (rank == 11) r = "J";
        else if (rank == 12) r = "Q";
        else if (rank == 13) r = "K";
        else r = to_string(rank);

        char s;
        if (suit == HEARTS) s = 'H';
        else if (suit == DIAMONDS) s = 'D';
        else if (suit == CLUBS) s = 'C';
        else s = 'S';

        return r + s;
    }

    bool isRed() const {
        return suit == HEARTS || suit == DIAMONDS;
    }
};

class Solitaire {
private:
    vector<Card> stock;
    vector<Card> waste;
    vector<vector<Card>> tableau;
    vector<vector<Card>> foundation;

public:
    Solitaire() {
        initDeck();
        deal();
    }

    void initDeck() {
        vector<Card> deck;

        for (int s = 0; s < 4; s++) {
            for (int r = 1; r <= 13; r++) {
                deck.push_back({ r, (Suit)s, false });
            }
        }

        shuffle(deck.begin(), deck.end(), default_random_engine(time(0)));
        stock = deck;
        tableau.resize(7);
        foundation.resize(4);
    }

    void deal() {
        for (int i = 0; i < 7; i++) {
            for (int j = 0; j <= i; j++) {
                Card c = stock.back();
                stock.pop_back();
                if (j == i) c.faceUp = true;
                tableau[i].push_back(c);
            }
        }
    }

    void drawCard() {
        if (stock.empty()) {
            stock = waste;
            for (auto& c : stock) c.faceUp = false;
            waste.clear();
            return;
        }

        Card c = stock.back();
        stock.pop_back();
        c.faceUp = true;
        waste.push_back(c);
    }

    bool moveWasteToTableau(int col) {
        if (waste.empty()) return false;

        Card card = waste.back();

        if (canPlaceOnTableau(card, col)) {
            tableau[col].push_back(card);
            waste.pop_back();
            return true;
        }
        return false;
    }

    bool moveTableauToTableau(int from, int to) {
        if (tableau[from].empty()) return false;

        Card card = tableau[from].back();
        if (!card.faceUp) return false;

        if (canPlaceOnTableau(card, to)) {
            tableau[to].push_back(card);
            tableau[from].pop_back();

            if (!tableau[from].empty())
                tableau[from].back().faceUp = true;

            return true;
        }
        return false;
    }

    bool moveTableauToFoundation(int col) {
        if (tableau[col].empty()) return false;

        Card card = tableau[col].back();
        if (!card.faceUp) return false;

        int f = card.suit;

        if (canPlaceOnFoundation(card, f)) {
            foundation[f].push_back(card);
            tableau[col].pop_back();

            if (!tableau[col].empty())
                tableau[col].back().faceUp = true;

            return true;
        }
        return false;
    }

    bool canPlaceOnTableau(Card card, int col) {
        if (tableau[col].empty()) {
            return card.rank == 13;
        }

        Card top = tableau[col].back();

        return top.faceUp &&
            top.rank == card.rank + 1 &&
            top.isRed() != card.isRed();
    }

    bool canPlaceOnFoundation(Card card, int f) {
        if (foundation[f].empty())
            return card.rank == 1;

        Card top = foundation[f].back();
        return top.rank + 1 == card.rank;
    }

    bool checkWin() {
        for (int i = 0; i < 4; i++)
            if (foundation[i].size() != 13)
                return false;
        return true;
    }

    void display() {
        cout << "\nStock: " << stock.size();
        cout << " | Waste: ";
        if (!waste.empty())
            cout << waste.back().toString();
        else
            cout << "--";

        cout << "\nFoundations: ";
        for (int i = 0; i < 4; i++) {
            if (!foundation[i].empty())
                cout << foundation[i].back().toString() << " ";
            else
                cout << "-- ";
        }

        cout << "\n\nTableau:\n";
        for (int i = 0; i < 7; i++) {
            cout << i << ": ";
            for (auto& c : tableau[i])
                cout << c.toString() << " ";
            cout << endl;
        }
    }

    void gameLoop() {
        while (true) {
            display();

            if (checkWin()) {
                cout << "\nYOU WIN!\n";
                break;
            }

            cout << "\nCommands:\n";
            cout << "1 = Draw\n";
            cout << "2 = Waste -> Tableau\n";
            cout << "3 = Tableau -> Tableau\n";
            cout << "4 = Tableau -> Foundation\n";
            cout << "0 = Exit\n";

            int cmd;
            cin >> cmd;

            if (cmd == 0) break;
            if (cmd == 1) drawCard();
            if (cmd == 2) {
                int col;
                cout << "Column: ";
                cin >> col;
                moveWasteToTableau(col);
            }
            if (cmd == 3) {
                int from, to;
                cout << "From To: ";
                cin >> from >> to;
                moveTableauToTableau(from, to);
            }
            if (cmd == 4) {
                int col;
                cout << "Column: ";
                cin >> col;
                moveTableauToFoundation(col);
            }
        }
    }
};

int main() {
    Solitaire game;
    game.gameLoop();
    return 0;
}
