// Copyright: Steven Toscano

#pragma once

class ComputerEasy :
	public Player
{
public:
	ComputerEasy(wstring wstrName, PieceType pt, int iPieceCount);
	~ComputerEasy();

	// Player methods
	MoveResult MakeMove(IBoard *pBoard, MoveResult mrLastMove);

private:
	// Disallow copy and assignment
	ComputerEasy(const ComputerEasy &);
	ComputerEasy& operator=(const ComputerEasy &);
};
