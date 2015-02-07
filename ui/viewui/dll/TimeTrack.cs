using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows.Media;
using AC.AvalonControlsLibrary.Controls;
using System.Windows.Input;

namespace ssi
{
    public partial class TimeTrack : Canvas, ITrack
    {
        public const uint TICKGAP = 100;

        private double seconds = 0;
        private uint n_ticks = 0;
        private List<TimeTrackSegment> segments = new List<TimeTrackSegment> ();        
        private int unitCount = 0;

        public TimeTrack()
        {
            this.Background = Brushes.LightGray;
            this.MouseDown += new MouseButtonEventHandler(OnMouseDown);            
        }

        void OnMouseDown(object sender, MouseButtonEventArgs e)
        {
            TimeTrackSegment.Unit[] values = (TimeTrackSegment.Unit[])Enum.GetValues(typeof(TimeTrackSegment.Unit)); 
            TimeTrackSegment.Unit unit = values[++unitCount % values.Length];
            foreach (TimeTrackSegment segment in segments) 
            {
                segment.setUnit (unit);
            }
        }

        public double Seconds
        {
            get { return seconds; }
        }

        public void timeRangeChanged(ViewTime time)
        {
            this.seconds = time.TotalDuration;
            this.Width = time.SelectionInPixel;

            n_ticks = (uint)(this.Width / TICKGAP + 0.5);

            for (int i = segments.Count; i < n_ticks; i++)
            {
                segments.Add(new TimeTrackSegment(this));
            }

            double pos = 0;
            for (int i = 0; i < n_ticks; i++)
            {
                segments[i].setPos(pos);
                pos += 1.0 / n_ticks;
            }
        }
    }
}
