// Copyright: Steven Toscano

#pragma once

class Piece : 
	public IPiece
{
public:
	Piece(PieceType pt);
	~Piece();

	// IPiece methods
	PieceType GetType() const;
	int GetPosition() const;
	void SetPosition(int iPosition);
	DirectionKind GetDirection() const;
	void SetDirection(enum DirectionKind dk);

private:
	DirectionKind m_dk;
	const PieceType m_pt;
	int m_iPosition;

private:
	// Disallow copy and assignment
	Piece(const Piece &);
	Piece& operator=(const Piece &);
};
