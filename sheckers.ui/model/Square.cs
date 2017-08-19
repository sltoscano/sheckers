using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Imaging;
using System.Windows.Controls;

namespace sheckersui
{
    enum SquareColor
    {
        White,
        Black
    }

    class Square
    {
        public BitmapImage Img { get { return _img; } }
        public SquareColor Color { get { return _color; } }
        public int Col { get { return _iCol; } }
        public int Row { get { return _iRow; } }
        BitmapImage _img;
        SquareColor _color;
        int _iCol;
        int _iRow;

        public Square(SquareColor color, int iCol, int iRow)
        {
            _color = color;
            _iCol = iCol;
            _iRow = iRow;

            string imgsrc = (color == SquareColor.White) ? "whitesquare" : "blacksquare";
            imgsrc += App.Rand.Next(5);
            _img = (BitmapImage)App.Current.TryFindResource(imgsrc);
        }
    }
}
