# SudokuSolver

```c++
enum Sudoku::Type
{
	solved,
	unsolved,
	error
};
```

solved – means that Sudoku is solved
unsolved – means that Sudoku is not solved (it has at least one empty field)
error – means that Sudoku can’t be solved due to mistakes on board

Sudoku::Sudoku()
Creates an empty sudoku board.

Sudoku::Sudoku(const std::string & filename)
Creates sudoku from file. Internally it calls Sudoku::readFromFile().

Sudoku::Type Sudoku::solve(bool enableBruteForce = false)
Solve the Sudoku. Firstly, it tries to solve Sudoku using standard algorithm, so it’s possible to not solve Sudoku. If enableBruteForce flag is set on true and standard algorithm returned Type::unsolved, function uses brute force to solve Sudoku. Brute force guarantees to find solution, if any is possible and returns the first one (there can be more than one solution). Then Sudoku::solve() returns only Type::solved, or Type::error.
Note: Beware of using brute force to solve sudoku. With only few given numbers, it can take a long time to find solution!

Sudoku::Type Sudoku::isSolved() const
Function that checks if Sudoku is already solved.

Sudoku::Type Sudoku::isSolvable() const
Function to check if Sudoku is solvable, using only standard algorithm. Internally it calls Sudoku::solve(false).

void Sudoku::setNumber(short line, short column, short number)
Set number in line and column. If given number is out of range [1..9], number is deleted.


short Sudoku::getNumber(short line, short column) const
Get number from line and column. If number is out of range [1..9], it returns 0.

bool Sudoku::readFromFile(const std::string & filename)
Read sudoku from file.

void Sudoku::saveToFile(const std::string & filename) const
Save sudoku to file. If number is not set, it writes ‘X’.

static Sudoku Sudoku::generateBoard(float difficulty)
Generate board and delete some of the numbers. Difficulty should be in range [0..100] and means that each number has (difficulty)% of chance to be deleted. If given difficulty is 0, none of the numbers is deleted. If given difficulty is 100, it returns an empty Sudoku board. 
