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
    /// Interaction logic for MediaControl.xaml
    /// </summary>
    public partial class MediaControl : UserControl
    {
        public MediaControl()
        {
            InitializeComponent();
        }

        public void clear()
        {
            this.audioGrid.RowDefinitions.Clear();
            this.audioGrid.Children.Clear();
            this.videoGrid.RowDefinitions.Clear();
            this.videoGrid.Children.Clear();
        }

        public void addMedia(IMedia media, bool is_video)
        {
            Grid grid = is_video ? videoGrid : audioGrid;

            if (is_video == true && grid.RowDefinitions.Count > 0)
            {
                // splitter
                RowDefinition split_row = new RowDefinition();
                split_row.Height = new GridLength(1, GridUnitType.Auto);
                grid.RowDefinitions.Add(split_row);
                GridSplitter splitter = new GridSplitter();
                splitter.ResizeDirection = GridResizeDirection.Rows;
                splitter.Height = 3;
                splitter.HorizontalAlignment = HorizontalAlignment.Stretch;
                splitter.VerticalAlignment = VerticalAlignment.Stretch;
                Grid.SetColumnSpan(splitter, 1);
                Grid.SetColumn(splitter, 0);
                Grid.SetRow(splitter, grid.RowDefinitions.Count - 1);
                grid.Children.Add(splitter);
            }

            // video
            RowDefinition row = new RowDefinition();
            row.Height = new GridLength(1, GridUnitType.Star);            
            grid.RowDefinitions.Add(row);

            MediaBox media_box = new MediaBox(media, is_video);
            Grid.SetColumn(media_box, 0);
            Grid.SetRow(media_box, grid.RowDefinitions.Count - 1);
            grid.Children.Add(media_box);            
        }
    }
}
