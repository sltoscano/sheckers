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
	static bool IsLegal(int iPos, DirectionKind dk, MoveKind mk, vector<Move> &moves);
	static bool GetLegalMoves(IBoard *pBoard, IPlayer *pPlayer, MoveResult mrLastMove, vector<Move> &moves);
	static bool CanLosePieceIfMoveTaken(IBoard *pBoard, IPlayer *pPlayer, const Move &moveAttempt);

private:
	// Disallow construction, copy and assignment
	Rules();
	Rules(const Rules &);
	Rules& operator=(const Rules &);
};
