// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "factories.h"

#include "rules.h"
#include "io.h"

Rules::Rules()
{
}

Rules::~Rules()
{
}

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
				MoveResult mr;
				FAILED_ASSERT_RETURN(false, spBoard->TryMove(iPos, mk, dk, &mr));
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
				FAILED_ASSERT_RETURN(false, spBoard->TryMove(iPos, mk, dk, &mr));
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

bool Rules::CanLosePieceIfMoveTaken(IBoard *pBoard, IPlayer *pPlayer, const Move &moveAttempt)
{
	IBoardPtr spBoard(pBoard);
	FAILED_ASSERT_RETURN(false, spBoard != NULL);
	IPlayerPtr spPlayer(pPlayer);
	FAILED_ASSERT_RETURN(false, spPlayer != NULL);

	int iNewPos = spBoard->GetNewPositionIfMoved(moveAttempt.iPos, moveAttempt.mk, moveAttempt.dk);
	return spBoard->CanLosePiece(spPlayer, moveAttempt.iPos, iNewPos);
}


JumpTree::JumpTree(Move move) :
	m_seedMove(move)
{
}

JumpTree::~JumpTree()
{
}

bool JumpTree::Generate(IBoard *pBoard, IPlayer *pPlayer, Move move, int iDepth)
{
	// Only jumps should be attempted
	FAILED_ASSERT_RETURN(false, move.mr == mrLegalJump);

	// Don't recurse on a node that we've already visited
	if (find(m_visitedJumps.begin(), m_visitedJumps.end(), move.iPos) != m_visitedJumps.end())
		return true;

	IBoardPtr spBoardOriginal(pBoard);
	FAILED_ASSERT_RETURN(false, spBoardOriginal != NULL);
	IPlayerPtr spPlayerOriginal(pPlayer);
	FAILED_ASSERT_RETURN(false, spPlayerOriginal != NULL);
	PieceType ptOriginal;
	FAILED_ASSERT_RETURN(false, spPlayerOriginal->GetType(&ptOriginal));
	int iRowSize = spBoardOriginal->GetRowSize();

	// Make a copy of the current board state and use it
	//	to recurse on legal jumps
	IBoardPtr spBoardCopy(BoardFactory::Create(gkCheckers, iRowSize, iRowSize));
	FAILED_ASSERT_RETURN(false, spBoardCopy != NULL);
	IPlayerPtr spPlayer1(PlayerFactory::Create(pkComputerHard, L"Player1", ptRed, 0));
	FAILED_ASSERT_RETURN(false, spPlayer1 != NULL);
	IPlayerPtr spPlayer2(PlayerFactory::Create(pkComputerHard, L"Player2", ptBlack, 0));
	FAILED_ASSERT_RETURN(false, spPlayer2 != NULL);

	IPlayerPtr spCurrentPlayer = (ptOriginal == ptRed) ? spPlayer1 : spPlayer2;
	FAILED_ASSERT_RETURN(false, spCurrentPlayer->SetLastMovePosition(spPlayerOriginal->GetLastMovePosition()));
	FAILED_ASSERT_RETURN(false, spBoardCopy->Setup(spPlayer1, spPlayer2));

	// Walk the new board and add pieces that match the
	//	board that was passed in
	for (int i=0; i < spBoardCopy->GetBoardSize(); i++)
	{
		IPiecePtr spPiece;
		if (spBoardOriginal->GetPiece(i, &spPiece))
		{
			FAILED_ASSERT_RETURN(false, spPiece != NULL);
			PieceType pt;
			FAILED_ASSERT_RETURN(false, spPiece->GetType(&pt));
			IPiecePtr spPieceCopy(PieceFactory::Create(pt));
			FAILED_ASSERT_RETURN(false, spPieceCopy != NULL);
			if (pt == ptRed)
				FAILED_ASSERT_RETURN(false, spPlayer1->AddPiece(spPieceCopy));
			else
				FAILED_ASSERT_RETURN(false, spPlayer2->AddPiece(spPieceCopy));
			FAILED_ASSERT_RETURN(false, spBoardCopy->PlacePiece(i, spPieceCopy));
		}
	}

	// First try to commit the jump that was passed to the tree generator using
	//	the copy of the board that was just created
	MoveResult mrTry;
	FAILED_ASSERT_RETURN(false, spBoardCopy->TryMove(move.iPos, move.mk, move.dk, &mrTry));
	FAILED_ASSERT_RETURN(false, mrTry == mrLegalJump);
	MoveResult mrCommit;
	UserOutput output;
	string s(iDepth, ' ');
//	cout << s.c_str() << "taking the jump" << endl;
//	cout << s.c_str();
	output.Write(spBoardCopy, move.iPos, move.dk, move.mk, mrTry);
	FAILED_ASSERT_RETURN(false, spBoardCopy->PerformLegalMove(spCurrentPlayer, move.iPos, move.dk, move.mk, &mrCommit));
	FAILED_ASSERT_RETURN(false, mrCommit == mrTry);

	// Now generate any additional jumps that are possible
	bool fFoundAdditionalJumps = false;

	int iPos = spCurrentPlayer->GetLastMovePosition();
	FAILED_ASSERT_RETURN(false, iPos != EmptyPosition);

	vector<DirectionKind> dks;
	dks.push_back(dkUp); dks.push_back(dkDown);
	vector<MoveKind> mks;
	mks.push_back(mkLeft); mks.push_back(mkRight);

//	cout << s.c_str() << "checking for more jumps" << endl;

	for each (DirectionKind dk in dks)
	{
		for each (MoveKind mk in mks)
		{
			MoveResult mr;
			FAILED_ASSERT_RETURN(false, spBoardCopy->TryMove(iPos, mk, dk, &mr));
			if (mr == mrLegalJump)
			{
				// Recurse and find more legal jumps from this point
				Move jump = {iPos, mk, dk, mr};
				if (!Generate(spBoardCopy, spCurrentPlayer, jump, iDepth+1))
					return false;
				m_visitedJumps.push_back(jump.iPos);
				fFoundAdditionalJumps = true;
			}
		}
	}

	if (!fFoundAdditionalJumps)
	{
//		cout << s.c_str() << "no more jumps!" << endl;
		// Base case, there are no more jumps so record this as a leaf
		LeafNode leaf = { m_seedMove, spBoardCopy, iPos, iDepth };
		m_leafNodes.push_back(leaf);
	}

	return true;
}
