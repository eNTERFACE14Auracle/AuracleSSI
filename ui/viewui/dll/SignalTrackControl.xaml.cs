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
    /// Interaction logic for SignalTrackControl.xaml
    /// </summary>
    public partial class SignalTrackControl : UserControl
    {
        public SignalTrackControl()
        {
            InitializeComponent();
        }

        public void clear()
        {
            this.signalTrackGrid.RowDefinitions.Clear();
            this.signalTrackGrid.Children.Clear();
        }

        public ISignalTrack addSignalTrack(Signal signal)
        {
            if (this.signalTrackGrid.RowDefinitions.Count > 0)
            {
                // add splitter
                RowDefinition split_row = new RowDefinition();
                split_row.Height = new GridLength(1, GridUnitType.Auto);
                this.signalTrackGrid.RowDefinitions.Add(split_row);
                GridSplitter splitter = new GridSplitter();
                splitter.ResizeDirection = GridResizeDirection.Rows;
                splitter.Height = 3;
                splitter.HorizontalAlignment = HorizontalAlignment.Stretch;
                splitter.VerticalAlignment = VerticalAlignment.Stretch;
                splitter.ShowsPreview = true;
                Grid.SetColumnSpan(splitter, 1);
                Grid.SetColumn(splitter, 0);
                Grid.SetRow(splitter, this.signalTrackGrid.RowDefinitions.Count - 1);
                this.signalTrackGrid.Children.Add(splitter);
            }

            // add signal track
            RowDefinition row = new RowDefinition();
            row.Height = new GridLength(1, GridUnitType.Star);
            this.signalTrackGrid.RowDefinitions.Add(row);
            SignalTrack track = new SignalTrack(signal);
            SignalTrackEx trackex = new SignalTrackEx();
            trackex.AddTrack(track);
            Grid.SetColumn(trackex, 0);
            Grid.SetRow(trackex, this.signalTrackGrid.RowDefinitions.Count - 1);
            this.signalTrackGrid.Children.Add(trackex);

            return track;
        }

    }
}
