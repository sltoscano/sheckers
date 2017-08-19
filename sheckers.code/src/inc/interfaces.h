// Copyright: Steven Toscano

#pragma once

#include <string>
#include <memory>
#include "exportdef.h"

using namespace std::tr1;

// Forward decls
class EXPORT_API IPiece;
class EXPORT_API IBoard;
class EXPORT_API IPlayerPieces;
class EXPORT_API IPlayer;
class EXPORT_API IGame;
class EXPORT_API IFeedback;

// Short-hand smart pointer decls
typedef shared_ptr<IPiece>			IPiecePtr;
typedef shared_ptr<IBoard>			IBoardPtr;
typedef shared_ptr<IPlayerPieces>	IPlayerPiecesPtr;
typedef shared_ptr<IPlayer>			IPlayerPtr;
typedef shared_ptr<IGame>			IGamePtr;
typedef shared_ptr<IFeedback>		IFeedbackPtr;

const static int EmptyPosition = -1;

enum EXPORT_API SquareColor
{
	scWhite,
	scBlack
};

enum EXPORT_API PieceType
{
	ptRed,
	ptBlack
};

enum EXPORT_API DirectionKind
{
	dkUp,
	dkDown
};

enum EXPORT_API MoveKind
{
	mkLeft,
	mkRight
};

enum EXPORT_API MoveResult
{
	mrLegalNonJump,
	mrLegalJump,
	mrIllegal,
	mrNone,
	mrError,
	mrForfeit
};

enum EXPORT_API GameState
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

enum EXPORT_API GameKind
{
	gkCheckers
};

enum EXPORT_API PlayerKind
{
	pkComputerHard,
	pkComputerEasy,
	pkHuman
};

class EXPORT_API IPiece
{
public:
	virtual PieceType		GetType() const = 0;
	virtual int				GetPosition() const = 0;
	virtual void			SetPosition(int iPosition) = 0;
	virtual DirectionKind	GetDirection() const = 0;
	virtual void			SetDirection(enum DirectionKind dk) = 0;
};

class EXPORT_API IBoard
{
public:
	virtual bool		Setup(IPlayerPtr spPlayer1, IPlayerPtr spPlayer2) = 0;
	virtual bool		PlacePiece(int iPosition, IPiecePtr spPiece) = 0;
	virtual IPiecePtr	RemovePiece(int iPosition) = 0;
	virtual IPiecePtr	GetPiece(int iPosition) const = 0;
	virtual IPlayerPtr	GetPlayer(PieceType pt) const = 0;
	virtual bool		IsOccupied(int iPosition) const = 0;
	virtual SquareColor	GetSquareColor(int iPosition) const = 0;
	virtual int			GetRowSize() const = 0;
	virtual int			GetColSize() const = 0;
	virtual int			GetBoardSize() const = 0;
	virtual bool		IsPositionAtEdge(int iPos) const = 0;
	virtual int			GetNewPositionIfMoved(int iPos, MoveKind mk, DirectionKind dk) const = 0;
	virtual bool		CanLosePiece(IPlayerPtr spPlayer, int iFromPos, int iToPos) const = 0;
	virtual MoveResult	TryMove(int iFromPos, MoveKind mk, DirectionKind dk) const = 0;
	virtual MoveResult	PerformLegalMove(IPlayerPtr spPlayer, int iFromPos, MoveKind mk, DirectionKind dkSource) = 0;
};

class EXPORT_API IPlayerPieces
{
public:
	virtual IPiecePtr	GetNext() = 0;
};

class EXPORT_API IPlayer
{
public:
	virtual bool				AddPiece(IPiecePtr spPiece) = 0;
	virtual bool				LosePiece(IPiecePtr pPiece) = 0;
	virtual bool				CapturePiece(IPiecePtr spPiece) = 0;
	virtual int					GetPieceOwnedCount() const = 0;
	virtual int					GetPieceCapturedCount() const = 0;
	virtual wstring				GetName() const = 0;
	virtual PieceType			GetType() const = 0;
	virtual int					GetInitialPieceCount() const = 0;
	virtual int					GetLastMovePosition() const = 0;
	virtual void				SetLastMovePosition(int iPos) = 0;
	virtual IPlayerPiecesPtr	GetPieces() = 0;
	virtual MoveResult			MakeMove(IBoardPtr spBoard, MoveResult mrLastMove) = 0;
};

class EXPORT_API IGame
{
public:
	virtual void		Start(IPlayerPtr spPlayer1, IPlayerPtr spPlayer2) = 0;
	virtual void		SetState(GameState gs) = 0;
	virtual bool		IsOver() const = 0;
	virtual GameState	GetState() const = 0;
	virtual MoveResult	DoTurn(MoveResult mrLast) = 0;
};

class IMPORT_API IFeedback
{
public:
	virtual void		SetPlayerName(wstring wstrName);
	virtual void		Read();
	virtual void		Clear();
	virtual int			ReadBoardRowSize();
	virtual int			ReadPieceCount(int iBoardRowSize);
	virtual PlayerKind	ReadPlayer(wstring wstrPlayer);
	virtual bool		ReadMove(IBoardPtr spBoard, PieceType pt, int *piPos, DirectionKind *pdk, MoveKind *pmk);
	virtual bool		ListLegalMoves();

	virtual void		Write(GameKind gk);
	virtual void		Write(IBoardPtr spBoard);
	virtual void		Write(IBoardPtr spBoard, int iPos, DirectionKind dk, MoveKind mk, MoveResult mr);
	virtual void		Write(GameState gs, wstring wstrPlayer1, wstring wstrPlayer2);
};
