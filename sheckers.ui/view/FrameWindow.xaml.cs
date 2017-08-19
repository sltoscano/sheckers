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
    /// Interaction logic for FrameWindow.xaml
    /// </summary>
    public partial class FrameWindow : Window
    {
        public FrameWindow()
        {
            InitializeComponent();
            App.FrameWindow = this;
            App.ShowInputPage();
        }

        private void MenuItem_Exit(object sender, RoutedEventArgs e)
        {
            App.Close();
        }

        private void MenuItem_NewGame(object sender, RoutedEventArgs e)
        {
            App.ShowInputPage();
        }
    }
}
