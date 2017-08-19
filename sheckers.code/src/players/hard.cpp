// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "rules.h"
#include "factories.h"
#include "player.h"

#include "hard.h"
#include <map>

using namespace std;

ComputerHard::ComputerHard(wchar_t *wstrName, PieceType pt, int iPieceCount,
						   IFeedback *pFeedback) :
	Player(wstrName, pt, iPieceCount, pFeedback), m_spFeedback(pFeedback)
{
	//m_spFeedback->SetPlayerName(m_wstrName.c_str());
}

ComputerHard::~ComputerHard()
{
}

MoveResult ComputerHard::MakeMove(IBoard *pBoard, MoveResult mrLastMove)
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

	int iBreadth = 0;
	map<int, Move> bestMoves;
	for each (const Move &move in moves)
	{
		++iBreadth;
		GameTree tree(spBoard, m_pt, 2);
		FAILED_ASSERT_RETURN(mrError, tree.Generate(0, iBreadth, spBoard, this, mrLastMove, move.iPos, move.dk, move.mk, 0));
		int iCount;
		FAILED_ASSERT_RETURN(mrError, !tree.m_capturedPieceCounts.empty());
		sort(tree.m_capturedPieceCounts.begin(), tree.m_capturedPieceCounts.end());
		iCount = tree.m_capturedPieceCounts.back();
		if (bestMoves.find(iCount) == bestMoves.end())
			bestMoves.insert(make_pair(iCount, move));
	}
	
	map<int, Move>::iterator iter = bestMoves.end();
	--iter;
	Move &move = iter->second;

	MoveResult mr;
	m_spFeedback->Write(spBoard, move.iPos, move.dk, move.mk, move.mr);
	FAILED_ASSERT_RETURN(mrError, spBoard->PerformLegalMove(this, move.iPos, move.dk, move.mk, &mr));
	FAILED_ASSERT_RETURN(mrError, mr == move.mr);
	return mr;
}

GameTree::GameTree(IBoard *pBoard, PieceType pt, int iMaxDepth) :
	m_spBoardSeed(pBoard),
	m_ptSeed(pt), 
	m_iMaxDepth(iMaxDepth)
{
}

GameTree::~GameTree()
{
}

bool GameTree::Generate(int iTop, int iBreadth, IBoard *pBoard, IPlayer *pPlayer, MoveResult mrLastMove, 
						int iPos, DirectionKind dk, MoveKind mk, int iDepth)
{
	IBoardPtr spBoardOriginal(pBoard);
	FAILED_ASSERT_RETURN(false, spBoardOriginal != NULL);
	IPlayerPtr spPlayerOriginalCurrent(pPlayer);
	FAILED_ASSERT_RETURN(false, spPlayerOriginalCurrent != NULL);
	PieceType ptCurrent;
	FAILED_ASSERT_RETURN(false, spPlayerOriginalCurrent->GetType(&ptCurrent));
	PieceType ptOpponent = (ptCurrent == ptRed) ? ptBlack : ptRed;
	IPlayerPtr spPlayerOriginalOpponent;
	FAILED_ASSERT_RETURN(false, spBoardOriginal->GetPlayer(ptOpponent, &spPlayerOriginalOpponent));
	int iRowSize = spBoardOriginal->GetRowSize();

	// Make a copy of the current board state as a child node
	IBoardPtr spBoardCopy(BoardFactory::Create(gkCheckers, iRowSize, iRowSize));
	FAILED_ASSERT_RETURN(false, spBoardCopy != NULL);
	IPlayerPtr spPlayer1(PlayerFactory::Create(pkComputerHard, L"Player1", ptRed, 0, NULL));
	FAILED_ASSERT_RETURN(false, spPlayer1 != NULL);
	IPlayerPtr spPlayer2(PlayerFactory::Create(pkComputerHard, L"Player2", ptBlack, 0, NULL));
	FAILED_ASSERT_RETURN(false, spPlayer2 != NULL);

	// Setup an empty board and initialize the players last move positions
	IPlayerPtr spCurrentPlayer = (ptCurrent == ptRed) ? spPlayer1 : spPlayer2;
	IPlayerPtr spOpponentPlayer = (ptOpponent == ptBlack) ? spPlayer2 : spPlayer1;
	FAILED_ASSERT_RETURN(false, spCurrentPlayer->SetLastMovePosition(spPlayerOriginalCurrent->GetLastMovePosition()));
	FAILED_ASSERT_RETURN(false, spOpponentPlayer->SetLastMovePosition(spPlayerOriginalOpponent->GetLastMovePosition()));
	FAILED_ASSERT_RETURN(false, spBoardCopy->Setup(spPlayer1, spPlayer2));

	// Walk the new board and add pieces to match the
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

	// Now add each player's captured pieces to get the game to the right state
	for (int i=0; i < spPlayerOriginalCurrent->GetPieceCapturedCount(); i++)
	{
		IPiecePtr spPiece(PieceFactory::Create(ptOpponent));
		FAILED_ASSERT_RETURN(false, spCurrentPlayer->CapturePiece(spPiece));
	}
	for (int i=0; i < spPlayerOriginalOpponent->GetPieceCapturedCount(); i++)
	{
		IPiecePtr spPiece(PieceFactory::Create(ptCurrent));
		FAILED_ASSERT_RETURN(false, spOpponentPlayer->CapturePiece(spPiece));
	}

	MoveResult mrTry;
	FAILED_ASSERT_RETURN(false, spBoardCopy->TryMove(iPos, mk, dk, &mrTry));
	FAILED_ASSERT_RETURN(false, mrTry != mrIllegal);
//	UserOutput output;
//	UserInput input;
//	string s(iDepth, ' ');
//	cout << s.c_str() << "taking the move" << endl;
//	cout << s.c_str();
//	output.Write(spBoardCopy, iPos, dk, mk, mrTry);
	FAILED_ASSERT_RETURN(false, spBoardCopy->PerformLegalMove(spCurrentPlayer, iPos, dk, mk, &mrLastMove));
	FAILED_ASSERT_RETURN(false, mrLastMove == mrTry);
/*
	input.Clear();
	cout << "depth=" << iDepth << " (" << iBreadth << ")";
	cout << " (" << iTop << ")" << endl;
	output.Write(spBoardCopy);
		PieceType ptCurr;
		spCurrentPlayer->GetType(&ptCurr);
		int iCap = (m_ptSeed == ptCurr) ? 
			spCurrentPlayer->GetPieceCapturedCount() : 
			spOpponentPlayer->GetPieceCapturedCount();
	cout << "seed captured=" << iCap << endl;
	output.Write(m_spBoardSeed);
	input.Read();
*/

	// Generate all legal moves from this point
	vector<Move> moves;
	FAILED_ASSERT_RETURN(false, Rules::GetLegalMoves(spBoardCopy, spCurrentPlayer, mrLastMove, moves));

	// If there are no legal moves, try the next player
	if (moves.empty())
	{
		IPlayerPtr tmp = spCurrentPlayer;
		spCurrentPlayer = spOpponentPlayer;
		spOpponentPlayer = tmp;
		mrLastMove = mrNone;
		FAILED_ASSERT_RETURN(false, Rules::GetLegalMoves(spBoardCopy, spCurrentPlayer, mrLastMove, moves));
	}

	// If max depth has been reached, or niether player can move
	if (iDepth > m_iMaxDepth || moves.empty())
	{
		PieceType ptCurrent;
		spCurrentPlayer->GetType(&ptCurrent);
		int iWeight = (m_ptSeed == ptCurrent) ? 
			spCurrentPlayer->GetPieceCapturedCount() : 
			spOpponentPlayer->GetPieceCapturedCount();
		m_capturedPieceCounts.push_back(iWeight);
		return true;
	}

	// Otherwise there are still legal moves, recurse on each
	int iTop = 0;
	for each (const Move &move in moves)
	{
		++iTop;
		if (!Generate(iTop, iBreadth, spBoardCopy, spCurrentPlayer, mrLastMove, move.iPos, move.dk, move.mk, iDepth+1))
			return false;
//		cout << endl;
	}

	return true;
}
