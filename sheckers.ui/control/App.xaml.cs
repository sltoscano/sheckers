using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Windows;

namespace sheckersui
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        internal static Random Rand
        {
            get { return _random; }
        }
        private static Random _random;

        internal static int RowSize { get; set; }
        internal static int InitialPieceCount { get; set; }
        internal static string Player1Name { get; set; }
        internal static string Player2Name { get; set; }
        internal static FrameWindow FrameWindow { get; set; }

        private static BoardControl _boardControl;
        private static InputControl _inputControl;

        internal App()
        {
            _random = new Random();
        }

        internal static void ShowBoard()
        {
            CleanUp(_boardControl);
            FrameWindow.menuItemNewGame.IsEnabled = true;
            FrameWindow.appSurface.Children.Clear();
            _boardControl = new BoardControl();
            FrameWindow.appSurface.Children.Add(_boardControl);
            FrameWindow.statusBarText.Text = "It's " + Player1Name + "'s turn";
        }

        internal static void ShowInputPage()
        {
            CleanUp(_inputControl);
            FrameWindow.menuItemNewGame.IsEnabled = false;
            FrameWindow.appSurface.Children.Clear();
            _inputControl = new InputControl();
            FrameWindow.appSurface.Children.Add(_inputControl);
            FrameWindow.statusBarText.Text = "";
        }

        internal static void Close()
        {
            FrameWindow.appSurface.Children.Clear();
            FrameWindow.Close();
        }

        internal static void CleanUp(object obj)
        {
            if (obj != null)
            {
                if (obj is IDisposable)
                {
                    ((IDisposable)obj).Dispose();
                }
                obj = null;
                GC.Collect();
                GC.WaitForPendingFinalizers();
                GC.Collect();
            }
        }
    }
}
