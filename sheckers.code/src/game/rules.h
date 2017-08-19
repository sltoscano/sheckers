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
	Rules();
	~Rules();
	bool IsLegal(int iPos, DirectionKind dk, MoveKind mk, vector<Move> &moves);
	bool GetLegalMoves(IBoard *pBoard, IPlayer *pPlayer, MoveResult mrLastMove, vector<Move> &moves);
	bool CanLosePieceIfMoveTaken(IBoard *pBoard, IPlayer *pPlayer, const Move &moveAttempt);

private:
	// Disallow copy and assignment
	Rules(const Rules &);
	Rules& operator=(const Rules &);
};

struct LeafNode
{
	Move seedMove;
	IBoardPtr spBoard;
	int iPos;
	int iJumpDepth;
};

static bool LeafGreater(LeafNode &lhs, LeafNode &rhs)
{
	return lhs.iJumpDepth > rhs.iJumpDepth;
}

class JumpTree
{
public:
	JumpTree(Move move);
	~JumpTree();
	bool Generate(IBoard *pBoard, IPlayer *pPlayer, Move move, int iDepth);

public:
	vector<LeafNode> m_leafNodes;
	Move m_seedMove;

private:
	vector<int> m_visitedJumps;

private:
	// Disallow copy and assignment
	JumpTree(const JumpTree &);
	JumpTree& operator=(const JumpTree &);
};
