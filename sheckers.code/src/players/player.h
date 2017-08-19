// Copyright: Steven Toscano

#pragma once

class PlayerPieces :
	public IPlayerPieces
{
public:
	PlayerPieces();
	~PlayerPieces();

	// IPlayerPieces methods
	IPiecePtr GetNext();

	// Non interface methods
	void AddQuery(vector<IPiecePtr> &pieces);

private:
	vector<IPiecePtr> m_pieces;
	vector<IPiecePtr>::const_iterator m_iter;

private:
	// Disallow copy and assignment
	PlayerPieces(const PlayerPieces &);
	PlayerPieces& operator=(const PlayerPieces &);
};

class Player : 
	public IPlayer
{
protected:
	Player(wstring wstrName, PieceType pt, int iPieceCount, IFeedbackPtr spFeedback);
	virtual ~Player();

public:
	// IPlayer methods
	bool AddPiece(IPiece *pPiece);
	bool LosePiece(IPiece *pPiece);
	bool CapturePiece(IPiece *pPiece);
	int  GetPieceOwnedCount() const;
	int  GetPieceCapturedCount() const;
	wstring GetName() const;
	PieceType GetType() const;
	int  GetInitialPieceCount() const;
	int  GetLastMovePosition() const;
	void SetLastMovePosition(int iPos);
	IPlayerPiecesPtr GetPieces();

	// IPlayer method to be implemented by derived types
	virtual MoveResult MakeMove(IBoard *pBoard, MoveResult mrLastMove) = 0;

protected:
	vector<IPiecePtr> m_pieces;
	vector<IPiecePtr> m_capturedPieces;
	typedef vector<IPiecePtr>::const_iterator PieceIter;
	int m_iInitialPieceCount;
	int m_iLastMovePosition;
	PieceType m_pt;
	wstring m_wstrName;

private:
	// Disallow copy and assignment
	Player(const Player &);
	Player& operator=(const Player &);
};
