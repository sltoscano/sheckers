using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;

namespace sheckersui
{
    class Board
    {
        ObservableCollection<Piece> _player1pieces;
        ObservableCollection<Piece> _player2pieces;
        ObservableCollection<Square> _squares;

        public int ColSize { get; set; }
        public int RowSize { get; set; }
        
        public Board(int iInitialPieceCount, int iColSize, int iRowSize)
        {
            ColSize = iColSize;
            RowSize = iRowSize;

            _squares = new ObservableCollection<Square>();

            int turn = 0;
            for (int i = 0; i < iRowSize; i++)
            {
                turn ^= 1;
                for (int j = 0; j < iColSize; j++)
                {
                    turn ^= 1;
                    SquareColor color = (turn == 1) ? SquareColor.Black : SquareColor.White;
                    _squares.Add(new Square(color, j, i));
                }
            }

            _player1pieces = new ObservableCollection<Piece>();
            _player2pieces = new ObservableCollection<Piece>();

            int iStartFillPosition = 0;
            int iPlacedCount = 0;
            for (int i = iStartFillPosition; i < _squares.Count; i++)
            {
                if (_squares[i].Color == SquareColor.White)
                {
                    _player1pieces.Add(new Piece(PieceType.Black, _squares[i]));
                    iPlacedCount++;
                    if (iPlacedCount == iInitialPieceCount)
                        break;
                }
            }
            iStartFillPosition = _squares.Count - (iInitialPieceCount * 2);
            iPlacedCount = 0;
            for (int i = iStartFillPosition; i < _squares.Count; i++)
            {
                if (_squares[i].Color == SquareColor.White)
                {
                    _player2pieces.Add(new Piece(PieceType.Red, _squares[i]));
                    iPlacedCount++;
                    if (iPlacedCount == iInitialPieceCount)
                        break;
                }
            }
        }

        public ObservableCollection<Square> Squares
        {
            get { return _squares; }
        }

        public ObservableCollection<Piece> Player1Pieces
        {
            get { return _player1pieces; }
        }

        public ObservableCollection<Piece> Player2Pieces
        {
            get { return _player2pieces; }
        }
    }
}
