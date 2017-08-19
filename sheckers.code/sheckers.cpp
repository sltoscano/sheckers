// Copyright: Steven Toscano

#include "std.h"
#include "common.h"
#include "factories.h"
#include "memleak.h"
#include "io.h"

int main(int argc, char* argv[])
{
	UserInput input;
	UserOutput output;

	GameKind gk = gkCheckers;
	output.Write(gk);

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

	IPlayerPtr spPlayer1(PlayerFactory::Create(pkPlayer1, wstrPlayer1, ptRed, iPieceCount));
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
		input.Read();
	}

	GameState gs = spGame->GetState();
	output.Write(gs, wstrPlayer1, wstrPlayer2);
	return gs == gsError ? 1 : 0;
}
