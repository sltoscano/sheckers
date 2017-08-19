// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "rules.h"
#include "player.h"

#include "easy.h"

ComputerEasy::ComputerEasy(wchar_t *wstrName, PieceType pt, int iPieceCount,
						   IFeedback *pFeedback) :
	Player(wstrName, pt, iPieceCount, pFeedback), m_spFeedback(pFeedback)
{
	//m_spFeedback->SetPlayerName(m_wstrName.c_str());
}

ComputerEasy::~ComputerEasy()
{
}


MoveResult ComputerEasy::MakeMove(IBoard *pBoard, MoveResult mrLastMove)
{
	IBoardPtr spBoard(pBoard);
	FAILED_ASSERT_RETURN(mrError, spBoard != NULL);

	vector<Move> moves;
	FAILED_ASSERT_RETURN(mrError, Rules::GetLegalMoves(spBoard, this, mrLastMove, moves));

	if (moves.empty())
	{
		m_spFeedback->Write(NULL, EmptyPosition, dkUp, mkLeft, mrNone);
		return mrNone;
	}
	else
	{
		// Pick a random legal move
		random_shuffle(moves.begin(), moves.end());
		Move &move = moves.front();
		MoveResult mr;
		m_spFeedback->Write(spBoard, move.iPos, move.dk, move.mk, move.mr);
		FAILED_ASSERT_RETURN(mrError, spBoard->PerformLegalMove(this, move.iPos, move.dk, move.mk, &mr));
		FAILED_ASSERT_RETURN(mrError, mr == move.mr);
		return mr;
	}
}
