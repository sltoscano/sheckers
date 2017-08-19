// Copyright: Steven Toscano

#pragma once

class Human :
	public Player
{
public:
	Human(wstring wstrName, PieceType pt, int iPieceCount);
	~Human();

	// Player methods
	MoveResult MakeMove(IBoard *pBoard, MoveResult mrLastMove);

private:
	// Disallow copy and assignment
	Human(const Human &);
	Human& operator=(const Human &);
};
