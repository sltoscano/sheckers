// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "board.h"

Board::MovementPrimitives::MovementPrimitives(const IBoard *pBoard) :
	m_spBoard(const_cast<IBoard *>(pBoard))
{
}

bool Board::MovementPrimitives::IsMoveGoingOutsideBoard(int iPos, DirectionKind dk, MoveKind mk) const
{
	int iRowSize = m_spBoard->GetRowSize();

	if (dk == dkDown)
	{
		if (iPos >= (iRowSize * iRowSize) - iRowSize)
		{
			// At the bottom of the board, no move legal
			return true;
		}
		if (iPos % iRowSize == 0)
		{
			if (mk == mkRight)
			{
				// At the left edge of the board, right move not legal
				return true;
			}
		}
		else if (iPos % iRowSize == (iRowSize - 1))
		{
			if (mk == mkLeft)
			{
				// At the right edge of the board, left move not legal
				return true;
			}
		}
	}
	else
	{
		if (iPos < iRowSize)
		{
			// At the top of the board, no move legal
			return true;
		}
		if (iPos % iRowSize == 0)
		{
			if (mk == mkLeft)
			{
				// At the left edge of the board, left move not legal
				return true;
			}
		}
		if (iPos % iRowSize == (iRowSize - 1))
		{
			if (mk == mkRight)
			{
				// At the right edge of the board, right move not legal
				return true;
			}
		}
	}
	return false;
}

int Board::MovementPrimitives::GetNewPositionIfMoved(int iPos, DirectionKind dk, MoveKind mk) const
{
	int iRowSize = m_spBoard->GetRowSize();

	int iHorizontalOffset = (dk == dkDown) ?
		iRowSize : -iRowSize;
	int iVerticalOffset = (dk == dkDown) ?
		(mk == mkRight) ? -1 : 1 :
		(mk == mkLeft) ? -1 : 1;

	return iPos + iHorizontalOffset + iVerticalOffset;
}

bool Board::MovementPrimitives::IsJumpPossible(int iPos, DirectionKind dk, MoveKind mk) const
{
	if (!m_spBoard->IsOccupied(iPos))
		return false;

	PieceType ptSource;
	IPiecePtr spSource;
	FAILED_ASSERT_RETURN(false, m_spBoard->GetPiece(iPos, &spSource));
	FAILED_ASSERT_RETURN(false, spSource != NULL);
	FAILED_ASSERT_RETURN(false, spSource->GetType(&ptSource));

	if (IsMoveGoingOutsideBoard(iPos, dk, mk))
		return false;

	int iNewPos = GetNewPositionIfMoved(iPos, dk, mk);

	if (!m_spBoard->IsOccupied(iNewPos))
		return false;

	PieceType ptJumpTarget;
	IPiecePtr spJumpTarget;
	FAILED_ASSERT_RETURN(false, m_spBoard->GetPiece(iNewPos, &spJumpTarget));
	FAILED_ASSERT_RETURN(false, spJumpTarget->GetType(&ptJumpTarget));

	// Can only jump pieces from a different player
	if (ptSource == ptJumpTarget)
		return false;

	if (IsMoveGoingOutsideBoard(iNewPos, dk, mk))
		return false;

	int iPosAfterJump = GetNewPositionIfMoved(iNewPos, dk, mk);

	if (m_spBoard->IsOccupied(iPosAfterJump))
		return false;

	return true;
}
