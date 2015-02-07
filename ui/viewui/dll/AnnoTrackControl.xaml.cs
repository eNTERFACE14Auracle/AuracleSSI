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
    /// Interaction logic for AnnoTrackControl.xaml
    /// </summary>
    public partial class AnnoTrackControl : UserControl
    {       
        public AnnoTrackControl()
        {
            InitializeComponent();
        }

        public void clear()
        {
            this.annoTrackGrid.RowDefinitions.Clear();            
            this.annoTrackGrid.Children.Clear();
        }

        public AnnoTrack addAnnoTrack(AnnoList list)
        {
            if (this.annoTrackGrid.RowDefinitions.Count > 0)
            {
                // add splitter
                RowDefinition split_row = new RowDefinition();
                split_row.Height = new GridLength(1, GridUnitType.Auto);
                this.annoTrackGrid.RowDefinitions.Add(split_row);
                GridSplitter splitter = new GridSplitter();
                splitter.ResizeDirection = GridResizeDirection.Rows;
                splitter.Height = 3;
                splitter.HorizontalAlignment = HorizontalAlignment.Stretch;
                splitter.VerticalAlignment = VerticalAlignment.Stretch;
                splitter.ShowsPreview = true;
                Grid.SetColumnSpan(splitter, 1);
                Grid.SetColumn(splitter, 0);
                Grid.SetRow(splitter, this.annoTrackGrid.RowDefinitions.Count - 1);
                this.annoTrackGrid.Children.Add(splitter);
            }

            // add anno track
            RowDefinition row = new RowDefinition();
            row.Height = new GridLength(1, GridUnitType.Star);
            this.annoTrackGrid.RowDefinitions.Add(row);
            AnnoTrack track = new AnnoTrack(list);
            Grid.SetColumn(track, 0);
            Grid.SetRow(track, this.annoTrackGrid.RowDefinitions.Count - 1);            
            this.annoTrackGrid.Children.Add(track);

            return track;
        }

    }
}
