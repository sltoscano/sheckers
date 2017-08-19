// Copyright: Steven Toscano

#include "std.h"
#include "common.h"

#include <iostream>
#include <sstream>

#include "io.h"

UserInput::UserInput(wstring wstrName) :
	m_wstrName(wstrName)
{
}

UserInput::UserInput()
{
}

UserInput::~UserInput()
{
}

void UserInput::Clear()
{
	system("cls");
}

void UserInput::Read()
{
	wcin.ignore(255, L'\n');
	wcin.putback(L'\n');
	wcin.get();
}

bool UserInput::ReadBoardRowSize(int *piBoardRowSize)
{
	while (true)
	{
		wstringstream wstrMessage;
		wstrMessage << L"Please type in the desired board row size." << endl;
		wstrMessage << L"For example for a 64 square board, type a row size of 8, or 'q' to quit: ";
		int iRowSize;
		if (!UserInput::ReadInt(wstrMessage.str(), &iRowSize))
			break;
		if (iRowSize < 2)
		{
			wcout << L"Board row size cannot be less than 2." << endl;
			Read();
		}
		else if (iRowSize > 1000)
		{
			wcout << L"Board row size over 1000 not supported." << endl;
			Read();
		}
		else
		{
			*piBoardRowSize = iRowSize;
			return true;
		}
	}
	return false;
}

bool UserInput::ReadPieceCount(int iBoardRowSize, int *piPieceCount)
{
	while (true)
	{
		wstringstream wstrMessage;
		wstrMessage << L"Please type in the desired player piece count." << endl;
		wstrMessage << L"For example on a typical 8x8 board each player will have 12 pieces, or 'q' to quit: ";
		int iPieceCount;
		if (!UserInput::ReadInt(wstrMessage.str(), &iPieceCount))
			break;
		if (iPieceCount * 2 * 2 > iBoardRowSize * iBoardRowSize)
		{
			wcout << L"Piece count will exceed size of board, please try again." << endl;
			Read();
		}
		else if (iPieceCount < 1)
		{
			wcout << L"Piece count cannot be less than 1." << endl;
			Read();
		}
		else
		{
			*piPieceCount = iPieceCount;
			return true;
		}
	}
	return false;
}

bool UserInput::ReadPlayer(wstring *pwstrPlayer, PlayerKind *ppkPlayer)
{
	while (true)
	{
		wstringstream wstrMessage;
		wstrMessage << L"Please choose the type of the next player." << endl;
		wstrMessage << L"\t'0' for hard computer player" << endl; 
		wstrMessage << L"\t'1' for easy computer player" << endl;
		wstrMessage << L"\t'2' for human player, or 'q' to quit: ";
		int iPlayer = 0;
		if (!UserInput::ReadInt(wstrMessage.str(), &iPlayer))
			break;
		if (iPlayer != 0 && iPlayer != 1 && iPlayer != 2)
		{
			wcout << L"Not a valid player kind." << endl;
			Read();
		}
		else
		{
			switch(iPlayer)
			{
			case 0:
				*ppkPlayer = pkComputerHard;
				break;
			case 1:
				*ppkPlayer = pkComputerEasy;
				break;
			case 2:
				*ppkPlayer = pkHuman;
				break;
			}
			wcout << L"\tType in a name for this player: ";
			wcin.ignore(255, L'\n');
			wchar_t wszPlayer[20];
			wcin.get(wszPlayer, 20);
			wcin.ignore(255, L'\n');
			*pwstrPlayer = wstring((wchar_t*)wszPlayer);
			return true;
		}
	}
	return false;
}

bool UserInput::ReadMove(IBoard *pBoard, PieceType pt, int *piPos, DirectionKind *pdk, MoveKind *pmk)
{
	if (ReadPosition(pBoard, pt, piPos))
		if (ReadMoveDirection(pdk, pmk))
			return true;
	return false;
}

bool UserInput::ListLegalMoves()
{
	wchar_t cValue;
	wcout << L"Move entered is not valid." << endl;
	wcout << L"\tList legal moves? 'y' or 'n': ";
	wcin >> cValue;
	
	return (cValue == L'y') ? true : false;
}

bool UserInput::ReadInt(wstring wstrMessage, int *piOut)
{
	while (true)
	{
		wchar_t cValue = 0;
		int iValue = 0;
		wcout << wstrMessage.c_str();
		wcin >> iValue;
		if (wcin.fail())
		{
			wcin.clear();
			wcin >> cValue;
			if (cValue == L'q')
			{
				wcout << L"quitting." << endl;
				break;
			}
			else
			{
				wcout << L"invalid entry, try again." << endl;
				Read();
			}
		}
		else
		{
			*piOut = iValue;
			return true;
		}
	}

	return false;
}

bool UserInput::ReadPosition(IBoard *pBoard, PieceType pt, int *piPos)
{
	IBoardPtr spBoard(pBoard);
	FAILED_ASSERT_RETURN(false, spBoard != NULL);
	int iColEnd = spBoard->GetColSize()-1;
	int iRowEnd = spBoard->GetRowSize()-1;
	while (true)
	{
		wstringstream wstrMessage;
		if (!m_wstrName.empty())
		{
			wstrMessage << m_wstrName.c_str() << L" ";
		}
		wstrMessage << L"type in the column number of the (";
		wstrMessage << (pt == ptRed ? L"0" : L"X") << L") piece to move." << endl;
		wstrMessage << L"\tMust be between 0 and " << iColEnd << L", or 'q' to quit: ";
		int iCol = 0;
		if (!UserInput::ReadInt(wstrMessage.str(), &iCol))
			break;
		if (iCol >= 0 && iCol <= iColEnd)
		{
			wstrMessage.str(L"");
			wstrMessage << L"Type in the row number of the piece to move." << endl;
			wstrMessage << L"\tMust be between 0 and " << iRowEnd << L", or 'q' to quit: ";
			int iRow = 0;
			if (!UserInput::ReadInt(wstrMessage.str(), &iRow))
				break;
			if (iRow >= 0 && iRow <= iRowEnd)
			{
				*piPos = (iRow * spBoard->GetRowSize()) + iCol;
				return true;
			}
			else
			{
				wcout << L"Not a valid row on the board." << endl;
				Read();
			}
		}
		else
		{
			wcout << L"Not a valid column on the board." << endl;
			Read();
		}
	}
	return false;
}

bool UserInput::ReadMoveDirection(DirectionKind *pdk, MoveKind *pmk)
{
	while (true)
	{
		wstringstream wstrMessage;
		if (!m_wstrName.empty())
		{
			wstrMessage << m_wstrName.c_str() << L" ";
		}
		wstrMessage << L"type in the number for your move selection." << endl;
		wstrMessage << L"\tOptions are up-left '0', up-right '1', " << endl;
		wstrMessage << L"\tdown-left '2', down-right '3' or 'q' to quit: ";
		int iMoveSelection;
		if (!UserInput::ReadInt(wstrMessage.str(), &iMoveSelection))
			break;
		switch (iMoveSelection)
		{
		case 0:
			*pdk = dkUp;
			*pmk = mkLeft;
			return true;
		case 1:
			*pdk = dkUp;
			*pmk = mkRight;
			return true;
		case 2:
			*pdk = dkDown;
			*pmk = mkLeft;
			return true;
		case 3:
			*pdk = dkDown;
			*pmk = mkRight;
			return true;
		}
		wcout << L"Not a valid move selection." << endl;
		Read();
	}
	return false;
}

UserOutput::UserOutput(wstring wstrName) :
	m_wstrName(wstrName)
{
}

UserOutput::UserOutput()
{
}

UserOutput::~UserOutput()
{
}

void UserOutput::Write(GameKind gk)
{
	switch (gk)
	{
	case gkCheckers:
		cout << "Welcome to sheckers!" << endl;
		break;
	default:
		cout << "Game kind not supported." << endl;
		break;
	}
}

void UserOutput::Write(IBoard *pBoard)
{
	IBoardPtr spBoard(pBoard);
	FAILED_ASSERT_RETURN(, spBoard != NULL);

	int iRowSize = spBoard->GetRowSize();
	for (int i = 0; i < spBoard->GetBoardSize(); i++)
	{
		if (i % iRowSize == 0)
		{
			wcout << endl;
		}
		if (spBoard->IsOccupied(i))
		{
			IPiecePtr spPiece;
			spBoard->GetPiece(i, &spPiece);
			PieceType pt;
			spPiece->GetType(&pt);
			if (pt == ptRed)
				wcout << L"0";
			else
				wcout << L"X";
		}
		else
		{
			wcout << L".";
		}
		wcout << L" ";
	}
	wcout << endl;
}

void UserOutput::Write(IBoard *pBoard, int iPos, DirectionKind dk, MoveKind mk, MoveResult mr)
{
	wstring wstrMovePrefix;
	if (!m_wstrName.empty())
	{
		wcout << m_wstrName.c_str() << L" ";
		wstrMovePrefix = L"attempting ";
	}

	if (mr == mrNone)
	{
		wcout << L"has no more legal moves." << endl;
	}
	else
	{
		IBoardPtr spBoard(pBoard);
		FAILED_ASSERT_RETURN(, spBoard != NULL);
		int iRow = iPos / spBoard->GetRowSize();
		int iCol = iPos - (iRow * spBoard->GetRowSize());
		wcout << wstrMovePrefix << (mr == mrLegalJump ? L"jump" : L"non-jump") << 
			L" " << (dk == dkDown ? L"down" : L"up") << L"-" << 
			(mk == mkLeft ? L"left" : L"right") <<  L" move from " << iCol <<
			L"," << iRow << L"." << endl;
	}
}

void UserOutput::Write(GameState gs, wstring wstrPlayer1, wstring wstrPlayer2)
{
	switch(gs)
	{
	case gsError:
		wcout << L"An internal error occured." << endl;
		break;
	case gsForfeit:
		wcout << L"Game has been forfeited." << endl;
		break;
	case gsPlayer1Wins:
		wcout << wstrPlayer1.c_str() << L" wins!" << endl;
		break;
	case gsPlayer2Wins:
		wcout << wstrPlayer2.c_str() << L" wins!" << endl;
		break;
	case gsStalemate:
		wcout << L"No legal moves, stalemate." << endl;
		break;
	default:
		wcout << L"Invalid game state." << endl;
		break;
	}
}
