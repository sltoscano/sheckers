// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "io.h"
#include "rules.h"
#include "player.h"

#include "easycomp.h"

ComputerEasy::ComputerEasy(wstring wstrName, PieceType pt, int iPieceCount) :
	Player(wstrName, pt, iPieceCount)
{
}

ComputerEasy::~ComputerEasy()
{
}


MoveResult ComputerEasy::MakeMove(IBoard *pBoard, MoveResult mrLastMove)
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
	else
	{
		// Pick a random legal move
		random_shuffle(moves.begin(), moves.end());
		Move &move = moves.front();
		MoveResult mr;
		output.Write(spBoard, move.iPos, move.dk, move.mk, move.mr);
		FAILED_ASSERT_RETURN(mrError, spBoard->PerformLegalMove(this, move.iPos, move.dk, move.mk, &mr));
		FAILED_ASSERT_RETURN(mrError, mr == move.mr);
		return mr;
	}
}
