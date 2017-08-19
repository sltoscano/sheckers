// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include "game.h"

Game::Game(IBoard *pBoard) :
	m_turn(0),
	m_iTurnCount(0),
	m_gs(gsNotStarted),
	m_spBoard(pBoard)
{
}

Game::~Game()
{
}

long Game::AddRef()
{
	return CAutoRef::AddRef();
}

long Game::Release()
{
	return CAutoRef::Release();
}

void Game::Start(IPlayer *pPlayer1, IPlayer *pPlayer2)
{
	FAILED_ASSERT_RETURN(, m_spBoard != NULL);

	m_turn = 0;
	m_iTurnCount = 0;
	m_gs = gsStarted;
	m_spPlayer1 = pPlayer1;
	m_spPlayer2 = pPlayer2;
	m_spBoard->Setup(m_spPlayer1, m_spPlayer2);
}

void Game::SetState(GameState gs)
{
	m_gs = gs;
}

bool Game::IsOver() const
{
	if (m_gs == gsError ||
		m_gs == gsForfeit ||
		m_gs == gsStalemate ||
		m_gs == gsPlayer1Wins ||
		m_gs == gsPlayer2Wins)
		return true;

	return false;
}

GameState Game::GetState() const
{
	return m_gs;
}

MoveResult Game::DoTurn(MoveResult mrLast)
{
	// Shouldn't ask to do a turn if the game is over
	FAILED_ASSERT_RETURN(mrError, !Game::IsOver());

	m_iTurnCount++;
	m_gs = gsRunning;

	// If the last move was a legal jump allow the
	//	same player to go again
	if (mrLast != mrLegalJump)
		m_turn ^= 1;

	IPlayerPtr spCurrentPlayer;
	spCurrentPlayer = (m_turn == 1) ? m_spPlayer1 : m_spPlayer2;

	MoveResult mr = spCurrentPlayer->MakeMove(m_spBoard, mrLast);

	// End game if no player can make a legal move
	if (mr == mrNone && mrLast == mrNone)
		m_gs = gsStalemate;

	// Other end game conditions
	if (mr == mrError)
		m_gs = gsError;
	if (mr == mrForfeit)
		m_gs = gsForfeit;
	if (m_spPlayer1->GetPieceOwnedCount() == 0)
		m_gs = gsPlayer2Wins;
	if (m_spPlayer2->GetPieceOwnedCount() == 0)
		m_gs = gsPlayer1Wins;

	return mr;
}
