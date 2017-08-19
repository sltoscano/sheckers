// Copyright: Steven Toscano

#pragma once

class EXPORT_API PieceFactory
{
public:
	static IPiece *Create(PieceType pt);
	// To be used by clients external to the DLL only
	// (internally memory is released through smart pointers)
	static void Destroy(IPiece *pPiece);

private:
	PieceFactory();
	~PieceFactory();
};

class EXPORT_API PlayerFactory
{
public:
	static IPlayer *Create(PlayerKind pk, wchar_t *wstrName, PieceType pt, int iPieceCount,
		IFeedback *pFeedback);
	// To be used by clients external to the DLL only
	// (internally memory is released through smart pointers)
	static void Destroy(IPlayer *pPlayer);

private:
	PlayerFactory();
	~PlayerFactory();
};

class EXPORT_API BoardFactory
{
public:
	static IBoard *Create(GameKind gk, int iRowSize, int iColSize);
	// To be used by clients external to the DLL only
	// (internally memory is released through smart pointers)
	static void Destroy(IBoard *pBoard);

private:
	BoardFactory();
	~BoardFactory();
};

class EXPORT_API GameFactory
{
public:
	static IGame *Create(GameKind gk, IBoard *pBoard);
	// To be used by clients external to the DLL only
	// (internally memory is released through smart pointers)
	static void Destroy(IGame *pGame);

private:
	GameFactory();
	~GameFactory();
};
