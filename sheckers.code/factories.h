// Copyright: Steven Toscano

#pragma once

class PieceFactory
{
public:
	static IPiece *Create(PieceType pt);

private:
	PieceFactory();
	~PieceFactory();
};

class PlayerFactory
{
public:
	static IPlayer *Create(PlayerKind pk, wstring wstrName, PieceType pt, int iPieceCount);

private:
	PlayerFactory();
	~PlayerFactory();
};

class BoardFactory
{
public:
	static IBoard *Create(GameKind gk, int iRowSize, int iColSize);

private:
	BoardFactory();
	~BoardFactory();
};

class GameFactory
{
public:
	static IGame *Create(GameKind gk, IBoard *pBoard);

private:
	GameFactory();
	~GameFactory();
};
