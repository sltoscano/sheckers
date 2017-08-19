// Copyright: Steven Toscano

#pragma once

class ComputerHard :
	public Player
{
public:
	ComputerHard(wstring wstrName, PieceType pt, int iPieceCount);
	~ComputerHard();

	// Player methods
	MoveResult MakeMove(IBoard *pBoard, MoveResult mrLastMove);

private:
	// Disallow copy and assignment
	ComputerHard(const ComputerHard &);
	ComputerHard& operator=(const ComputerHard &);
};
