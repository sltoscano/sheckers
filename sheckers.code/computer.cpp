// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "player.h"
#include "rules.h"
#include "io.h"

#include "computer.h"

Computer::Computer(wstring wstrName, PieceType pt, int iPieceCount) :
	Player(wstrName, pt, iPieceCount)
{
}

Computer::~Computer()
{
}

MoveResult Computer::MakeMove(IBoard *pBoard, MoveResult mrLastMove)
{
	IBoardPtr spBoard(pBoard);
	FAILED_ASSERT_RETURN(mrError, spBoard != NULL);

	Rules rules(spBoard);
	vector<Move> moves;
	FAILED_ASSERT_RETURN(mrError, rules.GetLegalMoves(this, mrLastMove, moves));

	UserOutput output(m_wstrName);

	if (moves.empty())
	{
		output.Write(NULL, EmptyPosition, dkUp, mkLeft, mrNone);
		return mrNone;
	}

	vector<Move> legalJumps;
	vector<Move> legalNonJumps;
	for each (const Move &move in moves)
	{
		if (move.mr == mrLegalJump)
			legalJumps.push_back(move);
		else
			legalNonJumps.push_back(move);
	}

	// There are legal moves, choose the best one following this heuristic:
	//
	//	1) Favor a legal tail-jump that will not result in a capture
	//	2) Favor a move that will not result in a capture
	//	3) Favor a jump
	//	4) Default to a non-jump
	//

	for each (const Move &jump in legalJumps)
	{
		vector<pair<IBoard,Move>> finalJumpBoards;
		rules.GenerateJumpTree(jump.iPos, finalJumpBoards);
		for each (pair<IBoard,Move> p in finalJumpBoards)
		{
			if (!rules.CanLosePieceIfMoveTaken(p.first, this, p.second))
			{
				MoveResult mr;
				output.Write(spBoard, jump.iPos, jump.dk, jump.mk, jump.mr);
				FAILED_ASSERT_RETURN(mrError, spBoard->PerformLegalMove(this, jump.iPos, jump.dk, jump.mk, &mr));
				FAILED_ASSERT_RETURN(mrError, mr == jump.mr);
				return mr;
			}
		}
	}

	// If the jumps all lead to positions that can be taken by opponent
	//	then find a move that doesn't get piece captured

	for each (const Move &nonJump in legalNonJumps)
	{
		if (!rules.CanLosePieceIfMoveTaken(spBoard, this, nonJump))
		{
			MoveResult mr;
			output.Write(spBoard, nonJump.iPos, nonJump.dk, nonJump.mk, nonJump.mr);
			FAILED_ASSERT_RETURN(mrError, spBoard->PerformLegalMove(this, nonJump.iPos, nonJump.dk, nonJump.mk, &mr));
			FAILED_ASSERT_RETURN(mrError, mr == nonJump.mr);
			return mr;
		}
	}

	// If a capture cannot be avoided, then take a random jump
	random_shuffle(legalJumps.begin(), legalJumps.end());
	for each (const Move &jump in legalJumps)
	{
		MoveResult mr;
		output.Write(spBoard, jump.iPos, jump.dk, jump.mk, jump.mr);
		FAILED_ASSERT_RETURN(mrError, spBoard->PerformLegalMove(this, jump.iPos, jump.dk, jump.mk, &mr));
		FAILED_ASSERT_RETURN(mrError, mr == jump.mr);
		return mr;
	}

	// No jumps, then just make a random regular move and lose a piece
	random_shuffle(legalNonJumps.begin(), legalNonJumps.end());
	for each (const Move &nonJump in legalNonJumps)
	{
		MoveResult mr;
		output.Write(spBoard, nonJump.iPos, nonJump.dk, nonJump.mk, nonJump.mr);
		FAILED_ASSERT_RETURN(mrError, spBoard->PerformLegalMove(this, nonJump.iPos, nonJump.dk, nonJump.mk, &mr));
		FAILED_ASSERT_RETURN(mrError, mr == nonJump.mr);
		return mr;
	}

	ASSERTMSG(false, "There should be have been at least one legal move.");
}
