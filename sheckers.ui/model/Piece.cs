using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Imaging;

namespace sheckersui
{
    enum PieceType
    {
        Red,
        Black
    }

    class Piece
    {
        public PieceType Type { get; set; }
        public BitmapImage Img { get; set; }
        public BitmapImage ImgWithShadow { get; set; }
        public Square Square { get; set; }

        public Piece(PieceType pt, Square square)
        {
            Type = pt;
            Square = square;
            string imgsrc;
            imgsrc = (Type == PieceType.Red) ? "redpiece" : "blackpiece";
            Img = (BitmapImage)App.Current.TryFindResource(imgsrc);
            imgsrc = (Type == PieceType.Red) ? "redpiece_shadow" : "blackpiece_shadow";
            ImgWithShadow = (BitmapImage)App.Current.TryFindResource(imgsrc);
        }
    }
}
