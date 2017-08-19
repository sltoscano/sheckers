// Copyright: Steven Toscano

#pragma once

struct Move
{
	int iPos;
	MoveKind mk;
	DirectionKind dk;
	MoveResult mr;
};

class Rules
{
public:
	Rules(IBoard *pBoard);
	~Rules();
	bool GetLegalMoves(IPlayer *pPlayer, MoveResult mrLastMove, vector<Move> &moves) const;
	bool IsLegal(int iPos, DirectionKind dk, MoveKind mk, vector<Move> &moves) const;
	bool CanLosePieceIfMoveTaken(IBoard *pBoard, IPlayer *pPlayer, Move moveAttempt) const;
	bool GenerateJumpTree(int iFromPos, vector<pair<IBoard,Move>> finalJumpBoards) const;

private:
	IBoardPtr m_spBoard;

private:
	// Disallow copy and assignment
	Rules(const Rules &);
	Rules& operator=(const Rules &);
};
