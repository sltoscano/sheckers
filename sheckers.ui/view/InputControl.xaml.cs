using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace sheckersui
{
    /// <summary>
    /// Interaction logic for InputControl.xaml
    /// </summary>
    public partial class InputControl : UserControl
    {
        public InputControl()
        {
            InitializeComponent();
        }

        private void startButton_Click(object sender, RoutedEventArgs e)
        {
            App.RowSize = Int32.Parse(textBoxRowSize.Text);
            App.InitialPieceCount = Int32.Parse(textBoxInitialPieceCount.Text);

            if (App.RowSize <= 0)
                App.RowSize = 1;

            if (App.InitialPieceCount <= 0)
                App.InitialPieceCount = 1;

            if ((App.InitialPieceCount * 2 * 2) > (App.RowSize * App.RowSize))
                App.InitialPieceCount = (App.RowSize * App.RowSize) / 4;

            App.Player1Name = player1Name.Text;
            App.Player2Name = player2Name.Text;
            App.ShowBoard();
        }
    }
}
