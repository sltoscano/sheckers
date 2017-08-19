// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "factories.h"
#include "player.h"

PlayerPieces::PlayerPieces()
{
}

PlayerPieces::~PlayerPieces()
{
}

IPiecePtr PlayerPieces::GetNext()
{
	if (m_iter == m_pieces.end())
		return NULL;

	IPiecePtr spPiece = *m_iter;
	m_iter++;
	return spPiece;
}

void PlayerPieces::AddQuery(vector<IPiecePtr> &pieces)
{
	m_pieces = pieces;
	m_iter = m_pieces.begin();
}

Player::Player(wchar_t* wstrName, PieceType pt, int iPieceCount, IFeedback *pFeedback)
{
	m_wstrName = wstring(wstrName);
	m_pt = pt;
	m_iInitialPieceCount = iPieceCount;
	m_iLastMovePosition = EmptyPosition;
}

Player::~Player()
{
}

bool Player::AddPiece(IPiecePtr spPiece)
{
	m_pieces.push_back(spPiece);
	return true;
}

bool Player::LosePiece(IPiecePtr spPiece)
{
	PieceType pt = spPiece->GetType();
	// Piece to remove should be from the same player
	FAILED_ASSERT_RETURN(false, pt == m_pt);
	PieceIter iter;
	iter = find(m_pieces.begin(), m_pieces.end(), spPiece);
	// The piece should exist
	FAILED_ASSERT_RETURN(false, iter != m_pieces.end());
	m_pieces.erase(iter);
	return true;
}

bool Player::CapturePiece(IPiecePtr spPiece)
{
	PieceType pt = spPiece->GetType();
	// Piece to capture should not be from the same player
	FAILED_ASSERT_RETURN(false, pt != m_pt);
	m_capturedPieces.push_back(spPiece);
	return true;
}

int Player::GetPieceOwnedCount() const
{
	return static_cast<int>(m_pieces.size());
}

int Player::GetPieceCapturedCount() const
{
	return static_cast<int>(m_capturedPieces.size());
}

wstring Player::GetName() const
{
	return m_wstrName;
}

PieceType Player::GetType() const
{
	return m_pt;
}

int Player::GetInitialPieceCount() const
{
	return m_iInitialPieceCount;
}

int Player::GetLastMovePosition() const
{
	return m_iLastMovePosition;
}

void Player::SetLastMovePosition(int iPos)
{
	m_iLastMovePosition = iPos;
}

IPlayerPiecesPtr Player::GetPieces()
{
	IPlayerPieces * pPlayerPieces(new PlayerPieces());
	pPlayerPieces->AddQuery(m_pieces);
	IPlayerPiecesPtr spPieces(pPlayerPieces);
	return spPieces;
}
