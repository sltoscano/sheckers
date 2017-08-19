using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Controls;
using System.Diagnostics;
using System.Windows;
using System.Windows.Media.Animation;

namespace sheckersui
{
    class BoardOperations
    {
        public static int GetPos(int iCol, int iRow)
        {
            return (iRow * App.RowSize) + iCol;
        }

        public static void AddSquare(Grid boardGrid, Square squareData, 
            Dictionary<int, SquareControl> squareControls)
        {
            if (boardGrid == null ||
                squareData == null ||
                squareControls == null)
            {
                Debug.Assert(false, "Invalid argument");
                return;
            }

            SquareControl squareControl = new SquareControl();
            // Set the data binding to the Piece data type
            squareControl.DataContext = squareData;
            squareControl.Uid = "square";
            Grid.SetColumn(squareControl, squareData.Col);
            Grid.SetRow(squareControl, squareData.Row);
            boardGrid.Children.Add(squareControl);
            int iPos = GetPos(squareData.Col, squareData.Row);
            squareControls[iPos] = squareControl;
            if (squareData.Color == SquareColor.White)
            {
                squareControl.AllowDrop = true;
            }
        }

        private static void RemovePiece(Grid boardGrid, Piece pieceData,
            Dictionary<int, PieceControl> pieceControls)
        {
            if (boardGrid == null ||
                pieceData == null ||
                pieceControls == null)
            {
                Debug.Assert(false, "Invalid argument");
                return;
            }

            int iPos = GetPos(pieceData.Square.Col, pieceData.Square.Row);
            if (!pieceControls.ContainsKey(iPos))
            {
                Debug.Assert(false, "No piece found at position: " + iPos);
                return;
            }

            PieceControl pieceToRemove = pieceControls[iPos];
            if (pieceToRemove == null)
            {
                Debug.Assert(false, "Null piece found at position: " + iPos);
                return;
            }

            // Remove the piece from the board grid
            pieceToRemove.Uid = "";
            boardGrid.Children.Remove(pieceToRemove);

            // Update the piece control dictionary
            pieceControls.Remove(iPos);
        }

        public static void MovePiece(Piece pieceDataSource, Square squareDataTarget,
            Dictionary<int, PieceControl> pieceControls)
        {
            if (pieceDataSource == null ||
                squareDataTarget == null ||
                pieceControls == null)
            {
                Debug.Assert(false, "Invalid argument");
                return;
            }

            int iOldPos = GetPos(pieceDataSource.Square.Col, pieceDataSource.Square.Row);
            int iNewPos = GetPos(squareDataTarget.Col, squareDataTarget.Row);

            if (!pieceControls.ContainsKey(iOldPos))
            {
                Debug.Assert(false, "No piece found at position: " + iOldPos);
                return;
            }

            PieceControl pieceToMove = pieceControls[iOldPos];
            if (pieceToMove == null)
            {
                Debug.Assert(false, "Null piece found at position: " + iOldPos);
                return;
            }

            // Move the piece to the new board grid position
            Grid.SetColumn(pieceToMove, squareDataTarget.Col);
            Grid.SetRow(pieceToMove, squareDataTarget.Row);

            // Update the data binding
            pieceDataSource.Square = squareDataTarget;
            pieceToMove.DataContext = pieceDataSource;

            // Update the piece control dictionary
            pieceControls.Remove(iOldPos);
            pieceControls[iNewPos] = pieceToMove;
        }

        public static void AutoMovePiece(PieceControl pieceToMove, SquareControl squareTarget,
            Dictionary<int, PieceControl> pieceControls)
        {
            // This method is for the computer to use, its an automatic
            // sliding of a piece to its destination (as opposed to manually
            // moving it with the mouse)
            if (pieceToMove == null || squareTarget == null)
            {
                Debug.Assert(false, "Invalid argument");
                return;
            }

            Storyboard sb = SetupMoveAnimation("autoMove", pieceToMove, squareTarget);
            sb.Completed += (s, args) =>
            {
                MovePiece(pieceToMove.DataContext as Piece,
                    squareTarget.DataContext as Square, pieceControls);
            };

            // Play a sound for sliding a piece
            if (BoardControl.PieceAutoMoveSounds.Count > 0)
            {
                int index = App.Rand.Next(BoardControl.PieceAutoMoveSounds.Count);
                if (BoardControl.PieceAutoMoveSounds[index].IsLoadCompleted)
                    BoardControl.PieceAutoMoveSounds[index].Play();
            }

            sb.Begin(pieceToMove);
        }

        public static void RemovePiece(Grid boardGrid, PieceControl pieceToMove,
            PlayerControl playerTarget, Dictionary<int, PieceControl> pieceControls)
        {
            // This method is for use when a piece goes away
            if (pieceToMove == null || playerTarget == null)
            {
                Debug.Assert(false, "Invalid argument");
                return;
            }

            Storyboard sb = SetupMoveAnimation("autoMove", pieceToMove, playerTarget);
            sb.Completed += (s, args) =>
            {
                RemovePiece(boardGrid, pieceToMove.DataContext as Piece, pieceControls);
            };

            // Play a sound for sliding a piece
            if (BoardControl.PieceAutoMoveSounds.Count > 0)
            {
                int index = App.Rand.Next(BoardControl.PieceAutoMoveSounds.Count);
                if (BoardControl.PieceAutoMoveSounds[index].IsLoadCompleted)
                    BoardControl.PieceAutoMoveSounds[index].Play();
            }

            sb.Begin(pieceToMove);
        }

        public static void AddPiece(Grid boardGrid, PlayerControl playerSource,
            PieceControl pieceControl, Dictionary<int, SquareControl> squareControls)
        {
            // This method is for use when a piece is introduced to the board
            if (boardGrid == null ||
                playerSource == null ||
                pieceControl == null)
            {
                Debug.Assert(false, "Invalid argument");
                return;
            }

            // Setup the animation
            Piece pieceData = pieceControl.DataContext as Piece;
            int iPos = GetPos(pieceData.Square.Col, pieceData.Square.Row);
            SquareControl target = squareControls[iPos];

            Storyboard sb = SetupMoveAnimation("autoMove", pieceControl, target);
            
            DoubleAnimation aniX = sb.Children[0] as DoubleAnimation;
            aniX.Duration = new Duration(TimeSpan.FromMilliseconds(1500));
            DoubleAnimation aniY = sb.Children[1] as DoubleAnimation;
            aniY.Duration = new Duration(TimeSpan.FromMilliseconds(1500));
            
            sb.Completed += (s, args) =>
            {
                // Z-order bug
                //playerSource.PlayerHUD.Children.Remove(pieceControl);
                Grid.SetColumn(pieceControl, pieceData.Square.Col);
                Grid.SetRow(pieceControl, pieceData.Square.Row);
                //boardGrid.Children.Add(pieceControl);
            };

            // Play a sound for sliding a piece
            if (BoardControl.PieceAutoMoveSounds.Count > 0)
            {
                int index = App.Rand.Next(BoardControl.PieceAutoMoveSounds.Count);
                if (BoardControl.PieceAutoMoveSounds[index].IsLoadCompleted)
                    BoardControl.PieceAutoMoveSounds[index].Play();
            }

            sb.Begin(pieceControl);
        }

        private static Storyboard SetupMoveAnimation(string animationName, 
            UserControl uiSource, UIElement uiTarget)
        {
            // This code will setup animation for a move of a UI element from source to target
            Point mid = new Point(uiSource.RenderSize.Width / 2, uiSource.RenderSize.Height / 2);
            Point source = uiSource.TranslatePoint(mid, App.FrameWindow);
            mid = new Point(uiTarget.RenderSize.Width / 2, uiTarget.RenderSize.Width / 2);
            Point target = uiTarget.TranslatePoint(mid, App.FrameWindow);
            Point destination = new Point(target.X - source.X, target.Y - source.Y);

            Storyboard sb = uiSource.Resources[animationName] as Storyboard;
            DoubleAnimation aniX = sb.Children[0] as DoubleAnimation;
            aniX.To = destination.X;
            DoubleAnimation aniY = sb.Children[1] as DoubleAnimation;
            aniY.To = destination.Y;
            return sb;
        }
    }
}
