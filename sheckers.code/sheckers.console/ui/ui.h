// Copyright: Steven Toscano

#pragma once

class UserInput
{
public:
	UserInput(wstring wstrName);
	UserInput();
	~UserInput();

	void Read();
	void Clear();
	bool ReadBoardRowSize(int *piBoardRowSize);
	bool ReadPieceCount(int iBoardRowSize, int *piPieceCount);
	bool ReadPlayer(wstring *pwstrPlayer, PlayerKind *ppkPlayer);
	bool ReadMove(IBoard *pBoard, PieceType pt, int *piPos, DirectionKind *pdk, MoveKind *pmk);
	bool ListLegalMoves();

private:
	bool ReadInt(wstring wstrMessage, int *piOut);
	bool ReadPosition(IBoard *pBoard, PieceType pt, int *piPos);
	bool ReadMoveDirection(DirectionKind *pdk, MoveKind *pmk);

private:
	wstring m_wstrName;

private:
	// Disallow copy and assignment
	UserInput(const UserInput &);
	UserInput& operator=(const UserInput &);
};

class UserOutput
{
public:
	UserOutput(wstring wstrName);
	UserOutput();
	~UserOutput();

	void Write(GameKind gk);
	void Write(IBoard *pBoard);
	void Write(IBoard *pBoard, int iPos, DirectionKind dk, MoveKind mk, MoveResult mr);
	void Write(GameState gs, wstring wstrPlayer1, wstring wstrPlayer2);

private:
	wstring m_wstrName;

private:
	// Disallow copy and assignment
	UserOutput(const UserOutput &);
	UserOutput& operator=(const UserOutput &);
};
