// Copyright: Steven Toscano

#pragma once

struct Square
{
	bool fOccupied;
	IPiecePtr spPiece;
	SquareColor scColor;
};

class Board :
	public IBoard,
	public CAutoRef
{
public:
	Board(int iRowSize);
	~Board();

	// IReferenceCounted methods
	long AddRef();
	long Release();

	// IBoard methods
	bool Setup(IPlayer *pPlayer1, IPlayer *pPlayer2);
	bool PlacePiece(int iPosition, IPiece *pPiece);
	bool RemovePiece(int iPosition, IPiece **ppPiece);
	bool GetPiece(int iPosition, IPiece **ppPiece) const;
	bool IsOccupied(int iPosition) const;
	bool GetSquareColor(int iPosition, SquareColor *psc) const;
	int  GetColSize() const;
	int  GetRowSize() const;
	int  GetBoardSize() const;
	bool IsPositionAtEdge(int iPos) const;
	int  GetNewPositionIfMoved(int iPos, MoveKind mk, DirectionKind dk) const;
	bool TryMove(int iPos, MoveKind mk, DirectionKind dk, MoveResult *pmr) const;
	bool PerformLegalMove(IPlayer *pPlayer, int iSourcePos, DirectionKind dkSource, MoveKind mk, MoveResult *pmr);

private:
	// Non interface methods
	bool AddPieces(IPlayer *pPlayer);

	class MovementPrimitives
	{
		friend class Board;
		MovementPrimitives(const IBoard *pBoard);
		bool IsMoveGoingOutsideBoard(int iPos, DirectionKind dk, MoveKind mk) const;
		int  GetNewPositionIfMoved(int iPos, DirectionKind dk, MoveKind mk) const;
		bool IsJumpPossible(int iPos, DirectionKind dk, MoveKind mk) const;
		IBoardPtr m_spBoard;
	};

private:
	vector<Square> m_board;
	IPlayerPtr m_spPlayer1;
	IPlayerPtr m_spPlayer2;
	int m_iRowSize;

private:
	// Disallow copy and assignment
	Board(const Board &);
	Board& operator=(const Board &);
};
