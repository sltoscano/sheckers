// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "io.h"
#include "rules.h"
#include "player.h"

#include "hardcomp.h"

ComputerHard::ComputerHard(wstring wstrName, PieceType pt, int iPieceCount) :
	Player(wstrName, pt, iPieceCount)
{
}

ComputerHard::~ComputerHard()
{
}

MoveResult ComputerHard::MakeMove(IBoard *pBoard, MoveResult mrLastMove)
{
	IBoardPtr spBoard(pBoard);
	FAILED_ASSERT_RETURN(mrError, spBoard != NULL);

	Rules rules;
	vector<Move> moves;
	FAILED_ASSERT_RETURN(mrError, rules.GetLegalMoves(spBoard, this, mrLastMove, moves));

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

	// Generate the jump trees for each legal jump
//	cout << "GENERATE LEAVES" << endl;
	vector<LeafNode> allLeafNodes;
	for each (const Move &jump in legalJumps)
	{
		JumpTree tree(jump);
		FAILED_ASSERT_RETURN(mrError, tree.Generate(spBoard, this, jump, 0));
		allLeafNodes.insert(allLeafNodes.end(), tree.m_leafNodes.begin(), tree.m_leafNodes.end());
	}

	// If this jump can take more than one piece then favor it
//	cout << "CHECK IF MORE THAN ONE PIECE JUMP" << endl;
	if (!allLeafNodes.empty())
	{
		sort(allLeafNodes.begin(), allLeafNodes.end(), LeafGreater);
		if (allLeafNodes.front().iJumpDepth > 0)
		{
			Move &move = allLeafNodes.front().seedMove;
			MoveResult mr;
			output.Write(spBoard, move.iPos, move.dk, move.mk, move.mr);
			FAILED_ASSERT_RETURN(mrError, spBoard->PerformLegalMove(this, move.iPos, move.dk, move.mk, &mr));
			FAILED_ASSERT_RETURN(mrError, mr == move.mr);
			return mr;
		}
	}

	// Otherwise favor a jump that will not eventually be captured
//	cout << "CHECK JUMP CANNOT BE CAPTURED" << endl;
	if (!allLeafNodes.empty())
	{
		for each (const LeafNode &node in allLeafNodes)
		{
			if (!node.spBoard->CanLosePiece(this, EmptyPosition, node.iPos))
			{
				const Move & move = node.seedMove;
				MoveResult mr;
				output.Write(spBoard, move.iPos, move.dk, move.mk, move.mr);
				FAILED_ASSERT_RETURN(mrError, spBoard->PerformLegalMove(this, move.iPos, move.dk, move.mk, &mr));
				FAILED_ASSERT_RETURN(mrError, mr == move.mr);
				return mr;
			}
		}
	}

	// If the jumps all lead to positions that can be taken by opponent
	//	then find a move that doesn't get piece captured
//	cout << "CHECK MOVE CANNOT BE CAPTURED" << endl;
	random_shuffle(legalNonJumps.begin(), legalNonJumps.end());
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
//	cout << "TAKE RANDOM JUMP" << endl;
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
//	cout << "TAKE RANDOM NONJUMP" << endl;
	for each (const Move &nonJump in legalNonJumps)
	{
		MoveResult mr;
		output.Write(spBoard, nonJump.iPos, nonJump.dk, nonJump.mk, nonJump.mr);
		FAILED_ASSERT_RETURN(mrError, spBoard->PerformLegalMove(this, nonJump.iPos, nonJump.dk, nonJump.mk, &mr));
		FAILED_ASSERT_RETURN(mrError, mr == nonJump.mr);
		return mr;
	}

	ASSERTMSG(false, "There should have been at least one legal move.");
	return mrError;
}
