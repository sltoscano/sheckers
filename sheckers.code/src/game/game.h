// Copyright: Steven Toscano

#pragma once

class Game :
	public IGame,
	public CAutoRef
{
public:
	Game(IBoard *pBoard);
	~Game();

	// IReferenceCounted methods
	long AddRef();
	long Release();

	// IGame methods
	void Start(IPlayer *pPlayer1, IPlayer *pPlayer2);
	void SetState(GameState gs);
	bool IsOver() const;
	GameState GetState() const;
	MoveResult DoTurn(MoveResult mrLast);

private:
	IPlayerPtr m_spPlayer1;
	IPlayerPtr m_spPlayer2;
	IBoardPtr m_spBoard;
	GameState m_gs;
	int m_iTurnCount;
	short m_turn;

private:
	// Disallow copy and assignment
	Game(const Game &);
	Game& operator=(const Game &);
};
