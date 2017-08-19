// Copyright: Steven Toscano

#pragma once

class ComputerHard :
	public Player
{
public:
	ComputerHard(wchar_t *wstrName, PieceType pt, int iPieceCount,
		IFeedback *pFeedback);
	~ComputerHard();

	// Player methods
	MoveResult MakeMove(IBoard *pBoard, MoveResult mrLastMove);

private:
	IFeedbackPtr m_spFeedback;

private:
	// Disallow copy and assignment
	ComputerHard(const ComputerHard &);
	ComputerHard& operator=(const ComputerHard &);
};

class GameTree
{
public:
	GameTree(IBoard *pBoard, PieceType pt, int iMaxDepth);
	~GameTree();
	bool GameTree::Generate(int iTop, int iBreadth, IBoard *pBoard, IPlayer *pPlayer, MoveResult mrLastMove, 
							int iPos, DirectionKind dk, MoveKind mk, int iDepth);

public:
	vector<int> m_capturedPieceCounts;

private:
	int m_iMaxDepth;
	PieceType m_ptSeed;
	IBoardPtr m_spBoardSeed;

private:
	// Disallow copy and assignment
	GameTree(const GameTree &);
	GameTree& operator=(const GameTree &);
};
