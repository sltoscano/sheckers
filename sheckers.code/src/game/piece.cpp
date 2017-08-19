// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "factories.h"
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

long Piece::AddRef()
{
	return CAutoRef::AddRef();
}

long Piece::Release()
{
	return CAutoRef::Release();
}

bool Piece::GetType(enum PieceType *ppt) const
{
	if (ppt == NULL)
		return false;

	*ppt = m_pt;
	return true;
}

bool Piece::GetPosition(int *piPosition) const
{
	if (piPosition == NULL)
		return false;

	*piPosition = m_iPosition;
	return true;
}

bool Piece::SetPosition(int iPosition)
{
	FAILED_ASSERT_RETURN(false, iPosition == EmptyPosition ? m_iPosition != EmptyPosition : 1);
	FAILED_ASSERT_RETURN(false, m_iPosition != iPosition);
	m_iPosition = iPosition;
	return true;
}

bool Piece::GetDirection(enum DirectionKind *pdk) const
{
	if (pdk == NULL)
		return false;

	*pdk = m_dk;
	return true;
}

bool Piece::SetDirection(enum DirectionKind dk)
{
	m_dk = dk;
	return true;
}
