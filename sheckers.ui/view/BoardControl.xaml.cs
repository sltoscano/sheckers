using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using System.Media;
using System.IO;

namespace sheckersui
{
    /// <summary>
    /// Interaction logic for BoardControl.xaml
    /// </summary>
    public partial class BoardControl : UserControl, IDisposable
    {
        Board _boardData;
        Player _player1Data;
        Player _player2Data;
        PlayerControl _player1Control;
        PlayerControl _player2Control;
        Dictionary<int, PieceControl> _pieceControls;
        Dictionary<int, SquareControl> _squareControls;
        public static List<SoundPlayer> PieceDropSounds;
        public static List<SoundPlayer> PieceAutoMoveSounds;

        public BoardControl()
        {
            InitializeComponent();
            InitBoard();
            InitSounds();

            if (App.FrameWindow.FindName(_boardGrid.Name) == null)
                App.FrameWindow.RegisterName(_boardGrid.Name, _boardGrid);

            LayoutUpdated += new EventHandler(boardControl_LayoutUpdated);
            Loaded += new RoutedEventHandler(boardControl_Loaded);

            // Hook the drag and drop events for moving pieces
            DragDropHelper.ItemDragStart += new EventHandler<DragDropEventArgs>(DragDropHelper_ItemDragStart);
            DragDropHelper.ItemDragEnd += new EventHandler<DragDropEventArgs>(DragDropHelper_ItemDragEnd);
            DragDropHelper.ItemDropped += new EventHandler<DragDropEventArgs>(DragDropHelper_ItemDropped);
            DragDropHelper.DropAnimationDone += new EventHandler<DragDropEventArgs>(DragDropHelper_DropAnimationDone);
        }

        void InitBoard()
        {
            // Create a dictionary of the UIElements on the board for O(1) lookup
            _pieceControls = new Dictionary<int, PieceControl>();
            _squareControls = new Dictionary<int, SquareControl>();

            // Setup the players
            _player1Data = new Player(PieceType.Black, App.Player1Name);
            _player2Data = new Player(PieceType.Red, App.Player2Name);

            // Setup the turn for the player
            _player1Data.IsMyTurn = true;
            _player2Data.IsMyTurn = false;

            // Add players to the board
            _player1Control = new PlayerControl();
            _player1Control.DataContext = _player1Data;
            _player1Panel.Children.Add(_player1Control);
            _player2Control = new PlayerControl();
            _player2Control.DataContext = _player2Data;
            _player2Panel.Children.Add(_player2Control);

            // Init the data model
            _boardData = new Board(App.InitialPieceCount, App.RowSize, App.RowSize);

            // Draw the board grid
            for (int i = 0; i < App.RowSize; i++)
            {
                ColumnDefinition colDef = new ColumnDefinition();
                colDef.Width = new GridLength(1, GridUnitType.Star);
                _boardGrid.ColumnDefinitions.Add(colDef);
                RowDefinition rowDef = new RowDefinition();
                rowDef.Height = new GridLength(1, GridUnitType.Star);
                _boardGrid.RowDefinitions.Add(rowDef);
            }

            // Fill in the board grid with data model
            foreach (var item in _boardData.Squares)
            {
                BoardOperations.AddSquare(_boardGrid, item, _squareControls);
            }

            // Give pieces to the players
            foreach (var piece in _boardData.Player1Pieces)
            {
                CreatePiece(piece, _player1Control);
            }
            foreach (var piece in _boardData.Player2Pieces)
            {
                CreatePiece(piece, _player2Control);
            }
        }

        private void CreatePiece(Piece piece, PlayerControl playerControl)
        {
            PieceControl pieceControl = new PieceControl();
            pieceControl.Uid = "piece";
            pieceControl.DataContext = piece;

            // Z - order bug in WPF's Grid so have to start on the board
            //Grid.SetColumn(pieceControl, 0);
            //Grid.SetRow(pieceControl, 1);
            //playerControl.PlayerHUD.Children.Add(pieceControl);

            // Initially place piece at the corner of the board
            // later the pieces will slide to the right positions
            if (piece.Type == PieceType.Black)
            {
                Grid.SetColumn(pieceControl, 0);
                Grid.SetRow(pieceControl, 0);
            }
            else
            {
                Grid.SetColumn(pieceControl, App.RowSize - 1);
                Grid.SetRow(pieceControl, App.RowSize-1);
            }
            _boardGrid.Children.Add(pieceControl);

            int iPos = BoardOperations.GetPos(piece.Square.Col, piece.Square.Row);
            _pieceControls[iPos] = pieceControl;
        }

        private void InitSounds()
        {
            // Initialize the sounds once per app instance
            if (BoardControl.PieceDropSounds == null)
            {
                BoardControl.PieceDropSounds = new List<SoundPlayer>();
                foreach (var wav in Directory.GetFiles(AppDomain.CurrentDomain.BaseDirectory, "piecedrop*.wav"))
                {
                    SoundPlayer soundPlayer = new SoundPlayer();
                    soundPlayer.SoundLocation = wav;
                    soundPlayer.LoadAsync();
                    BoardControl.PieceDropSounds.Add(soundPlayer);
                }
            }
            if (BoardControl.PieceAutoMoveSounds == null)
            {
                BoardControl.PieceAutoMoveSounds = new List<SoundPlayer>();
                foreach (var wav in Directory.GetFiles(AppDomain.CurrentDomain.BaseDirectory, "pieceslide*.wav"))
                {
                    SoundPlayer soundPlayer = new SoundPlayer();
                    soundPlayer.SoundLocation = wav;
                    soundPlayer.LoadAsync();
                    BoardControl.PieceAutoMoveSounds.Add(soundPlayer);
                }
            }
        }

        void boardControl_Loaded(object sender, RoutedEventArgs e)
        {
            // Now add each players pieces to the board from data model
            foreach (var pieceControl in _pieceControls)
            {
                PieceControl control = pieceControl.Value;
                Piece piece = pieceControl.Value.DataContext as Piece;
                Player player1 = _player1Control.DataContext as Player;
                if (piece.Type == player1.Type)
                    BoardOperations.AddPiece(_boardGrid, _player1Control, control, _squareControls);
                else
                    BoardOperations.AddPiece(_boardGrid, _player2Control, control, _squareControls);
            }
        }

        void DragDropHelper_ItemDragStart(object sender, DragDropEventArgs e)
        {
            // The user is dragging a piece, highlight legal moves here
            throw new NotImplementedException();
        }

        void DragDropHelper_ItemDragEnd(object sender, DragDropEventArgs e)
        {
            // The user stopped dragging, stop highlights
            throw new NotImplementedException();
        }       

        void DragDropHelper_ItemDropped(object target, DragDropEventArgs source)
        {
            // The user just let go of the mouse button during a drag of a piece
            // this means the move should be committed
            if (target == null ||
                source == null)
            {
                Debug.Assert(false, "Invalid argument");
                return;
            }

            Square squareDataTarget = target as Square;
            Piece pieceDataSource = source.Content as Piece;

            int iOldPos = BoardOperations.GetPos(pieceDataSource.Square.Col, pieceDataSource.Square.Row);

            if (!_pieceControls.ContainsKey(iOldPos))
            {
                Debug.Assert(false, "No piece found at position: " + iOldPos);
                return;
            }

            PieceControl pieceToMove = _pieceControls[iOldPos];
            if (pieceToMove == null)
            {
                Debug.Assert(false, "Null piece found at position: " + iOldPos);
                return;
            }

            // Hide the piece during the move to its new position
            pieceToMove.Opacity = 0;
            BoardOperations.MovePiece(pieceDataSource, squareDataTarget, _pieceControls);
        }

        void DragDropHelper_DropAnimationDone(object target, DragDropEventArgs e)
        {
            // The animation of the piece moving to its final position
            // just finished, update any visuals as necessary
            Square squareDataTarget = target as Square;

            int iPos = BoardOperations.GetPos(squareDataTarget.Col, squareDataTarget.Row);

            if (!_pieceControls.ContainsKey(iPos))
            {
                Debug.Assert(false, "No piece found at position: " + iPos);
                return;
            }

            PieceControl pieceMoved = _pieceControls[iPos];
            if (pieceMoved == null)
            {
                Debug.Assert(false, "Null piece found at position: " + iPos);
                return;
            }

            // Unhide the piece after move to the new position
            pieceMoved.Opacity = 1;

            // Play a sound when the user moves a piece into a new position
            if (BoardControl.PieceDropSounds.Count > 0)
            {
                int index = App.Rand.Next(BoardControl.PieceDropSounds.Count);
                if (BoardControl.PieceDropSounds[index].IsLoadCompleted)
                    BoardControl.PieceDropSounds[index].Play();
            }
        }

        void boardControl_LayoutUpdated(object sender, EventArgs e)
        {
            // This is required everytime the board is resized to avoid
            // leaving space between the squares
            if (_boardGrid == null || _boardGrid.Children == null)
                return;

            // Scale the pieces down to fit inside of the squares
            double width = _boardGrid.ColumnDefinitions[0].ActualWidth * 0.85;
            double height = _boardGrid.RowDefinitions[0].ActualHeight * 0.85;

            foreach (UIElement element in _boardGrid.Children)
            {
                if (element.Uid == "piece")
                {
                    PieceControl piece = element as PieceControl;
                    piece.Width = width;
                    piece.Height = height;
                }
            }
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                _boardGrid.RowDefinitions.Clear();
                _boardGrid.ColumnDefinitions.Clear();
                _boardGrid.Children.Clear();
                LayoutUpdated -= new EventHandler(boardControl_LayoutUpdated);
                Loaded -= new RoutedEventHandler(boardControl_Loaded);
                DragDropHelper.ItemDropped -= new EventHandler<DragDropEventArgs>(DragDropHelper_ItemDropped);
                DragDropHelper.DropAnimationDone -= new EventHandler<DragDropEventArgs>(DragDropHelper_DropAnimationDone);
                DragDropHelper.Reset();
            }
        }
    }
}
