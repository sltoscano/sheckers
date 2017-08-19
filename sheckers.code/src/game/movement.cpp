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


bool Board::MovementPrimitives::CanLosePiece(IPlayer *pPlayer, int iFromPos, int iToPos) const
{
	IPlayerPtr spPlayer(pPlayer);
	FAILED_ASSERT_RETURN(false, spPlayer != NULL);

	// If the new position after a jump is at the edge
	//	then it can't be jumped by the opponent
	if (m_spBoard->IsPositionAtEdge(iToPos))
		return false;

	// Otherwise check to see if an opponent piece is
	//	poised for a legal jump over the new position
	int iTopLeft = iToPos - m_spBoard->GetRowSize() - 1;
	int iTopRight = iToPos - m_spBoard->GetRowSize() + 1;
	int iBottomLeft = iToPos + m_spBoard->GetRowSize() - 1;
	int iBottomRight = iToPos + m_spBoard->GetRowSize() + 1;

	PieceType pt;
	spPlayer->GetType(&pt);
	DirectionKind dk = (pt == ptRed) ? dkDown : dkUp;

	if (dk == dkUp)
	{
		IPiecePtr spPieceTopLeft;
		if (m_spBoard->GetPiece(iTopLeft, &spPieceTopLeft))
		{
			PieceType ptTopLeft;
			spPieceTopLeft->GetType(&ptTopLeft);
			if (pt != ptTopLeft)
			{
				if (iBottomRight == iFromPos)
				{
					return true;
				}
				else if (!m_spBoard->IsOccupied(iBottomRight))
				{
					return true;
				}
			}
		}
		IPiecePtr spPieceTopRight;
		if (m_spBoard->GetPiece(iTopRight, &spPieceTopRight))
		{
			PieceType ptTopRight;
			spPieceTopRight->GetType(&ptTopRight);
			if (pt != ptTopRight)
			{
				if (iBottomLeft == iFromPos)
				{
					return true;
				}
				else if (!m_spBoard->IsOccupied(iBottomLeft))
				{
					return true;
				}
			}
		}
	}
	else
	{
		IPiecePtr spPieceBottomLeft;
		if (m_spBoard->GetPiece(iBottomLeft, &spPieceBottomLeft))
		{
			PieceType ptBottomLeft;
			spPieceBottomLeft->GetType(&ptBottomLeft);
			if (pt != ptBottomLeft)
			{
				if (iTopRight == iFromPos)
				{
					return true;
				}
				else if (!m_spBoard->IsOccupied(iTopRight))
				{
					return true;
				}
			}
		}
		IPiecePtr spPieceBottomRight;
		if (m_spBoard->GetPiece(iBottomRight, &spPieceBottomRight))
		{
			PieceType ptBottomRight;
			spPieceBottomRight->GetType(&ptBottomRight);
			if (pt != ptBottomRight)
			{
				if (iTopLeft == iFromPos)
				{
					return true;
				}
				else if (!m_spBoard->IsOccupied(iTopLeft))
				{
					return true;
				}
			}
		}
	}

	return false;
}
