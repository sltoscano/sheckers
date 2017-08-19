// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "rules.h"
#include "player.h"

#include "human.h"

Human::Human(wchar_t *wstrName, PieceType pt, int iPieceCount,
			 IFeedback *pFeedback) :
	Player(wstrName, pt, iPieceCount, pFeedback), m_spFeedback(pFeedback)
{
	//m_spFeedback->SetPlayerName(m_wstrName.c_str());
}

Human::~Human()
{
}

MoveResult Human::MakeMove(IBoard *pBoard, MoveResult mrLastMove)
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
		int iPos;
		MoveKind mk;
		DirectionKind dk;
		while (m_spFeedback->ReadMove(spBoard, m_pt, &iPos, &dk, &mk))
		{
			// Check the validity of the user entered move
			if (Rules::IsLegal(iPos, dk, mk, moves))
			{
				MoveResult mr;
				FAILED_ASSERT_RETURN(mrError, spBoard->PerformLegalMove(this, iPos, dk, mk, &mr));
				return mr;
			}
			else
			{
				// Offer to list the set of legal moves
				if (m_spFeedback->ListLegalMoves())
				{
					m_spFeedback->Clear();
					m_spFeedback->Write(spBoard);
					for each (const Move &move in moves)
						m_spFeedback->Write(spBoard, move.iPos, move.dk, move.mk, move.mr);
				}
			}
		}

		return mrForfeit;
	}
}
