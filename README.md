# SudokuSolver

## Documentation

```c++
enum Sudoku::Type
{
	solved,
	unsolved,
	error
};
```

```c++ solved``` – means that Sudoku is solved

```c++ unsolved``` – means that Sudoku is not solved (it has at least one empty field)

```c++ error``` – means that Sudoku can’t be solved due to mistakes on board



```c++ Sudoku::Sudoku()```
Creates an empty sudoku board.


```c++ Sudoku::Sudoku(const std::string & filename)```
Creates sudoku from file. Internally it calls Sudoku::readFromFile().


```c++ Sudoku::Type Sudoku::solve(bool enableBruteForce = false)```
Solve the Sudoku. Firstly, it tries to solve Sudoku using standard algorithm, so it’s possible to not solve Sudoku. If enableBruteForce flag is set on true and standard algorithm returned Type::unsolved, function uses brute force to solve Sudoku. Brute force guarantees to find solution, if any is possible and returns the first one (there can be more than one solution). Then Sudoku::solve() returns only Type::solved, or Type::error.
Note: Beware of using brute force to solve sudoku. With only few given numbers, it can take a long time to find solution!


```c++ Sudoku::Type Sudoku::isSolved() const```
Function that checks if Sudoku is already solved.


```c++ Sudoku::Type Sudoku::isSolvable() const```
Function to check if Sudoku is solvable, using only standard algorithm. Internally it calls Sudoku::solve(false).


```c++ void Sudoku::setNumber(short line, short column, short number)```
Set number in line and column. If given number is out of range [1..9], number is deleted.


```c++ short Sudoku::getNumber(short line, short column) const```
Get number from line and column. If number is out of range [1..9], it returns 0.


```c++ bool Sudoku::readFromFile(const std::string & filename)```
Read sudoku from file.


```c++ void Sudoku::saveToFile(const std::string & filename) const```
Save sudoku to file. If number is not set, it writes ‘X’.


```c++ static Sudoku Sudoku::generateBoard(float difficulty)```
Generate board and delete some of the numbers. Difficulty should be in range [0..100] and means that each number has (difficulty)% of chance to be deleted. If given difficulty is 0, none of the numbers is deleted. If given difficulty is 100, it returns an empty Sudoku board. 
