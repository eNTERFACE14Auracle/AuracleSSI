using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ssi
{
    public class AnnoListItem : MyListItem
    {
        private double start;
        private double duration;
        private String label;
        private String meta;

        public double Start
        {
            get { return start; }
            set {
                duration = Math.Max (0, duration + start - value);
                start = value;                
                OnPropertyChanged("Start");
                OnPropertyChanged("Duration");
            }
        }

        public double Stop
        {
            get { return start + duration; }
            set {
                duration = Math.Max (0, value - start);
                OnPropertyChanged("Stop");
                OnPropertyChanged("Duration");
            }
        }

        public double Duration
        {
            get { return duration; }
            set {
                duration = Math.Max (0, value);
                OnPropertyChanged("Stop");
                OnPropertyChanged("Duration");
            }
        }

        public String Label
        {
            get { return label; }
            set
            {
                label = value;
                OnPropertyChanged("Label");
            }
        }

        public String Meta
        {
            get { return meta; }
            set
            {
                meta = value;
                OnPropertyChanged("Meta");
            }
        }

        public AnnoListItem(double _start, double _duration, String _label, String _meta = "")
        {
            start = _start;
            duration = Math.Max (0, _duration);
            label = _label;
            meta = _meta;
        }
    }
}
