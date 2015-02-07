using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ssi
{
    public class ViewTime
    {
        private double totalDuration = 0;
        public double TotalDuration
        {
            get { return totalDuration; }
            set { totalDuration = value; }
        }

        private double selectionStart = 0;
        public double SelectionStart
        {
            get { return selectionStart; }
            set { selectionStart = value; }
        }
        private double selectionStop = 0;
        public double SelectionStop
        {
            get { return selectionStop; }
            set { selectionStop = value; }
        }

        private double selectionInPixel = 0;
        public double SelectionInPixel
        {
            get { return selectionInPixel; }
            set { selectionInPixel = value; }
        }

        public double TimeFromPixel(double pixel)
        {
            if (pixel > SelectionInPixel) {
                return totalDuration;
            }
            else if (pixel > 0)
            {
                return selectionStart + (pixel / selectionInPixel) * (selectionStop - selectionStart);
            } else {            
                return 0;
            }
        }

        public double PixelFromTime(double time)
        {
            if (time > selectionStop) {
                return SelectionInPixel;
            }
            if (time > selectionStart)
            {
                return ((time - selectionStart) / (selectionStop - selectionStart)) * selectionInPixel;
            }
            else 
            {
                return 0;
            }

        }
    }
}
