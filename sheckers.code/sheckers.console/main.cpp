// Copyright: Steven Toscano

/*
#include "std.h"
#include "common.h"
#include "memleak.h"
#include "io.h"
#include "factories.h"
*/

#include <memory>
using namespace std::tr1;

shared_ptr<char*> foo;
//#include "ui.h"

#undef AUTOMATED
extern "C" void __stdcall Sleep(int dwMilliseconds);

int main(int argc, char* argv[])
{
	UserInput input;
	UserOutput output;

	GameKind gk = gkCheckers;
	output.Write(gk);

#ifdef AUTOMATED
	int iBoardRowSize = 8;
	int iPieceCount = 12;
	wstring wstrPlayer1 = L"Fred";
	PlayerKind pkPlayer1 = pkComputerHard;
	wstring wstrPlayer2 = L"George";
	PlayerKind pkPlayer2 = pkComputerEasy;
#else
	int iBoardRowSize;
	if (!input.ReadBoardRowSize(&iBoardRowSize))
		return 1;

	int iPieceCount;
	if (!input.ReadPieceCount(iBoardRowSize, &iPieceCount))
		return 1;

	input.Clear();

	wstring wstrPlayer1;
	PlayerKind pkPlayer1;
	if (!input.ReadPlayer(&wstrPlayer1, &pkPlayer1))
		return 1;

	input.Clear();

	wstring wstrPlayer2;
	PlayerKind pkPlayer2;
	if (!input.ReadPlayer(&wstrPlayer2, &pkPlayer2))
		return 1;
#endif

	IPlayerPtr spPlayer1(PlayerFactory::Create(pkPlayer1, wstrPlayer1, ptRed, iPieceCount), PlayerFactory::Destroy);
	IPlayerPtr spPlayer2(PlayerFactory::Create(pkPlayer2, wstrPlayer2, ptBlack, iPieceCount));

	IBoardPtr spBoard(BoardFactory::Create(gk, iBoardRowSize, iBoardRowSize));
	IGamePtr spGame(GameFactory::Create(gk, spBoard));
	spGame->Start(spPlayer1, spPlayer2);

	MoveResult mr = mrNone;
	while (!spGame->IsOver())
	{
		input.Clear();
		output.Write(spBoard);
		mr = spGame->DoTurn(mr);
#ifdef AUTOMATED
		Sleep(100);
#else
		input.Read();
#endif
	}

	GameState gs = spGame->GetState();
	input.Clear();
	output.Write(spBoard);
	output.Write(gs, wstrPlayer1, wstrPlayer2);

	int retval = 0;
	switch(gs)
	{
	case gsError:
		retval = 1;
		break;
	case gsForfeit:
		retval = 2;
		break;
	case gsPlayer1Wins:
		retval = 3;
		break;
	case gsPlayer2Wins:
		retval = 4;
		break;
	case gsStalemate:
		retval = 5;
		break;
	}
	return retval;
}
