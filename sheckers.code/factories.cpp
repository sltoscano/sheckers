// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "factories.h"
#include "piece.h"
#include "player.h"
#include "computer.h"
#include "human.h"
#include "board.h"
#include "game.h"

IPiece *PieceFactory::Create(PieceType pt)
{
	return new Piece(pt);
}

IPlayer *PlayerFactory::Create(PlayerKind pk, wstring wstrName, PieceType pt, int iPieceCount)
{
	IPlayer *pPlayer = NULL;

	switch (pk)
	{
	case pkComputer:
		pPlayer = new Computer(wstrName, pt, iPieceCount);
		break;
	case pkHuman:
		pPlayer = new Human(wstrName, pt, iPieceCount);
		break;
	default:
		ASSERTMSG(false, "PlayerKind not supported");
		break;
	}

	return pPlayer;
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
