// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "player.h"

PlayerPieces::PlayerPieces()
{
}

PlayerPieces::~PlayerPieces()
{
}

long PlayerPieces::AddRef()
{
	return CAutoRef::AddRef();
}

long PlayerPieces::Release()
{
	return CAutoRef::Release();
}

bool PlayerPieces::GetNext(IPiece **ppPiece)
{
	if (ppPiece == NULL)
		return false;

	if (m_iter == m_pieces.end())
		return false;

	IPiecePtr spPiece = *m_iter;
	*ppPiece = spPiece.Detach();
	m_iter++;
	return true;
}

bool PlayerPieces::AddQuery(vector<IPiecePtr> &pieces)
{
	FAILED_ASSERT_RETURN(false, m_pieces.size() == 0);
	m_pieces = pieces;
	m_iter = m_pieces.begin();
	return true;
}

Player::Player(wstring wstrName, PieceType pt, int iPieceCount)
{
	m_wstrName = wstrName;
	m_pt = pt;
	m_iInitialPieceCount = iPieceCount;
	m_iLastMovePosition = EmptyPosition;
}

Player::~Player()
{
}

long Player::AddRef()
{
	return CAutoRef::AddRef();
}

long Player::Release()
{
	return CAutoRef::Release();
}

bool Player::AddPiece(IPiece *pPiece)
{
	IPiecePtr spPiece(pPiece);
	FAILED_ASSERT_RETURN(false, spPiece != NULL);
	m_pieces.push_back(spPiece);
	return true;
}

bool Player::LosePiece(IPiece *pPiece)
{
	IPiecePtr spPiece(pPiece);
	PieceType pt;
	FAILED_ASSERT_RETURN(false, spPiece->GetType(&pt));
	// Piece to remove should be from the same player
	FAILED_ASSERT_RETURN(false, pt == m_pt);
	PieceIter iter;
	iter = find(m_pieces.begin(), m_pieces.end(), spPiece);
	// The piece should exist
	FAILED_ASSERT_RETURN(false, iter != m_pieces.end());
	m_pieces.erase(iter);
	return true;
}

bool Player::CapturePiece(IPiece *pPiece)
{
	IPiecePtr spPiece(pPiece);
	PieceType pt;
	FAILED_ASSERT_RETURN(false, spPiece->GetType(&pt));
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

bool Player::GetName(wstring *pwstrName) const
{
	if (pwstrName == NULL)
		return false;

	*pwstrName = m_wstrName;
	return true;
}

bool Player::GetType(PieceType *ppt) const
{
	if (ppt == NULL)
		return false;

	*ppt = m_pt;
	return true;
}

int Player::GetInitialPieceCount() const
{
	return m_iInitialPieceCount;
}

int Player::GetLastMovePosition() const
{
	return m_iLastMovePosition;
}

bool Player::SetLastMovePosition(int iPos)
{
	m_iLastMovePosition = iPos;
	return true;
}

bool Player::GetPieces(IPlayerPieces **ppPieces)
{
	PlayerPieces *pPlayerPieces = new PlayerPieces();
	IPlayerPiecesPtr spPieces(pPlayerPieces);
	FAILED_ASSERT_RETURN(false, pPlayerPieces->AddQuery(m_pieces));
	*ppPieces = spPieces.Detach();
	return true;
}
