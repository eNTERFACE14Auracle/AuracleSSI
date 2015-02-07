using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;

namespace ssi
{    
    public partial class TimeTrackSegment : TextBlock
    {
        public enum Unit
        {
            SECONDS = 0,
            CLOCK = 1
        }

        private TimeTrack track = null;
        private double time = 0;
        private Unit unit = TimeTrackSegment.Unit.CLOCK;

        public void setUnit (Unit unit)
        {
            this.unit = unit;
            switch (unit)
            {
                case Unit.CLOCK:
                    this.Text = ViewTools.FormatSeconds(time);
                    break;
                case Unit.SECONDS:
                    this.Text = time.ToString();
                    break;
            }
        }        

        public TimeTrackSegment(TimeTrack track)
        {
            this.track = track;            
            track.Children.Add(this);
        }
        
        public void setPos (double pos) 
        {
            Canvas.SetLeft(this, pos * track.Width);
            time = track.Seconds * pos;
            setUnit(unit);
        }

       
    }
}
