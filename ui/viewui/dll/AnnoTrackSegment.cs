using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Data;

namespace ssi
{
    public class AnnoTrackSegment : TextBlock
    {

        public const int RESIZE_OFFSET = 10;
        public const int MIN_WIDTH = 5;

        public bool is_selected;
        public bool is_resizeable_right;
        public bool is_resizeable_left;
        public bool is_moveable;

        private AnnoListItem item = null;
        public AnnoListItem Item
        {
            get { return item; }
        }
        private AnnoTrack track = null;

        public AnnoTrack Track
        {
            get { return track; }
        }
      
        public AnnoTrackSegment(AnnoListItem item, AnnoTrack track)
        {
            this.track = track;
            this.item = item;

            this.is_selected = false;
            this.is_resizeable_left = false;
            this.is_resizeable_right = false;
            this.is_moveable = false;

            this.Inlines.Add(item.Label);
            this.FontSize = 12;
            this.TextWrapping = TextWrapping.Wrap;
            this.Background = System.Windows.Media.Brushes.SkyBlue;
            this.TextAlignment = TextAlignment.Center;
            this.TextTrimming = TextTrimming.WordEllipsis;
            ToolTip tt = new ToolTip();
            if (item.Meta == null || item.Meta.ToString() == "")
            {
                tt.Content = item.Label;
            }
            else
            {
                tt.Content = item.Label + "\n\n" + item.Meta;
            }
            tt.StaysOpen = true;
            this.ToolTip = tt;

            item.PropertyChanged += new System.ComponentModel.PropertyChangedEventHandler(item_PropertyChanged);

            MouseDown += new MouseButtonEventHandler(OnAnnoTrackSegmentMouseDown);

            update();
        }

        void OnAnnoTrackSegmentMouseDown(object sender, MouseButtonEventArgs e)
        {
            if (Keyboard.IsKeyDown(Key.LeftAlt) || Keyboard.IsKeyDown(Key.RightAlt))
            {
                LabelInputBox inputBox = new LabelInputBox("Input", "Enter a new label name", Text);
                inputBox.ShowDialog();
                inputBox.Close();
                if (inputBox.DialogResult == true)
                {
                    rename(inputBox.Result());
                }
            }
        }

        void item_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {            
            update();
        }

        public void rename(string label)
        {
            this.item.Label = label;
            this.Text = label;
            update();
        }

        public void move(double pixel)
        {
            double time = ViewHandler.Time.TimeFromPixel(ViewHandler.Time.PixelFromTime (this.item.Start) + pixel);            
            this.item.Start = time;
            time = ViewHandler.Time.TimeFromPixel(ViewHandler.Time.PixelFromTime (this.item.Stop) + pixel);
            this.item.Stop = time;
            update();
        }

        public void resize_left(double pixel)
        {
            double time = ViewHandler.Time.TimeFromPixel(ViewHandler.Time.PixelFromTime (this.item.Start) + pixel);
            this.item.Start = time;
            update();
        }

        public void resize_right(double pixel)
        {
            double time = ViewHandler.Time.TimeFromPixel(ViewHandler.Time.PixelFromTime (this.item.Stop) + pixel);
            this.item.Stop = time;
            update();
        }

        public void update () 
        {
            double start = ViewHandler.Time.PixelFromTime(item.Start);
            double stop = ViewHandler.Time.PixelFromTime(item.Start + item.Duration);
            double len = Math.Max (MIN_WIDTH, stop - start);

            this.Text = item.Label;

            if (len >= 0 && start >= 0)
            {
                this.Height = track.ActualHeight;
                this.Width = len;
                Canvas.SetLeft(this, start);
            }
        }

        public void select(bool flag)
        {
            this.is_selected = flag;
            if (flag)
            {
                this.Background = System.Windows.Media.Brushes.DeepSkyBlue;
            }
            else
            {
                this.Background = System.Windows.Media.Brushes.SkyBlue;
            }
        }

        public bool isResizableOrMovable()
        {
            return this.is_resizeable_left || this.is_resizeable_right || this.is_moveable;
        }

        public void resizeableLeft(bool flag)
        {
            this.is_resizeable_left = flag;
            if (flag)
            {
                this.Cursor = Cursors.SizeWE;
            }
            else
            {
                this.Cursor = Cursors.Arrow;
            }
        }

        public void resizeableRight(bool flag)
        {
            this.is_resizeable_right = flag;
            if (flag)
            {
                this.Cursor = Cursors.SizeWE;
            }
            else
            {
                this.Cursor = Cursors.Arrow;
            }
        }

        public void movable(bool flag)
        {
            this.is_moveable = flag;
            if (flag)
            {
                this.Cursor = Cursors.SizeAll;
            }
            else
            {
                this.Cursor = Cursors.Arrow;
            }
        }

        public void checkResizeable(Point point)
        {
            if (point.X > 0 && point.X < RESIZE_OFFSET)
            {
                if (this.is_resizeable_left == false)
                {
                    this.resizeableLeft(true);
                }
            }
            else if (point.X > this.ActualWidth - RESIZE_OFFSET && point.X < this.ActualWidth)
            {
                if (this.is_resizeable_right == false)
                {
                    this.resizeableRight(true);
                }
            }
            else if (point.X > this.ActualWidth / 2 - RESIZE_OFFSET && point.X < this.ActualWidth / 2 + RESIZE_OFFSET)
            {
                if (this.is_moveable == false)
                {
                    this.movable(true);
                }
            }
            else if (this.is_resizeable_left)
            {
                this.resizeableLeft(false);
            }
            else if (this.is_resizeable_right)
            {
                this.resizeableRight(false);
            }
            else if (this.is_moveable)
            {
                this.movable(false);
            }
        }

    }
}
