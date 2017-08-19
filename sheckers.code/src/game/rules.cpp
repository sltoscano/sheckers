// Copyright: Steven Toscano

#include "common.h"

#include "rules.h"

bool Rules::IsLegal(int iPos, DirectionKind dk, MoveKind mk, vector<Move> &moves)
{
	for each (const Move& move in moves)
	{
		if (move.iPos == iPos &&
			move.dk == dk &&
			move.mk == mk)
			return true;
	}
	return false;
}

bool Rules::GetLegalMoves(IBoard *pBoard, IPlayer *pPlayer, MoveResult mrLastMove, vector<Move> &moves)
{
	IBoardPtr spBoard(pBoard);
	FAILED_ASSERT_RETURN(false, spBoard != NULL);
	IPlayerPtr spPlayer(pPlayer);
	FAILED_ASSERT_RETURN(false, spPlayer != NULL);

	if (mrLastMove == mrLegalJump)
	{
		// If the last legal move was a jump then only the
		//	same piece can move again - but it can move in
		//	any direction if it results in another jump

		int iPos = spPlayer->GetLastMovePosition();
		FAILED_ASSERT_RETURN(false, iPos != EmptyPosition);

		vector<DirectionKind> dks;
		dks.push_back(dkUp); dks.push_back(dkDown);
		vector<MoveKind> mks;
		mks.push_back(mkLeft); mks.push_back(mkRight);

		for each (DirectionKind dk in dks)
		{
			for each (MoveKind mk in mks)
			{
				MoveResult mr = spBoard->TryMove(iPos, mk, dk);
				if (mr == mrLegalJump)
				{
					Move move = {iPos, mk, dk, mr};
					moves.push_back(move);
				}
			}
		}
	}
	else
	{
		// Otherwise walk all the player's pieces and add all
		//	legal moves found

		IPlayerPiecesPtr spPieces = spPlayer->GetPieces();

		IPiecePtr spPiece = spPieces->GetNext();
		
		while (spPiece != NULL)
		{
			int iPos = spPiece->GetPosition();
			DirectionKind dk = spPiece->GetDirection();
			
			vector<MoveKind> mks;
			mks.push_back(mkLeft); mks.push_back(mkRight);

			for each (MoveKind mk in mks)
			{
				MoveResult mr = spBoard->TryMove(iPos, mk, dk);
				if (mr != mrIllegal)
				{
					Move move = {iPos, mk, dk, mr};
					moves.push_back(move);
				}
			}

			spPiece = spPieces->GetNext();
		}
	}

	return true;
}

bool Rules::CanLosePieceIfMoveTaken(IBoard *pBoard, IPlayer *pPlayer, const Move &moveAttempt)
{
	IBoardPtr spBoard(pBoard);
	FAILED_ASSERT_RETURN(false, spBoard != NULL);
	IPlayerPtr spPlayer(pPlayer);
	FAILED_ASSERT_RETURN(false, spPlayer != NULL);

	int iNewPos = spBoard->GetNewPositionIfMoved(moveAttempt.iPos, moveAttempt.mk, moveAttempt.dk);
	return spBoard->CanLosePiece(spPlayer, moveAttempt.iPos, iNewPos);
}
