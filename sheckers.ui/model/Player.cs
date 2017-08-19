using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Media.Imaging;

namespace sheckersui
{
    class Player
    {
        public PieceType Type { get; set; }
        public string Name { get; set; }
        public int CapturedCount { get; set; }
        public BitmapImage Img { get; set; }
        public BitmapImage CapturedPieceImg { get; set; }

        BitmapImage TurnImg { get; set; }
        BitmapImage NoTurnImg { get; set; }

        public Player(PieceType pt, string playerName)
        {
            Type = pt;
            Name = playerName;
            CapturedCount = 0;
            TurnImg = (BitmapImage)App.Current.TryFindResource("playerturn");
            NoTurnImg = (BitmapImage)App.Current.TryFindResource("playernoturn");
            BitmapImage redPieceImage = (BitmapImage)App.Current.TryFindResource("redpiece");
            BitmapImage blackPieceImage = (BitmapImage)App.Current.TryFindResource("blackpiece");
            CapturedPieceImg = (Type == PieceType.Black) ? redPieceImage : blackPieceImage;
        }

        public bool IsMyTurn
        {
            set
            {
                if (value)
                    Img = TurnImg;
                else
                    Img = NoTurnImg;
            }
        }
    }
}
