#include "Sudoku.hpp"

#include <fstream>
#include <cctype>
#include <random>
#include <algorithm>
#include <utility>
#include <bitset>

const std::vector<short> Sudoku::defaultPossibilities = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

Sudoku::Sudoku()
{
	for (short line = 0; line < 9; ++line)
	{
		board.push_back(std::vector<Possibilities>(9));
		for (short column = 0; column < 9; ++column)
			board[line][column] = defaultPossibilities;
	}
}

Sudoku::Sudoku(const std::string & filename)
{
	readFromFile(filename);
}

Sudoku::Type Sudoku::solve(bool enableBruteForce)
{
	Type type = isSolved();
	if (type == Type::error || type == Type::solved)
		return type;

	std::vector<std::bitset<9>> checkedSquares(9);
	short checkCount(0);
	bool change = true;

	auto basicCheck = [this, &checkedSquares, &checkCount]() -> bool
	{
		Coord coord;
		bool change = false;
		do
		{
			if (!checkedSquares[coord.line][coord.column] &&
				board[coord.line][coord.column].size() == 1)
			{
				checkedSquares[coord.line][coord.column] = true;
				++checkCount;
				change |= solveNumber(coord.line, coord.column);
			}

		} while (coord.next());
		return change;
	};

	// must be called after any other solve function
	while (basicCheck());

	while (checkCount != 81 && change)
	{
		change = false;

		if (solveLines())
		{
			change = true;
			while (basicCheck());
		}
		if (solveColumns())
		{
			change = true;
			while (basicCheck());
		}
		if (solveBigSquares())
		{
			change = true;
			while (basicCheck());
		}
	}

	type = isSolved();
	if (type == Type::error || type == Type::solved)
		return type;

	if (enableBruteForce)
		return solveWithBruteForce();
	return type;
}

Sudoku::Type Sudoku::isSolved() const
{
	Coord coord;

	// validates the board
	do
	{
		auto & currentSquare = board[coord.line][coord.column];
		auto size = currentSquare.size();

		if (size == 0)
			return Type::error;
		if (size == 1 && (currentSquare[0] < 1 || currentSquare[0] > 9))
			return Type::error;

	} while (coord.next());

	// check if board is unsolved
	coord = Coord();
	do
	{
		auto & currentSquare = board[coord.line][coord.column];
		auto size = currentSquare.size();

		if (size > 1)
			return Type::unsolved;

	} while (coord.next());

	const std::vector<short> defaultNumberCount(9, 0);
	std::vector<short> numberCount;

	// count in lines
	for (short line = 0; line < 9; ++line)
	{
		coord.line = line;
		coord.column = 0;
		numberCount = defaultNumberCount;
		do
		{
			auto & currentSquare = board[coord.line][coord.column];
			++numberCount[currentSquare[0] - 1];

		} while (coord.nextColumn());

		for (auto & count : numberCount)
		{
			if (count > 2)
				return Type::error;
		}
	}

	// count in columns
	for (short column = 0; column < 9; ++column)
	{
		coord.line = 0;
		coord.column = column;
		numberCount = defaultNumberCount;
		do
		{
			auto & currentSquare = board[coord.line][coord.column];
			++numberCount[currentSquare[0] - 1];

		} while (coord.nextLine());

		for (auto & count : numberCount)
		{
			if (count > 2)
				return Type::error;
		}
	}

	// count in squares
	auto bigSquares = std::move(getAllBigSquares());
	for (auto & square : bigSquares)
	{
		numberCount = defaultNumberCount;
		for (short line = square.lineStart; line <= square.lineEnd; ++line)
		{
			for (short column = square.columnStart; column <= square.columnEnd; ++column)
			{
				++numberCount[board[line][column][0] - 1];
			}
		}

		for (auto & count : numberCount)
		{
			if (count > 2)
				return Type::error;
		}
	}

	return Type::solved;
}

Sudoku::Type Sudoku::isSolvable() const
{
	Sudoku sudokuCopy(*this);
	return sudokuCopy.solve(false);
}

void Sudoku::setNumber(short line, short column, short number)
{
	if (number >= 1 && number <= 9)
	{
		board[line][column].clear();
		board[line][column].push_back(number);
	}
	else
		board[line][column] = defaultPossibilities;
}

short Sudoku::getNumber(short line, short column) const
{
	if (board[line][column].size() == 1)
		return board[line][column][0];
	return 0;
}

bool Sudoku::readFromFile(const std::string & filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
		return false;

	*this = Sudoku();
	Coord coord;
	char c;

	do
	{
		if (!(file >> c))
			break;
		if (std::isspace(c) || std::iscntrl(c))
			continue;

		if (c >= '1' && c <= '9')
		{
			int number = c - '0';
			setNumber(coord.line, coord.column, number);
		}

	} while (coord.next());

	return true;
}

void Sudoku::saveToFile(const std::string & filename) const
{
	std::ofstream file(filename);
	Coord coord;

	do
	{
		if (board[coord.line][coord.column].size() == 1)
			file << board[coord.line][coord.column][0];
		else
			file << 'X';

		if ((coord.column + 1) % 3 == 0)
			file << '\t';
		else
			file << ' ';

		if (coord.column == 8)
		{
			file << std::endl;
			if ((coord.line + 1) % 3 == 0)
				file << std::endl;
		}
			
	} while (coord.next());
}

Sudoku Sudoku::generateBoard(float difficulty)
{
	std::random_device rd;
	std::mt19937 mt(rd());

	Sudoku sudoku;
	auto possibillities = sudoku.board;
	Coord coord;

	// fill board with numbers
	bool isDone = false;
	while (!isDone)
	{
		auto & currentSquare = possibillities[coord.line][coord.column];

		if (currentSquare.size() > 0)
		{
			std::uniform_int_distribution<int> random(0, currentSquare.size() - 1);
			short index = random(mt);
			short number = currentSquare[index];

			currentSquare.erase(std::find(currentSquare.begin(), currentSquare.end(), number));

			sudoku.setNumber(coord.line, coord.column, number);

			if (!sudoku.numberCollide(coord.line, coord.column))
			{
				if (!coord.next())
					isDone = true;
			}
			else
				continue;
		}
		else
		{
			sudoku.setNumber(coord.line, coord.column, 0);
			currentSquare = defaultPossibilities;
			coord.prev();
		}
	}


	// deleting random numbers
	if (difficulty < 0.f)
		difficulty = 0.f;
	if (difficulty > 100.f)
		difficulty = 100.f;

	int percentage = static_cast<int>(difficulty * 10.f);

	coord = Coord();
	std::uniform_int_distribution<int> random(1, 1000);
	do
	{
		int randomNumber = random(mt);
		if (randomNumber <= percentage)
		{
			auto & currentSquare = sudoku.board[coord.line][coord.column];
			currentSquare = defaultPossibilities;
		}

	} while (coord.next());

	return sudoku;
}

Sudoku::BigSquare Sudoku::getBigSquare(short line, short column)
{
	BigSquare bigSquare;

	if (line < 3)
	{
		bigSquare.lineStart = 0;
		bigSquare.lineEnd = 2;
	}
	else if (line < 6)
	{
		bigSquare.lineStart = 3;
		bigSquare.lineEnd = 5;
	}
	else
	{
		bigSquare.lineStart = 6;
		bigSquare.lineEnd = 8;
	}

	if (column < 3)
	{
		bigSquare.columnStart = 0;
		bigSquare.columnEnd = 2;
	}
	else if (column < 6)
	{
		bigSquare.columnStart = 3;
		bigSquare.columnEnd = 5;
	}
	else
	{
		bigSquare.columnStart = 6;
		bigSquare.columnEnd = 8;
	}

	return bigSquare;
}

std::vector<Sudoku::BigSquare> Sudoku::getAllBigSquares()
{
	std::vector<BigSquare> squares;
	for (short line = 0; line < 8; line += 3)
	{
		for (short column = 0; column < 8; column += 3)
		{
			squares.push_back(getBigSquare(line, column));
		}
	}

	return squares;
}

bool Sudoku::numberCollide(short line, short column) const
{
	if (board[line][column].size() != 1)
		return false;

	short number = board[line][column][0];

	Coord coord;
	coord.column = column;
	do
	{
		if (coord.line != line &&
			board[coord.line][coord.column].size() == 1 &&
			board[coord.line][coord.column][0] == number)
			return true;
	} while (coord.nextLine());

	coord = Coord();
	coord.line = line;
	do
	{
		if (coord.column != column &&
			board[coord.line][coord.column].size() == 1 &&
			board[coord.line][coord.column][0] == number)
			return true;
	} while (coord.nextColumn());


	BigSquare biqSquare = getBigSquare(line, column);
	for (short l = biqSquare.lineStart; l <= biqSquare.lineEnd; ++l)
	{
		for (short c = biqSquare.columnStart; c <= biqSquare.columnEnd; ++c)
		{
			if (line != l && column != c && board[l][c].size() == 1 && board[l][c][0] == number)
				return true;
		}
	}

	return false;
}

bool Sudoku::deletePossibility(short line, short column, short number)
{
	auto & currentSquare = board[line][column];
	auto iter = std::find(currentSquare.begin(), currentSquare.end(), number);
	if (iter == currentSquare.end())
		return false;
	currentSquare.erase(iter);
	return true;
}

bool Sudoku::solveNumber(short line, short column)
{
	auto & currentSquare = board[line][column];
	if (currentSquare.size() != 1)
		return false;

	bool change = false;
	short number = currentSquare[0];
	Coord coord;

	// delete number in line
	coord.line = line;
	coord.column = 0;
	do
	{
		if (coord.column != column)
			change |= deletePossibility(coord.line, coord.column, number);

	} while (coord.nextColumn());
	// delete number in column
	coord.line = 0;
	coord.column = column;
	do
	{
		if (coord.line != line)
			change |= deletePossibility(coord.line, coord.column, number);

	} while (coord.nextLine());

	// delete number in big square
	BigSquare bigSquare = getBigSquare(line, column);

	for (short l = bigSquare.lineStart; l <= bigSquare.lineEnd; ++l)
	{
		for (short c = bigSquare.columnStart; c <= bigSquare.columnEnd; ++c)
		{
			if (l != line || c != column)
				change |= deletePossibility(l, c, number);
		}
	}

	return change;
}

bool Sudoku::solveLines()
{
	bool change = false;

	const std::vector<short> defaultNumberCount(9, 0);
	std::vector<short> numberCount;
	std::vector<short> lastPos(9);

	for (short line = 0; line < 9; ++line)
	{
		numberCount = defaultNumberCount;
		for (short column = 0; column < 9; ++column)
		{
			auto & currentSquare = board[line][column];
			if (currentSquare.size() == 1)
				continue;

			for (short poss = 0; poss < currentSquare.size(); ++poss)
			{
				short number = currentSquare[poss];
				++numberCount[number - 1];
				lastPos[number - 1] = column;
			}
		}

		for (short number = 0; number < 9; ++number)
		{
			if (numberCount[number] == 1)
			{
				change = true;
				board[line][lastPos[number]].clear();
				board[line][lastPos[number]].push_back(number + 1);
			}
		}
	}

	// dalsza czesc algorytmu

	return change;
}

bool Sudoku::solveColumns()
{
	bool change = false;

	const std::vector<short> defaultNumberCount(9, 0);
	std::vector<short> numberCount;
	std::vector<short> lastPos(9);

	for (short column = 0; column < 9; ++column)
	{
		numberCount = defaultNumberCount;
		for (short line = 0; line < 9; ++line)
		{
			auto & currentSquare = board[line][column];
			if (currentSquare.size() == 1)
				continue;

			for (short poss = 0; poss < currentSquare.size(); ++poss)
			{
				short number = currentSquare[poss];
				++numberCount[number - 1];
				lastPos[number - 1] = line;
			}
		}

		for (short number = 0; number < 9; ++number)
		{
			if (numberCount[number] == 1)
			{
				change = true;
				board[lastPos[number]][column].clear();
				board[lastPos[number]][column].push_back(number + 1);
			}
		}
	}

	// dalsza czesc algorytmu

	return change;
}

bool Sudoku::solveBigSquares()
{
	bool change = false;
	auto bigSquares = getAllBigSquares();

	const std::vector<short> defaultNumberCount(9, 0);
	std::vector<short> numberCount;
	std::vector<std::pair<short, short>> lastPos(9);

	for (auto & square : bigSquares)
	{
		numberCount = defaultNumberCount;
		for (short line = square.lineStart; line <= square.lineEnd; ++line)
		{
			for (short column = square.columnStart; column <= square.columnEnd; ++column)
			{
				auto & currentSquare = board[line][column];
				if (currentSquare.size() == 1)
					continue;

				for (short poss = 0; poss < currentSquare.size(); ++poss)
				{
					short number = currentSquare[poss];
					++numberCount[number - 1];
					lastPos[number - 1] = std::make_pair(line, column);
				}
			}
		}

		for (short number = 0; number < 9; ++number)
		{
			if (numberCount[number] == 1)
			{
				change = true;
				auto & coord = lastPos[number];
				board[coord.first][coord.second].clear();
				board[coord.first][coord.second].push_back(number + 1);
			}
		}
	}

	// dalsza czesc algorytmu

	return change;
}

Sudoku::Type Sudoku::solveWithBruteForce()
{
	auto defaultPoss = board;
	auto possibilities = board;
	Coord coord;

	Type isDone = unsolved;
	bool forwardFlag = true;

	while (true)
	{	
		if (defaultPoss[coord.line][coord.column].size() == 1)
		{
			if (forwardFlag)
			{
				if (!coord.next())
				{
					isDone = solved;
					break;
				}
			}
			else
			{
				if (!coord.prev())
				{
					isDone = error;
					break;
				}
			}
		}

		auto & currentSquare = possibilities[coord.line][coord.column];

		if (currentSquare.size() > 0)
		{
			short number = currentSquare.back();
			currentSquare.pop_back();

			setNumber(coord.line, coord.column, number);

			if (!numberCollide(coord.line, coord.column))
			{
				forwardFlag = true;
				if (!coord.next())
				{
					isDone = solved;
					break;
				}
			}
		}
		else
		{
			currentSquare = board[coord.line][coord.column] = defaultPoss[coord.line][coord.column];
			forwardFlag = false;
			if (!coord.prev())
			{
				isDone = error;
				break;
			}
		}
	}

	return isDone;
}

Sudoku::Coord::Coord()
	: line(0),
	column(0)
{
}

bool Sudoku::Coord::next()
{
	if (line == 8 && column == 8)
		return false;
	++column;
	if (column > 8)
	{
		column = 0;
		++line;
	}
	return true;
}

bool Sudoku::Coord::prev()
{
	if (line == 0 && column == 0)
		return false;
	--column;
	if (column < 0)
	{
		column = 8;
		--line;
	}
	return true;
}

bool Sudoku::Coord::nextLine()
{
	if (line == 8)
		return false;
	++line;
	return true;
}

bool Sudoku::Coord::nextColumn()
{
	if (column == 8)
		return false;
	++column;
	return true;
}

bool Sudoku::Coord::operator==(const Coord & coord) const
{
	if (line == coord.line && column == coord.column)
		return true;
	return false;
}

bool Sudoku::Coord::operator!=(const Coord & coord) const
{
	return !(*this == coord);
}
