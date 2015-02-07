using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Shapes;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using AC.AvalonControlsLibrary.Controls;

namespace ssi
{
    public class AnnoTrackPlayEventArgs : EventArgs
    {
        public AnnoListItem item = null;     
    }

    public class AnnoTrackMoveEventArgs : EventArgs
    {
        public double pos = 0;
    }

    public delegate void AnnoTrackChangeEventHandler(AnnoTrack track, EventArgs e);
    public delegate void AnnoTrackSegmentChangeEventHandler(AnnoTrackSegment segment, EventArgs e);
    public delegate void AnnoTrackResizeEventHandler(double pos);

    public partial class AnnoTrackStatic : Canvas
    {
        static protected AnnoTrack selected_track = null;
        static protected AnnoTrackSegment selected_segment = null;
        static protected int selected_zindex = 0;
        static protected int selected_zindex_max = 0;

        static public event AnnoTrackChangeEventHandler OnTrackChange;
        static public event AnnoTrackSegmentChangeEventHandler OnTrackSegmentChange;
        static public event AnnoTrackResizeEventHandler OnTrackResize;

        static public AnnoTrackSegment GetSelectedSegment()
        {
            return selected_segment;
        }

        static public AnnoTrack GetSelectedTrack()
        {
            return selected_track;
        }

        static protected void SelectTrack(AnnoTrack t)
        {
            UnselectTrack();
            selected_track = t;
            t.select(true);

            if (OnTrackChange != null)
            {
                OnTrackChange(selected_track, null);                
            }
        }

        static protected void UnselectTrack()
        {
            if (selected_track != null)
            {
                selected_track.select(false);
                selected_track = null;
            }
        }

        static protected void SelectSegment(AnnoTrackSegment s)
        {
            UnselectSegment();
            if (s != null)
            {
                s.select(true);
                selected_segment = s;
                selected_zindex = Panel.GetZIndex(selected_segment);
                Panel.SetZIndex(selected_segment, selected_zindex_max + 1);

                if (OnTrackSegmentChange != null)
                {
                    OnTrackSegmentChange(s, null);
                }
            }
        }

        static protected void UnselectSegment()
        {
            if (selected_segment != null)
            {
                selected_segment.select(false);
                Panel.SetZIndex(selected_segment, selected_zindex);
                selected_segment = null;
            }
        }

        static public void OnKeyDownHandler(object sender, KeyEventArgs e)
        {
            
            switch (e.Key)
            {
                case Key.Delete:

                    if (selected_segment != null)
                    {
                        AnnoTrackSegment tmp = selected_segment;
                        UnselectSegment();
                        selected_track.remSegment(tmp);
                    }

                    break;
            }
        }

        static public void FireOnMove(double pos)
        {
            if (OnTrackResize != null)
            {
                OnTrackResize(pos);
            }
        }
    }

    public class AnnoTrack : AnnoTrackStatic, ITrack
    {
        private List<AnnoTrackSegment> segments = new List<AnnoTrackSegment>();
        private AnnoList anno_list = null;

        public AnnoList AnnoList
        {
            get { return anno_list; }
            set { anno_list = value; }
        }

        private bool is_selected = false;         

        public AnnoTrack(AnnoList list)
        {
            this.anno_list = list;
            this.SizeChanged += new SizeChangedEventHandler(sizeChanged);
            AnnoTrack.SelectTrack(this);

            foreach (AnnoListItem item in list) 
            {
                addSegment(item);
            }
        }        

        void sizeChanged(object sender, SizeChangedEventArgs e)
        {
            this.Visibility = Visibility.Hidden;
            foreach (AnnoTrackSegment segment in segments)
            {
                segment.Height = e.NewSize.Height;
            }
            this.Visibility = Visibility.Visible;
        }
       
        public void select(bool flag)
        {
            this.is_selected = flag;
            if (flag)
            {
                this.Background = System.Windows.Media.Brushes.LightCyan;

            }
            else
            {
                this.Background = System.Windows.Media.Brushes.Azure;
            }
        }

        public AnnoTrackSegment getSegment(AnnoListItem item)
        {
            foreach (AnnoTrackSegment segment in segments) {
                if (segment.Item == item)
                {
                    return segment;
                }
            }
            return null;
        }

        public AnnoTrackSegment addSegment(AnnoListItem item)
        {                       
            AnnoTrackSegment segment = new AnnoTrackSegment(item, this);                                 
            segments.Add(segment);
            this.Children.Add(segment);
            selected_zindex_max = Math.Max(selected_zindex_max, Panel.GetZIndex(segment));

            return segment;
        }

        public void remSegment(AnnoTrackSegment s)
        {
            anno_list.Remove(s.Item);
            s.Track.Children.Remove(s);
            s.Track.segments.Remove(s);
        }

        protected override void OnMouseDown(MouseButtonEventArgs e)
        {
            base.OnMouseDown(e);

            // change track
            if (selected_track != this)
            {
                AnnoTrack.SelectTrack(this);
            }

            // create new segment
            if (e.ChangedButton == MouseButton.Right)
            {
                
                double start = ViewHandler.Time.TimeFromPixel(e.GetPosition(this).X);
                double stop = ViewHandler.Time.TimeFromPixel(e.GetPosition(this).X + 5 * AnnoTrackSegment.RESIZE_OFFSET);
                double len = stop - start;

                AnnoListItem item = new AnnoListItem(start, len, "");
                anno_list.Add(item);
                AnnoTrackSegment s = addSegment(item);                
                SelectSegment(s);      
            }
        }

        protected override void OnMouseMove(MouseEventArgs e)
        {
            base.OnMouseMove(e);

            if (selected_segment != null)
            {

                Point point = e.GetPosition(selected_segment);                                                       

                // check if use wants to resize/move

                if (e.LeftButton == MouseButtonState.Pressed)
                {
                    // resize segment left
                    if (selected_segment.is_resizeable_left)
                    {                        
                        double delta = point.X;
                        if (selected_segment.ActualWidth - delta > 3 * AnnoTrackSegment.RESIZE_OFFSET)
                        {
                            selected_segment.resize_left(delta);
                            FireOnMove(selected_segment.Item.Start);
                        }
                    }
                    // resize segment right
                    else if (selected_segment.is_resizeable_right)
                    {
                        double delta = point.X - selected_segment.ActualWidth;
                        if (point.X > 3 * AnnoTrackSegment.RESIZE_OFFSET)
                        {
                            selected_segment.resize_right(delta);
                            FireOnMove(selected_segment.Item.Stop);
                        }
                    }
                    // move segment
                    else if (selected_segment.is_moveable)
                    {
                        double pos = GetLeft(selected_segment);
                        double delta = point.X - selected_segment.ActualWidth / 2;
                        if (pos + delta >= 0 && pos + selected_segment.ActualWidth + delta <= this.Width)
                        {
                            selected_segment.move(delta);
                            FireOnMove(selected_segment.Item.Start + (selected_segment.Item.Stop - selected_segment.Item.Start) * 0.5);
                        }
                    }
                } 
                else
                {
                    // check if use can resize/move

                    selected_segment.checkResizeable(point);

                    // check if selection still valid

                    if (selected_segment.IsMouseOver == false)
                    {
                        UnselectSegment();
                    }
                }
            }
            
            if (selected_segment == null) 
            {

                // check for new selection

                foreach (AnnoTrackSegment s in segments)
                {
                    if (s.IsMouseOver)
                    {
                        SelectSegment(s);
                    }            
                }
            }
        }

        public void timeRangeChanged(ViewTime time)
        {
            this.Width = time.SelectionInPixel;

            foreach (AnnoTrackSegment s in segments)
            {
                s.Visibility = Visibility.Hidden;
                if (s.Item.Start >= time.SelectionStart && s.Item.Stop <= time.SelectionStop)
                {                    
                    s.update();
                    s.Visibility = Visibility.Visible;
                }                
            }            
        }
    }
}
