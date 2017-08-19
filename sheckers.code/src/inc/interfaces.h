// Copyright: Steven Toscano

#pragma once

// Forward decls
class IPlayer;

const static int EmptyPosition = -1;

enum SquareColor
{
	scRed,
	scBlack
};

enum PieceType
{
	ptRed,
	ptBlack
};

enum DirectionKind
{
	dkUp,
	dkDown
};

enum MoveKind
{
	mkLeft,
	mkRight
};

enum MoveResult
{
	mrLegalNonJump,
	mrLegalJump,
	mrIllegal,
	mrNone,
	mrError,
	mrForfeit
};

enum GameState
{
	gsRunning,
	gsStarted,
	gsNotStarted,
	gsPlayer1Wins,
	gsPlayer2Wins,
	gsStalemate,
	gsForfeit,
	gsError
};

enum GameKind
{
	gkCheckers
};

enum PlayerKind
{
	pkComputerHard,
	pkComputerEasy,
	pkHuman
};

class IPiece : public IReferenceCounted
{
public:
	virtual bool GetType(enum PieceType *ppt) const = 0;
	virtual bool GetPosition(int *piPosition) const = 0;
	virtual bool SetPosition(int iPosition) = 0;
	virtual bool GetDirection(enum DirectionKind *pdk) const = 0;
	virtual bool SetDirection(enum DirectionKind dk) = 0;
};

typedef CSharedPtr<IPiece> IPiecePtr;

class IBoard : public IReferenceCounted
{
public:
	virtual bool Setup(IPlayer *pPlayer1, IPlayer *pPlayer2) = 0;
	virtual bool PlacePiece(int iPosition, IPiece *pPiece) = 0;
	virtual bool RemovePiece(int iPosition, IPiece **ppPiece) = 0;
	virtual bool GetPiece(int iPosition, IPiece **ppPiece) const = 0;
	virtual bool IsOccupied(int iPosition) const = 0;
	virtual bool GetSquareColor(int iPosition, SquareColor *psc) const = 0;
	virtual int  GetRowSize() const = 0;
	virtual int  GetColSize() const = 0;
	virtual int  GetBoardSize() const = 0;
	virtual bool IsPositionAtEdge(int iPos) const = 0;
	virtual int  GetNewPositionIfMoved(int iPos, MoveKind mk, DirectionKind dk) const = 0;
	virtual bool CanLosePiece(IPlayer *pPlayer, int iFromPos, int iToPos) const = 0;
	virtual bool TryMove(int iPos, MoveKind mk, DirectionKind dk, MoveResult *pmr) const = 0;
	virtual bool PerformLegalMove(IPlayer *pPlayer, int iSourcePos, DirectionKind dkSource, MoveKind mk, MoveResult *pmr) = 0;
};

typedef CSharedPtr<IBoard> IBoardPtr;

class IPlayerPieces : public IReferenceCounted
{
public:
	virtual bool GetNext(IPiece **ppPiece) = 0;
};

typedef CSharedPtr<IPlayerPieces> IPlayerPiecesPtr;

class IPlayer : public IReferenceCounted
{
public:
	virtual bool AddPiece(IPiece *pPiece) = 0;
	virtual bool LosePiece(IPiece *pPiece) = 0;
	virtual bool CapturePiece(IPiece *pPiece) = 0;
	virtual int  GetPieceOwnedCount() const = 0;
	virtual int  GetPieceCapturedCount() const = 0;
	virtual bool GetName(wstring *pwstrName) const = 0;
	virtual bool GetType(PieceType *ppt) const = 0;
	virtual int  GetInitialPieceCount() const = 0;
	virtual int  GetLastMovePosition() const = 0;
	virtual bool SetLastMovePosition(int iPos) = 0;
	virtual bool GetPieces(IPlayerPieces **ppPieces) = 0;
	virtual MoveResult MakeMove(IBoard *pBoard, MoveResult mrLastMove) = 0;
};

typedef CSharedPtr<IPlayer> IPlayerPtr;

class IGame : public IReferenceCounted
{
public:
	virtual void Start(IPlayer *pPlayer1, IPlayer *pPlayer2) = 0;
	virtual void SetState(GameState gs) = 0;
	virtual bool IsOver() const = 0;
	virtual GameState GetState() const = 0;
	virtual MoveResult DoTurn(MoveResult mrLast) = 0;
};

typedef CSharedPtr<IGame> IGamePtr;
