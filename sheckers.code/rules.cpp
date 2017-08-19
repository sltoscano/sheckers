// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "factories.h"
#include "rules.h"

Rules::Rules(IBoard *pBoard) :
	m_spBoard(pBoard)
{
}

Rules::~Rules()
{
}

bool Rules::GetLegalMoves(IPlayer *pPlayer, MoveResult mrLastMove, vector<Move> &moves) const
{
	IPlayerPtr spPlayer(pPlayer);
	FAILED_ASSERT_RETURN(false, spPlayer != NULL);

	if (mrLastMove == mrLegalJump)
	{
		// If the last legal move was a jump then only the
		//	same piece can move again - but it can move in
		//	any direction if it results in another jump

		int iPos = spPlayer->GetLastMovePosition();

		vector<DirectionKind> dks;
		dks.push_back(dkUp); dks.push_back(dkDown);
		vector<MoveKind> mks;
		mks.push_back(mkLeft); mks.push_back(mkRight);

		for each (DirectionKind dk in dks)
		{
			for each (MoveKind mk in mks)
			{
				MoveResult mr;
				FAILED_ASSERT_RETURN(false, m_spBoard->TryMove(iPos, mk, dk, &mr));
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

		IPlayerPiecesPtr spPieces;
		FAILED_ASSERT_RETURN(false, spPlayer->GetPieces(&spPieces));

		for (IPiecePtr spPiece; 
			 spPieces->GetNext(&spPiece); 
 			 spPiece.Release())
		{
			FAILED_ASSERT_RETURN(false, spPiece != NULL);

			int iPos;
			FAILED_ASSERT_RETURN(false, spPiece->GetPosition(&iPos));
			DirectionKind dk;
			FAILED_ASSERT_RETURN(false, spPiece->GetDirection(&dk));
			
			vector<MoveKind> mks;
			mks.push_back(mkLeft); mks.push_back(mkRight);

			for each (MoveKind mk in mks)
			{
				MoveResult mr;
				FAILED_ASSERT_RETURN(false, m_spBoard->TryMove(iPos, mk, dk, &mr));
				if (mr != mrIllegal)
				{
					Move move = {iPos, mk, dk, mr};
					moves.push_back(move);
				}
			}
		}
	}

	return true;
}

bool Rules::IsLegal(int iPos, DirectionKind dk, MoveKind mk, vector<Move> &moves) const
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

bool Rules::CanLosePieceIfMoveTaken(IBoard *pBoard, IPlayer *pPlayer, Move moveAttempt) const
{
	IBoardPtr spBoard(pBoard);
	FAILED_ASSERT_RETURN(false, spBoard != NULL);
	IPlayerPtr spPlayer(pPlayer);
	FAILED_ASSERT_RETURN(false, spPlayer != NULL);

	int iNewPos = spBoard->GetNewPositionIfMoved(moveAttempt.iPos, moveAttempt.mk, moveAttempt.dk);

	// If piece is at the edge it can't be jumped
	if (spBoard->IsPositionAtEdge(iNewPos))
		return false;

	// Make a temporary 3x3 board with the target piece at its new
	//	position after the move in the center.
	// . . .
	// . 0 .
	// . . .
	// Then add all the pieces from the original board which surround it
	// X . X
	// . 0 .
	// . . .
	// Next permute through all the opponents possible moves to see if
	//	a jump is possible
	// . . X
	// . . .
	// . . X
	// In the above example the opponent can jump the target piece so it
	//	can be lost.

	// Get the color of the current player
	PieceType ptThisPlayer;
	spPlayer->GetType(&ptThisPlayer);
	PieceType ptOpponent = (ptThisPlayer == ptBlack) ? ptRed : ptBlack;

	// Create the temp players and board
	IPlayerPtr spThisPlayer(PlayerFactory::Create(pkComputer, L"ThisPlayer", ptThisPlayer, 0));
	IPlayerPtr spOpponentPlayer(PlayerFactory::Create(pkComputer, L"Opponent", ptOpponent, 0));
	IBoardPtr spTempBoard(BoardFactory::Create(gkCheckers, 3, 3));

	// Put a new piece from this player in the middle of the temp board
	IPiecePtr spNewPiece(PieceFactory::Create(ptThisPlayer));
	spTempBoard->PlacePiece(4, spNewPiece);

	// Walk the original board and copy to temp board
	//	starting at the top left corner
	int iTempMoveAttempt;
	int iOrigPos = iNewPos - spBoard->GetRowSize() - 1;
	for (int iTempPos = 0; iTempPos < 9; iTempPos++)
	{
		// Save the originating move 
		if (iOrigPos == moveAttempt.iPos)
			iTempMoveAttempt = iTempPos;

		IPiecePtr spPiece;
		if (spBoard->GetPiece(iOrigPos, &spPiece))
		{
			PieceType pt;
			spPiece->GetType(&pt);
			spTempBoard->PlacePiece(iTempPos, PieceFactory::Create(pt));
		}

		// Move both counters at different rates since one is walking
		//	the original board and the other the temporary board
		if (iTempPos != 0 && iTempPos % 3 == 2)
			iOrigPos += spBoard->GetRowSize() - 2;
		else
			iOrigPos++;
	}

	// Take the original move attempt on the temp board
	MoveResult mr;
	spTempBoard->PerformLegalMove(spThisPlayer, iTempMoveAttempt, moveAttempt.dk, moveAttempt.mk, &mr);
	ASSERTMSG(mr != mrIllegal, "This method can only be called on legal moves");

	// Get all the legal moves of the opponent check if any jumps are possible
	Rules rules(spTempBoard);
	vector<Move> moves;
	rules.GetLegalMoves(spOpponentPlayer, mrNone, moves);
	for each (const Move &move in moves)
	{
		if (move.mr == mrLegalJump)
			return true;
	}
	return false;
}

bool Rules::GenerateJumpTree(int iFromPos, vector<pair<IBoard,Move>> finalJumpBoards) const
{
	vector<DirectionKind> dks;
	dks.push_back(dkUp); dks.push_back(dkDown);
	vector<MoveKind> mks;
	mks.push_back(mkLeft); mks.push_back(mkRight);

	for each (DirectionKind dk in dks)
	{
		for each (MoveKind mk in mks)
		{
			MoveResult mr;
			FAILED_ASSERT_RETURN(false, m_spBoard->TryMove(iFromPos, mk, dk, &mr));
			if (mr == mrLegalJump)
			{
				Move move = {iFromPos, mk, dk, mr};
				make_pair(move, spBoard);

				int iNewPos = m_spBoard->GetNewPositionIfMoved(iFromPos, mk, dk);
				
				jumpTargets.push_back(iNewPos);
			}
		}
	}
}

{
	// Don't recurse down a position if it has already been recursed
	if (find(visitedTargets.begin(), visitedTargets.end(), iFromPos) != visitedTargets.end())
		return false;

	vector<int> jumpTargets;
	vector<DirectionKind> dks;
	dks.push_back(dkUp); dks.push_back(dkDown);
	vector<MoveKind> mks;
	mks.push_back(mkLeft); mks.push_back(mkRight);

	for each (DirectionKind dk in dks)
	{
		for each (MoveKind mk in mks)
		{
			MoveResult mr;
			FAILED_ASSERT_RETURN(false, m_spBoard->TryMove(iFromPos, mk, dk, &mr));
			if (mr == mrLegalJump)
			{
				int iNewPos = m_spBoard->GetNewPositionIfMoved(iFromPos, mk, dk);
				jumpTargets.push_back(iNewPos);
			}
		}
	}

	if (jumpTargets.empty())
	{
		// Base-case, there are no more legal jumps, add the final
		//	resting position of this piece to the final jump target list
		finalJumpTargets.push_back(iFromPos);
		return true;
	}
	else
	{
		// Recurse, on all the legal jumps
		for each (int iPos in jumpTargets)
		{
			GenerateJumpTree(iPos, visitedTargets, finalJumpTargets);
		}
		visitedTargets.push_back(iFromPos);
		return true;
	}
}
