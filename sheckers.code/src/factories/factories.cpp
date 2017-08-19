// Copyright: Steven Toscano

#include "common.h"

#include "piece.h"
#include "board.h"
#include "game.h"
#include "player.h"
#include "hard.h"
#include "easy.h"
#include "human.h"

#include "factories.h"

IPiece *PieceFactory::Create(PieceType pt)
{
	return new Piece(pt);
}

void PieceFactory::Destroy(IPiece *pPiece)
{
	delete pPiece;
}

IPlayer *PlayerFactory::Create(PlayerKind pk, wchar_t *wstrName, PieceType pt, int iPieceCount,
							   IFeedback *pFeedback)
{
	IPlayer *pPlayer = NULL;

	switch (pk)
	{
	case pkComputerEasy:
		pPlayer = new ComputerEasy(wstrName, pt, iPieceCount, pFeedback);
		break;
	case pkComputerHard:
		pPlayer = new ComputerHard(wstrName, pt, iPieceCount, pFeedback);
		break;
	case pkHuman:
		pPlayer = new Human(wstrName, pt, iPieceCount, pFeedback);
		break;
	default:
		ASSERTMSG(false, "PlayerKind not supported");
		break;
	}

	return pPlayer;
}

void PlayerFactory::Destroy(IPlayer *pPlayer)
{
	delete pPlayer;
}

IBoard *BoardFactory::Create(GameKind gk, int iRowSize, int iColSize)
{
	IBoard *pBoard = NULL;

	switch (gk)
	{
	case gkCheckers:
		pBoard = new Board(iRowSize);
		break;
	default:
		ASSERTMSG(false, "GameKind not supported");
		break;
	}

	return pBoard;
}

void BoardFactory::Destroy(IBoard *pBoard)
{
	delete pBoard;
}

IGame *GameFactory::Create(GameKind gk, IBoard *pBoard)
{
	IGame *pGame = NULL;

	switch (gk)
	{
	case gkCheckers:
		pGame = new Game(pBoard);
		break;
	default:
		ASSERTMSG(false, "GameKind not supported");
		break;
	}

	return pGame;
}

void GameFactory::Destroy(IGame *pGame)
{
	delete pGame;
}
