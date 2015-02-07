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

namespace ssi
{
    /// <summary>
    /// Interaction logic for SignalTrack.xaml
    /// </summary>
    public partial class SignalTrackEx : UserControl
    {
        SignalTrack track = null;

        public SignalTrackEx()
        {
            InitializeComponent();
        }

        public void AddTrack(SignalTrack track)
        {
            Grid.SetColumn(track, 0);
            Grid.SetRow(track, 1);

            for (int i = 0; i < track.getSignal().dim; i++ )
            {
                DimComboBox.Items.Add(i+1);
            }
            DimComboBox.SelectedItem = 0;

            this.grid.Children.Add(track);
            this.track = track;
        }

        private void ZoomInButton_Click(object sender, RoutedEventArgs e)
        {
            if (track != null)
            {
                if (Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl))
                {
                    track.zoom(3, 0);
                }
                else
                {
                    track.zoom(1, 0);
                }                
            }
        }

        private void ZoomOutButton_Click(object sender, RoutedEventArgs e)
        {
            if (track != null)
            {
                if (Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl))
                {
                    track.zoom(-3, 0);
                }
                else
                {
                    track.zoom(-1, 0);
                }
            }
        }

        private void MoveUpButton_Click(object sender, RoutedEventArgs e)
        {
            if (track != null)
            {
                if (Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl))
                {
                    track.zoom(0, -6);
                }
                else
                {
                    track.zoom(0, -1);
                }
            }
        }

        private void MoveDownButton_Click(object sender, RoutedEventArgs e)
        {
            if (track != null)
            {
                if (Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl))
                {
                    track.zoom(0, 6);
                }
                else
                {
                    track.zoom(0, 1);
                }
            }
        }

        private void ResetButton_Click(object sender, RoutedEventArgs e)
        {
            if (track != null)
            {
                track.zoomReset();
            }
        }

        private void ColorButton_Click(object sender, RoutedEventArgs e)
        {
            if (track != null)
            {
                track.changeColour();
            }
        }

        private void DimComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (track != null)
            {                
                uint index = (uint) DimComboBox.SelectedIndex;
                track.getSignal().ShowDim = index;
                track.InvalidateVisual();                
            }
        }

    }
}
