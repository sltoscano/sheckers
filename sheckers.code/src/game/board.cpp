// Copyright: Steven Toscano

#include "common.h"

#include "factories.h"
#include "board.h"

Board::Board(int iRowSize)
{
	m_iRowSize = iRowSize;

	m_board.resize(iRowSize * iRowSize);

	// Initialize as an empty board
	char toggleColorStart = 0;
	for (int iRow = 0; iRow < iRowSize; iRow++)
	{
		toggleColorStart ^= 1;
		for (int iCol = 0; iCol < iRowSize; iCol++)
		{
			int iPos = (iRow * iRowSize) + iCol;
			m_board[iPos].fOccupied = false;
			if (toggleColorStart == 1)
			{
				m_board[iPos].scColor = (iCol % 2 == 0) ? scBlack : scWhite;
			}
			else
			{
				m_board[iPos].scColor = (iCol % 2 == 0) ? scWhite : scBlack;
			}
		}
	}
}

Board::~Board()
{
}

bool Board::Setup(IPlayerPtr spPlayer1, IPlayerPtr spPlayer2)
{
	FAILED_ASSERT_RETURN(false, spPlayer1);
	FAILED_ASSERT_RETURN(false, spPlayer2);

	m_spPlayer1 = spPlayer1;
	m_spPlayer2 = spPlayer2;

	if (m_spPlayer1->GetInitialPieceCount() > 0)
		FAILED_ASSERT_RETURN(false, AddPieces(m_spPlayer1));

	if (m_spPlayer2->GetInitialPieceCount() > 0)
		FAILED_ASSERT_RETURN(false, AddPieces(m_spPlayer2));

	return true;
}

bool Board::AddPieces(IPlayerPtr spPlayer)
{
	int iPieceCount = spPlayer->GetInitialPieceCount();

	FAILED_ASSERT_RETURN(false, iPieceCount > 0);
	FAILED_ASSERT_RETURN(false, iPieceCount * 2 * 2 <= GetBoardSize());

	int iStartFillPosition;
	PieceType pt = spPlayer->GetType();
	DirectionKind dk = (pt == ptRed) ? dkDown : dkUp;

	// Start placing pieces depending on orientation for an 8x8
	//	board top player starts at 1 and bottom player starts
	//	at (8*8) - (12*2) = 40

	if (dk == dkDown)
		iStartFillPosition = 0;
	else
		iStartFillPosition = GetBoardSize() - (iPieceCount * 2);

	int iPlacedCount = 0;
	for (int i = iStartFillPosition; i < GetBoardSize(); i++)
	{
		// Pieces are only placed on the same color square
		if (m_board[i].scColor == scWhite)
		{
			IPiecePtr spPiece(PieceFactory::Create(pt));
			FAILED_ASSERT_RETURN(false, spPiece);
			spPiece->SetDirection(dk);
			FAILED_ASSERT_RETURN(false, spPlayer->AddPiece(spPiece));
			FAILED_ASSERT_RETURN(false, PlacePiece(i, spPiece));
			iPlacedCount++;
			if (iPlacedCount == iPieceCount)
				break;
		}
	}
	return true;
}

bool Board::PlacePiece(int iPosition, IPiecePtr spPiece)
{
	FAILED_ASSERT_RETURN(false, iPosition >= 0);
	FAILED_ASSERT_RETURN(false, iPosition < GetBoardSize());

	FAILED_ASSERT_RETURN(false, spPiece);

	// Not asserting here since a user can ask if its
	//	possible to place a piece at this position
	FAILED_RETURN(false, !m_board[iPosition].fOccupied);
	FAILED_RETURN(false, m_board[iPosition].spPiece == NULL);

	FAILED_ASSERT_RETURN(false, spPiece->GetPosition() == EmptyPosition);
	spPiece->SetPosition(iPosition);

	// Place the piece on the board
	m_board[iPosition].fOccupied = true;
	m_board[iPosition].spPiece = spPiece;
	return true;
}

IPiecePtr Board::RemovePiece(int iPosition)
//bool Board::RemovePiece(int iPosition, IPiece **ppPiece)
{
	FAILED_ASSERT_RETURN(IPiecePtr(), iPosition >= 0);
	FAILED_ASSERT_RETURN(IPiecePtr(), iPosition < GetBoardSize());

	// Can only remove a piece if it exists on the board
	FAILED_ASSERT_RETURN(IPiecePtr(), m_board[iPosition].fOccupied);
	FAILED_ASSERT_RETURN(IPiecePtr(), m_board[iPosition].spPiece);

	// Remove the piece from the board
	IPiecePtr spPieceToMove(m_board[iPosition].spPiece);
	spPieceToMove->SetPosition(EmptyPosition);

	// Clear the board
	m_board[iPosition].fOccupied = false;
	m_board[iPosition].spPiece.reset();

	// Give the piece to the user
	return spPieceToMove;
}

IPiecePtr Board::GetPiece(int iPosition) const
{
	FAILED_ASSERT_RETURN(IPiecePtr(), iPosition >= 0);
	FAILED_ASSERT_RETURN(IPiecePtr(), iPosition < GetBoardSize());
	
	// Return an empty pointer if the position is not occupied
	FAILED_RETURN(IPiecePtr(), m_board[iPosition].fOccupied);

	// Addref it and copy it out to the user
	return m_board[iPosition].spPiece;
}

IPlayerPtr Board::GetPlayer(PieceType pt) const
{
	PieceType ptPlayer1 = m_spPlayer1->GetType();

	return (pt == ptPlayer1) ? m_spPlayer1 : m_spPlayer2;
}

bool Board::IsOccupied(int iPosition) const
{
	FAILED_ASSERT_RETURN(false, iPosition >= 0);
	FAILED_ASSERT_RETURN(false, iPosition < GetBoardSize());
	return m_board[iPosition].fOccupied;
}

SquareColor Board::GetSquareColor(int iPosition) const
{
	FAILED_ASSERT_RETURN(scWhite, iPosition >= 0);
	FAILED_ASSERT_RETURN(scWhite, iPosition < GetBoardSize());
	return m_board[iPosition].scColor;
}

int Board::GetColSize() const
{
	return m_iRowSize;
}

int Board::GetRowSize() const
{
	return m_iRowSize;
}

int Board::GetBoardSize() const
{
	return static_cast<int>(m_board.size());
}

bool Board::IsPositionAtEdge(int iPos) const
{
	vector<MoveKind> mks;
	mks.push_back(mkLeft); mks.push_back(mkRight);
	vector<DirectionKind> dks;
	dks.push_back(dkUp); dks.push_back(dkDown);

	// If any single move causes the piece to go
	// outside of the board then its on the edge

	MovementPrimitives movement(this);
	for each (DirectionKind dk in dks)
	{
		for each (MoveKind mk in mks)
		{
			if (movement.IsMoveGoingOutsideBoard(iPos, dk, mk))
				return true;
		}
	}
	return false;
}

int Board::GetNewPositionIfMoved(int iPos, MoveKind mk, DirectionKind dk) const
{
	MovementPrimitives movement(this);
	return movement.GetNewPositionIfMoved(iPos, dk, mk);
}

bool Board::CanLosePiece(IPlayerPtr spPlayer, int iFromPos, int iToPos) const
{
	MovementPrimitives movement(this);
	return movement.CanLosePiece(spPlayer, iFromPos, iToPos);
}

MoveResult Board::TryMove(int iFromPos, MoveKind mk, DirectionKind dk) const
{
	MovementPrimitives movement(this);
	if (movement.IsMoveGoingOutsideBoard(iFromPos, dk, mk))
	{
		return mrIllegal;
	}

	int newPos = movement.GetNewPositionIfMoved(iFromPos, dk, mk);
	if (m_board[newPos].fOccupied)
	{
		// If opponent piece the check if a jump is legal
		if (movement.IsJumpPossible(iFromPos, dk, mk))
		{
			return mrLegalJump;
		}
		else
		{
			return mrIllegal;
		}
	}
	else
	{
		return mrLegalNonJump;
	}
}

MoveResult Board::PerformLegalMove(IPlayerPtr spPlayer, int iFromPos, MoveKind mk, DirectionKind dkSource)
{
	FAILED_ASSERT_RETURN(mrError, spPlayer);
	spPlayer->SetLastMovePosition(EmptyPosition);
	
	MovementPrimitives movement(this);

	if (movement.IsMoveGoingOutsideBoard(iFromPos, dkSource, mk))
	{
		ASSERTMSG(false, "Making this move is NOT legal, call TryMove first");
		return mrIllegal;
	}

	int iTargetPos = movement.GetNewPositionIfMoved(iFromPos, dkSource, mk);
	if (m_board[iTargetPos].fOccupied)
	{
		PieceType ptSource = spPlayer->GetType();
		PieceType ptTarget = m_board[iTargetPos].spPiece->GetType();
		if (ptSource == ptTarget)
		{
			ASSERTMSG(false, "Making this jump is NOT legal, call TryMove first");
			return mrIllegal;
		}
		else
		{
			// Remove the piece to move from the board
			IPiecePtr spPieceToMove = RemovePiece(iFromPos);

			// Remove the jumped piece from the board
			IPiecePtr spJumpedPiece = RemovePiece(iTargetPos);

			// Take away the piece from the other player
			IPlayerPtr spOpponent = (spPlayer == m_spPlayer1) ? m_spPlayer2 : m_spPlayer1;
			FAILED_ASSERT_RETURN(mrError, spOpponent->LosePiece(spJumpedPiece));

			// Place the moved piece to the empty spot
			int iNewJumpPos = movement.GetNewPositionIfMoved(iTargetPos, dkSource, mk);
			FAILED_ASSERT_RETURN(mrError, PlacePiece(iNewJumpPos, spPieceToMove));

			// Add piece to current player's captured list
			FAILED_ASSERT_RETURN(mrError, spPlayer->CapturePiece(spJumpedPiece));

			// Record the last move position to allow future jumps
			spPlayer->SetLastMovePosition(iNewJumpPos);
			return mrLegalJump;
		}
	}
	else
	{
		// Square is not occupied so this is a normal non-jump move
		IPiecePtr spPieceToMove = RemovePiece(iFromPos);
		FAILED_ASSERT_RETURN(mrError, PlacePiece(iTargetPos, spPieceToMove));
		return mrLegalNonJump;
	}

	return mrIllegal;
}
