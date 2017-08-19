// Copyright: Steven Toscano

#include "common.h"

#include "board.h"

Board::MovementPrimitives::MovementPrimitives(const Board * pBoard) :
	m_pBoard(pBoard)
{
}

bool Board::MovementPrimitives::IsMoveGoingOutsideBoard(int iPos, DirectionKind dk, MoveKind mk) const
{
	int iRowSize = m_pBoard->GetRowSize();

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
	int iRowSize = m_pBoard->GetRowSize();

	int iHorizontalOffset = (dk == dkDown) ?
		iRowSize : -iRowSize;
	int iVerticalOffset = (dk == dkDown) ?
		(mk == mkRight) ? -1 : 1 :
		(mk == mkLeft) ? -1 : 1;

	return iPos + iHorizontalOffset + iVerticalOffset;
}

bool Board::MovementPrimitives::IsJumpPossible(int iPos, DirectionKind dk, MoveKind mk) const
{
	if (!m_pBoard->IsOccupied(iPos))
		return false;

	IPiecePtr spSource = m_pBoard->GetPiece(iPos);
	FAILED_ASSERT_RETURN(false, spSource);
	PieceType ptSource = spSource->GetType();

	if (IsMoveGoingOutsideBoard(iPos, dk, mk))
		return false;

	int iNewPos = GetNewPositionIfMoved(iPos, dk, mk);

	if (!m_pBoard->IsOccupied(iNewPos))
		return false;

	IPiecePtr spJumpTarget = m_pBoard->GetPiece(iNewPos);
	FAILED_ASSERT_RETURN(false, spJumpTarget);
	PieceType ptJumpTarget = spJumpTarget->GetType();

	// Can only jump pieces from a different player
	if (ptSource == ptJumpTarget)
		return false;

	if (IsMoveGoingOutsideBoard(iNewPos, dk, mk))
		return false;

	int iPosAfterJump = GetNewPositionIfMoved(iNewPos, dk, mk);

	if (m_pBoard->IsOccupied(iPosAfterJump))
		return false;

	return true;
}


bool Board::MovementPrimitives::CanLosePiece(IPlayerPtr spPlayer, int iFromPos, int iToPos) const
{
	FAILED_ASSERT_RETURN(false, spPlayer);

	// If the new position after a jump is at the edge
	//	then it can't be jumped by the opponent
	if (m_pBoard->IsPositionAtEdge(iToPos))
		return false;

	// Otherwise check to see if an opponent piece is
	//	poised for a legal jump over the new position
	int iTopLeft = iToPos - m_pBoard->GetRowSize() - 1;
	int iTopRight = iToPos - m_pBoard->GetRowSize() + 1;
	int iBottomLeft = iToPos + m_pBoard->GetRowSize() - 1;
	int iBottomRight = iToPos + m_pBoard->GetRowSize() + 1;

	PieceType pt = spPlayer->GetType();
	DirectionKind dk = (pt == ptRed) ? dkDown : dkUp;

	if (dk == dkUp)
	{
		IPiecePtr spPieceTopLeft = m_pBoard->GetPiece(iTopLeft);
		if (spPieceTopLeft)
		{
			if (pt != spPieceTopLeft->GetType())
			{
				if (iBottomRight == iFromPos)
				{
					return true;
				}
				else if (!m_pBoard->IsOccupied(iBottomRight))
				{
					return true;
				}
			}
		}
		IPiecePtr spPieceTopRight = m_pBoard->GetPiece(iTopRight);
		if (spPieceTopRight)
		{
			if (pt != spPieceTopRight->GetType())
			{
				if (iBottomLeft == iFromPos)
				{
					return true;
				}
				else if (!m_pBoard->IsOccupied(iBottomLeft))
				{
					return true;
				}
			}
		}
	}
	else
	{
		IPiecePtr spPieceBottomLeft = m_pBoard->GetPiece(iBottomLeft);
		if (spPieceBottomLeft)
		{
			if (pt != spPieceBottomLeft->GetType())
			{
				if (iTopRight == iFromPos)
				{
					return true;
				}
				else if (!m_pBoard->IsOccupied(iTopRight))
				{
					return true;
				}
			}
		}
		IPiecePtr spPieceBottomRight = m_pBoard->GetPiece(iBottomRight);
		if (spPieceBottomRight)
		{
			if (pt != spPieceBottomRight->GetType())
			{
				if (iTopLeft == iFromPos)
				{
					return true;
				}
				else if (!m_pBoard->IsOccupied(iTopLeft))
				{
					return true;
				}
			}
		}
	}

	return false;
}
