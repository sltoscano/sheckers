// Copyright: Steven Toscano

#pragma once

class Human :
	public Player
{
public:
	Human(wchar_t *wstrName, PieceType pt, int iPieceCount,
		IFeedback *pFeedback);
	~Human();

	// Player methods
	MoveResult MakeMove(IBoard *pBoard, MoveResult mrLastMove);

private:
	IFeedbackPtr m_spFeedback;

private:
	// Disallow copy and assignment
	Human(const Human &);
	Human& operator=(const Human &);
};
