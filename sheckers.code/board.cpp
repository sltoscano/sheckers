// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "factories.h"
#include "board.h"

Board::Board(int iRowSize)
{
	m_iRowSize = iRowSize;

	m_board.resize(iRowSize * iRowSize);

	// Initialize as an empty board
	short toggleColorStart = 0;
	for (int iRow = 0; iRow < iRowSize; iRow++)
	{
		toggleColorStart ^= 1;
		for (int iCol = 0; iCol < iRowSize; iCol++)
		{
			int iPos = (iRow * iRowSize) + iCol;
			m_board[iPos].fOccupied = false;
			m_board[iPos].spPiece.Release();
			if (toggleColorStart == 1)
			{
				m_board[iPos].scColor = (iCol % 2 == 0) ? scBlack : scRed;
			}
			else
			{
				m_board[iPos].scColor = (iCol % 2 == 0) ? scRed : scBlack;
			}
		}
	}
}

Board::~Board()
{
}

long Board::AddRef()
{
	return CAutoRef::AddRef();
}

long Board::Release()
{
	return CAutoRef::Release();
}

bool Board::Setup(IPlayer *pPlayer1, IPlayer *pPlayer2)
{
	IPlayerPtr spPlayer1(pPlayer1);
	IPlayerPtr spPlayer2(pPlayer2);
	FAILED_ASSERT_RETURN(false, spPlayer1 != NULL);
	FAILED_ASSERT_RETURN(false, spPlayer2 != NULL);

	FAILED_ASSERT_RETURN(false, AddPieces(spPlayer1));
	FAILED_ASSERT_RETURN(false, AddPieces(spPlayer2));

	m_spPlayer1 = spPlayer1;
	m_spPlayer2 = spPlayer2;
	return true;
}

bool Board::AddPieces(IPlayer *pPlayer)
{
	IPlayerPtr spPlayer(pPlayer);
	int iPieceCount = spPlayer->GetInitialPieceCount();

	FAILED_ASSERT_RETURN(false, iPieceCount > 0);
	FAILED_ASSERT_RETURN(false, iPieceCount * 2 * 2 <= GetBoardSize());

	int iStartFillPosition;
	PieceType pt;
	FAILED_ASSERT_RETURN(false, spPlayer->GetType(&pt));
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
		if (m_board[i].scColor == scRed)
		{
			IPiecePtr spPiece(PieceFactory::Create(pt));
			FAILED_ASSERT_RETURN(false, spPiece != NULL);
			FAILED_ASSERT_RETURN(false, spPlayer->AddPiece(spPiece));
			FAILED_ASSERT_RETURN(false, PlacePiece(i, spPiece));
			iPlacedCount++;
			if (iPlacedCount == iPieceCount)
				break;
		}
	}
	return true;
}

bool Board::PlacePiece(int iPosition, IPiece *pPiece)
{
	FAILED_ASSERT_RETURN(false, iPosition >= 0);
	FAILED_ASSERT_RETURN(false, iPosition < GetBoardSize());

	IPiecePtr spPiece(pPiece);
	FAILED_ASSERT_RETURN(false, spPiece != NULL);

	// Not asserting here since a user can ask if its
	//	possible to place a piece at this position
	FAILED_RETURN(false, !m_board[iPosition].fOccupied);
	FAILED_RETURN(false, m_board[iPosition].spPiece == NULL);

	int iOldPosition;
	FAILED_ASSERT_RETURN(false, spPiece->GetPosition(&iOldPosition));
	FAILED_ASSERT_RETURN(false, iOldPosition == EmptyPosition);
	
	FAILED_ASSERT_RETURN(false, spPiece->SetPosition(iPosition));

	// Place the piece on the board
	m_board[iPosition].fOccupied = true;
	m_board[iPosition].spPiece = spPiece;
	return true;
}

bool Board::RemovePiece(int iPosition, IPiece **ppPiece)
{
	FAILED_ASSERT_RETURN(false, iPosition >= 0);
	FAILED_ASSERT_RETURN(false, iPosition < GetBoardSize());

	if (ppPiece == NULL)
		return false;

	// Not asserting here since a user can ask if its
	//	possible to remove a piece at this position
	FAILED_RETURN(false, m_board[iPosition].fOccupied);
	FAILED_RETURN(false, m_board[iPosition].spPiece != NULL);

	// Remove the piece from the board
	IPiecePtr spPieceToMove(m_board[iPosition].spPiece);
	FAILED_ASSERT_RETURN(false, spPieceToMove->SetPosition(EmptyPosition));

	// Clear the board
	m_board[iPosition].fOccupied = false;
	m_board[iPosition].spPiece.Release();

	// Give the piece to the user
	*ppPiece = spPieceToMove.Detach();
	return true;
}

bool Board::GetPiece(int iPosition, IPiece **ppPiece) const
{
	FAILED_ASSERT_RETURN(false, iPosition >= 0);
	FAILED_ASSERT_RETURN(false, iPosition < GetBoardSize());

	if (ppPiece == NULL)
		return false;

	if (!m_board[iPosition].fOccupied)
		return false;

	// Addref it and copy it out to the user
	IPiecePtr spPiece = m_board[iPosition].spPiece;
	*ppPiece = spPiece.Detach();
	return true;
}

bool Board::IsOccupied(int iPosition) const
{
	FAILED_ASSERT_RETURN(false, iPosition >= 0);
	FAILED_ASSERT_RETURN(false, iPosition < GetBoardSize());
	return m_board[iPosition].fOccupied;
}

bool Board::GetSquareColor(int iPosition, SquareColor *psc) const
{
	if (psc == NULL)
		return false;

	FAILED_ASSERT_RETURN(false, iPosition >= 0);
	FAILED_ASSERT_RETURN(false, iPosition < GetBoardSize());
	*psc = m_board[iPosition].scColor;
	return true;
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

bool Board::TryMove(int iPos, MoveKind mk, DirectionKind dk, MoveResult *pmr) const
{
	if (pmr == NULL)
		return false;

	MovementPrimitives movement(this);
	if (movement.IsMoveGoingOutsideBoard(iPos, dk, mk))
	{
		*pmr = mrIllegal;
		return true;
	}

	int newPos = movement.GetNewPositionIfMoved(iPos, dk, mk);
	if (m_board[newPos].fOccupied)
	{
		// If opponent piece the check if a jump is legal
		if (movement.IsJumpPossible(iPos, dk, mk))
		{
			*pmr = mrLegalJump;
			return true;
		}
		else
		{
			*pmr = mrIllegal;
			return true;
		}
	}
	else
	{
		*pmr = mrLegalNonJump;
		return true;
	}
}

bool Board::PerformLegalMove(IPlayer *pPlayer, int iSourcePos, DirectionKind dkSource, MoveKind mk, MoveResult *pmr)
{
	if (pmr == NULL)
		return false;

	IPlayerPtr spPlayer(pPlayer);
	FAILED_ASSERT_RETURN(false, spPlayer != NULL);
	FAILED_ASSERT_RETURN(false, spPlayer->SetLastMovePosition(EmptyPosition));
	
	MovementPrimitives movement(this);

	if (movement.IsMoveGoingOutsideBoard(iSourcePos, dkSource, mk))
	{
		ASSERTMSG(false, "Making this move is NOT legal, call TryMove first");
		*pmr = mrIllegal;
		return false;
	}

	int iTargetPos = movement.GetNewPositionIfMoved(iSourcePos, dkSource, mk);
	if (m_board[iTargetPos].fOccupied)
	{
		PieceType ptSource;
		FAILED_ASSERT_RETURN(false, spPlayer->GetType(&ptSource));
		PieceType ptTarget;
		FAILED_ASSERT_RETURN(false, m_board[iTargetPos].spPiece->GetType(&ptTarget));
		if (ptSource == ptTarget)
		{
			ASSERTMSG(false, "Making this jump is NOT legal, call TryMove first");
			*pmr = mrIllegal;
			return false;
		}
		else
		{
			// Remove the piece to move from the board
			IPiecePtr spPieceToMove;
			FAILED_ASSERT_RETURN(false, RemovePiece(iSourcePos, &spPieceToMove));

			// Remove the jumped piece from the board
			IPiecePtr spJumpedPiece;
			FAILED_ASSERT_RETURN(false, RemovePiece(iTargetPos, &spJumpedPiece));

			// Take away the piece from the other player
			IPlayerPtr spOpponent = (spPlayer == m_spPlayer1) ? m_spPlayer2 : m_spPlayer1;
			FAILED_ASSERT_RETURN(false, spOpponent->LosePiece(spJumpedPiece));

			// Place the moved piece to the empty spot
			int iNewJumpPos = movement.GetNewPositionIfMoved(iTargetPos, dkSource, mk);
			FAILED_ASSERT_RETURN(false, PlacePiece(iNewJumpPos, spPieceToMove));

			// Add piece to current player's captured list
			FAILED_ASSERT_RETURN(false, spPlayer->CapturePiece(spJumpedPiece));

			// Record the last move position to allow future jumps
			FAILED_ASSERT_RETURN(false, spPlayer->SetLastMovePosition(iNewJumpPos));
			*pmr = mrLegalJump;
			return true;
		}
	}
	else
	{
		// Square is not occupied so this is a normal non-jump move
		IPiecePtr spPieceToMove;
		FAILED_ASSERT_RETURN(false, RemovePiece(iSourcePos, &spPieceToMove));
		FAILED_ASSERT_RETURN(false, PlacePiece(iTargetPos, spPieceToMove));
		*pmr = mrLegalNonJump;
		return true;
	}

	*pmr = mrIllegal;
	return false;
}
