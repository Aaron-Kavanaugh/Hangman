#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <random>
#include <sstream>
#include <limits>
#include <cctype>
#include <algorithm>

constexpr int MAX_TRIES = 7;

//Generate random number from array of words
int randomGen(int wordCount)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	//wordCount - 1 to prevent generating a number larger then vector
	std::uniform_int_distribution<int> random(0, wordCount - 1);
	return random(gen);
}
//update console to prevent multiple instences of hangman stage
void clearScreen()
{
	system("cls");
}
//Show currently guessed letters to player
void displayVector(const std::vector<char>& vec)
{
	std::cout << "Guessed letters: ";
	for (char ch : vec) {
		std::cout << ch << " ";
	}
	std::cout << std::endl;
}
//check to see if all letters have been guessed 
bool hasUserWon(const std::vector<char>& hiddenWord)
{
	return (std::find(hiddenWord.begin(), hiddenWord.end(), '_') == hiddenWord.end());;
}
//check to see if user guessed any letters correct if yes return true and update hidden word with new letter
bool updateHiddenWord(std::string& word, std::vector<char>& displayHiddenWord, char userGuess)
{
	bool lettersFound{ false };
	for (int i = 0; i < word.size(); ++i) {
		if (word[i] == userGuess) {
			displayHiddenWord[i] = userGuess;
			lettersFound = true;
		}
	}
	return lettersFound;
}
//display hangman stage 
void hangManParts(int wrongTries) {

	clearScreen();

	std::vector<std::vector<char>> hangManStage
	{
		{' ', ' ', ' ', ' '},
		{' ', ' ', ' ', ' '},
		{' ', ' ', ' ', ' '},
		{' ', ' ', ' ', ' '},
		{' ', ' ', ' ', ' '},
		{' ', ' ', ' ', ' '},
		{' ', ' ', ' ', ' '}
	};

	std::vector<std::pair<int, int>> parts = {
		{1, 2}, {2, 2}, {3, 2}, {3, 1}, {3, 3}, {4, 1}, {4, 3}
	};

	std::vector<std::pair<int, int>> stage = {
		{0, 2}, {0, 1}, {0, 0}, {1, 2}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}
	};

	std::vector<char> stageParts = { '_', '_', '_', '|', '|', '|', '|', '|', '|', '|' };
	std::vector<char> partChars = { '|', 'O', '|', '/', '\\', '/', '\\' };

	for (int i = 0; i < stage.size(); ++i) {
		hangManStage[stage[i].first][stage[i].second] = stageParts[i];
	}

	for (int i = 0; i < wrongTries && i < parts.size(); ++i) {
		hangManStage[parts[i].first][parts[i].second] = partChars[i];
	}


	for (const auto& row : hangManStage) {
		for (char c : row) {
			std::cout << c << " ";
		}
		std::cout << std::endl;
	}
}
//Pulls words stored in txt document and stores in vector
std::string seedRandomWord()
{
	std::vector<std::string> words;
	std::string line;
	std::ifstream myFile("aBunchOfRandomWords.txt");

	if (myFile.is_open()) {
		while (getline(myFile, line)) {
			std::stringstream ss(line);
			std::string word;
			while (ss >> word) {
				std::transform(word.begin(), word.end(), word.begin(), ::tolower);
				words.push_back(word);
			}
		}
		myFile.close();
	}
	else {
		std::cout << "Unable to open file";
	}

	int randomWordIndex{ randomGen(static_cast<int>(words.size())) };
	return words[randomWordIndex];
}
//checks to see if user has already used a letter 
bool hasUserUsedLetter(const std::vector<char>& userLetters, char userGuess)
{
	return std::find(userLetters.begin(), userLetters.end(), userGuess) != userLetters.end();
}
//check that user input was a letter
bool isLetter(char ch) {
	return std::isalpha(ch);
}

void gameStart()
{

	std::string secretWord{ seedRandomWord() };
	int wrongTries{ 0 };
	int TriesRemaining{ MAX_TRIES };
	char userGuess;
	bool endGameLoop{ false };
	std::vector<char> userLetters;
	std::vector<char> displayHiddenWord(secretWord.size(), '_');
	hangManParts(wrongTries);

	while (!endGameLoop) {

		std::cout << "Tries remaining: " << TriesRemaining << '\n';
		if (hasUserWon(displayHiddenWord)) {
			std::cout << "\033[32m" << "Congratulations! You've guessed the word: " << "\033[0m" << secretWord << std::endl;
			endGameLoop = true;
			continue;
		}
		if (wrongTries >= 7) {
			std::cout << "\033[31m" << "Too many tries, you lose. The word was: " << "\033[0m" << secretWord << std::endl;
			endGameLoop = true;
			continue;
		}

		std::cout << "Current word: ";
		displayVector(displayHiddenWord);
		std::cout << std::endl;
		std::cout << "type in a letter to guess: ";
		std::cin >> userGuess;
		userGuess = tolower(userGuess);

		if (!isLetter(userGuess)) {
			hangManParts(wrongTries);
			std::cout << "\033[36m" << "Please enter a valid letter.\n" << "\033[0m";
			continue;
		}

		if (hasUserUsedLetter(userLetters, userGuess)) {
			hangManParts(wrongTries);
			std::cout << "\033[36m" << "You have already guessed that letter. try a different one.\n" << "\033[0m";
			displayVector(userLetters);
			continue;
		}
		else if (!updateHiddenWord(secretWord, displayHiddenWord, userGuess)) {
			++wrongTries;
			--TriesRemaining;
		}

		hangManParts(wrongTries);

		userLetters.push_back(userGuess);

		displayVector(userLetters);

	}

}

int main()
{

	gameStart();

	return 0;
}