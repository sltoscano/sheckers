// Copyright: Steven Toscano

#pragma once

class Game :
	public IGame
{
public:
	Game(IBoardPtr spBoard);
	~Game();

	// IGame methods
	void Start(IPlayerPtr spPlayer1, IPlayerPtr spPlayer2);
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
	char m_turn;

private:
	// Disallow copy and assignment
	Game(const Game &);
	Game& operator=(const Game &);
};
