#ifndef SUDOKU_HPP_
#define SUDOKU_HPP_

#include <string>
#include <vector>

class Sudoku
{
public:
	enum Type
	{
		solved,
		unsolved,
		error
	};

	Sudoku();
	explicit Sudoku(const std::string & filename);

	Type solve(bool enableBruteForce = false);
	Type isSolved() const;
	Type isSolvable() const;

	void setNumber(short line, short column, short number);
	short getNumber(short line, short column) const;

	bool readFromFile(const std::string & filename);
	void saveToFile(const std::string & filename) const;

	static Sudoku generateBoard(float difficulty);

private:
	struct Coord
	{
		short line;
		short column;

		Coord();
		bool next();
		bool prev();
		bool nextLine();
		bool nextColumn();
		bool operator==(const Coord & coord) const;
		bool operator!=(const Coord & coord) const;
	};
	struct BigSquare
	{
		short lineStart;
		short lineEnd;
		short columnStart;
		short columnEnd;
	};

	static BigSquare getBigSquare(short line, short column);
	static std::vector<BigSquare> getAllBigSquares();
	bool numberCollide(short line, short column) const;

	bool deletePossibility(short line, short column, short number);
	bool solveNumber(short line, short column);

	bool solveLines();
	bool solveColumns();
	bool solveBigSquares();

	Type solveWithBruteForce();

private:
	using Possibilities = std::vector<short>;
	static const std::vector<short> defaultPossibilities;

	std::vector<std::vector<Possibilities>> board;
};

#endif // !SUDOKU_HPP_
