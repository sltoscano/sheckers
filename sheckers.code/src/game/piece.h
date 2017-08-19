// Copyright: Steven Toscano

#pragma once

class Piece : 
	public IPiece,
	public CAutoRef
{
public:
	Piece(PieceType pt);
	~Piece();

	// IReferenceCounted methods
	long AddRef();
	long Release();

	// IPiece methods
	bool GetType(enum PieceType *ppt) const;
	bool GetPosition(int *piPosition) const;
	bool SetPosition(int iPosition);
	bool GetDirection(enum DirectionKind *pdk) const;
	bool SetDirection(enum DirectionKind dk);

private:
	DirectionKind m_dk;
	const PieceType m_pt;
	int m_iPosition;

private:
	// Disallow copy and assignment
	Piece(const Piece &);
	Piece& operator=(const Piece &);
};
