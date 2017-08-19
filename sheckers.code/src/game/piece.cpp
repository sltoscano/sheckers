// Copyright: Steven Toscano

#include "common.h"

#include "piece.h"

Piece::Piece(PieceType pt) :
	m_iPosition(EmptyPosition),
	m_pt(pt)
{
	m_dk = (pt == ptRed) ? dkDown : dkUp;
}

Piece::~Piece()
{
}

PieceType Piece::GetType() const
{
	return m_pt;
}

int Piece::GetPosition() const
{
	return m_iPosition;
}

void Piece::SetPosition(int iPosition)
{
	FAILED_ASSERT_RETURN(, iPosition == EmptyPosition ? m_iPosition != EmptyPosition : 1);
	FAILED_ASSERT_RETURN(, m_iPosition != iPosition);
	m_iPosition = iPosition;
}

DirectionKind Piece::GetDirection() const
{
	return m_dk;
}

void Piece::SetDirection(enum DirectionKind dk)
{
	m_dk = dk;
}
