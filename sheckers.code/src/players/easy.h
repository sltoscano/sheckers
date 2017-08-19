// Copyright: Steven Toscano

#pragma once

class ComputerEasy :
	public Player
{
public:
	ComputerEasy(wchar_t* wstrName, PieceType pt, int iPieceCount,
		IFeedback *pFeedback);
	~ComputerEasy();

	// Player methods
	MoveResult MakeMove(IBoard *pBoard, MoveResult mrLastMove);

private:
	IFeedbackPtr m_spFeedback;

private:
	// Disallow copy and assignment
	ComputerEasy(const ComputerEasy &);
	ComputerEasy& operator=(const ComputerEasy &);
};
