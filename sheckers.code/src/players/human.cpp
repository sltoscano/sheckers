// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "io.h"
#include "rules.h"
#include "player.h"

#include "human.h"

Human::Human(wstring wstrName, PieceType pt, int iPieceCount) :
	Player(wstrName, pt, iPieceCount)
{
}

Human::~Human()
{
}

MoveResult Human::MakeMove(IBoard *pBoard, MoveResult mrLastMove)
{
	IBoardPtr spBoard(pBoard);
	FAILED_ASSERT_RETURN(mrError, spBoard != NULL);

	Rules rules;
	vector<Move> moves;
	FAILED_ASSERT_RETURN(mrError, rules.GetLegalMoves(spBoard, this, mrLastMove, moves));

	if (moves.empty())
	{
		UserOutput output(m_wstrName);
		output.Write(NULL, EmptyPosition, dkUp, mkLeft, mrNone);
		return mrNone;
	}
	else
	{
		int iPos;
		MoveKind mk;
		DirectionKind dk;
		UserInput input(m_wstrName);
		while (input.ReadMove(spBoard, m_pt, &iPos, &dk, &mk))
		{
			// Check the validity of the user entered move
			if (rules.IsLegal(iPos, dk, mk, moves))
			{
				MoveResult mr;
				FAILED_ASSERT_RETURN(mrError, spBoard->PerformLegalMove(this, iPos, dk, mk, &mr));
				return mr;
			}
			else
			{
				// Offer to list the set of legal moves
				if (input.ListLegalMoves())
				{
					UserOutput output;
					input.Clear();
					output.Write(spBoard);
					for each (const Move &move in moves)
						output.Write(spBoard, move.iPos, move.dk, move.mk, move.mr);
				}
			}
		}

		return mrForfeit;
	}
}
