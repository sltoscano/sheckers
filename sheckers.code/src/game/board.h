// Copyright: Steven Toscano

#pragma once

struct Square
{
	bool fOccupied;
	IPiecePtr spPiece;
	SquareColor scColor;
};

class Board :
	public IBoard
{
public:
	Board(int iRowSize);
	~Board();

	// IBoard methods
	bool Setup(IPlayerPtr spPlayer1, IPlayerPtr spPlayer2);
	bool PlacePiece(int iPosition, IPiecePtr spPiece);
	IPiecePtr RemovePiece(int iPosition);
	IPiecePtr GetPiece(int iPosition) const;
	bool IsOccupied(int iPosition) const;
	SquareColor GetSquareColor(int iPosition) const;
	int GetRowSize() const;
	int GetColSize() const;
	int GetBoardSize() const;
	bool IsPositionAtEdge(int iPos) const;
	int GetNewPositionIfMoved(int iPos, MoveKind mk, DirectionKind dk) const;
	bool CanLosePiece(IPlayerPtr spPlayer, int iFromPos, int iToPos) const;
	MoveResult TryMove(int iFromPos, MoveKind mk, DirectionKind dk) const;
	MoveResult PerformLegalMove(IPlayerPtr spPlayer, int iFromPos, MoveKind mk, DirectionKind dkSource);

private:
	// Non interface methods
	bool AddPieces(IPlayerPtr spPlayer);

	class MovementPrimitives
	{
		friend class Board;
		MovementPrimitives(const Board * pBoard);
		bool IsMoveGoingOutsideBoard(int iPos, DirectionKind dk, MoveKind mk) const;
		bool CanLosePiece(IPlayerPtr spPlayer, int iFromPos, int iToPos) const;
		int  GetNewPositionIfMoved(int iPos, DirectionKind dk, MoveKind mk) const;
		bool IsJumpPossible(int iPos, DirectionKind dk, MoveKind mk) const;
		const Board * m_pBoard;
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
