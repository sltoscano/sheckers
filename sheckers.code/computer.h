// Copyright: Steven Toscano

#pragma once

class Computer :
	public Player
{
public:
	Computer(wstring wstrName, PieceType pt, int iPieceCount);
	~Computer();

	// Player methods
	MoveResult MakeMove(IBoard *pBoard, MoveResult mrLastMove);

private:
	// Disallow copy and assignment
	Computer(const Computer &);
	Computer& operator=(const Computer &);
};
