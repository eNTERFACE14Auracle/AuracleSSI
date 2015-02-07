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
    public delegate void TimeRangeChanged(ViewTime time); 

    public partial class TimeRangeSlider : UserControl
    {

        ViewTime viewTime = null;

        public ViewTime ViewTime
        {
            get { return viewTime; }
            set { viewTime = value; }
        }

        public event TimeRangeChanged OnTimeRangeChanged;

        public TimeRangeSlider()
        {
            InitializeComponent();

            ui.RangeStart = 0;
            ui.RangeStop = 100;
            ui.RangeStartSelected = 0;
            ui.RangeStopSelected = 100;
            ui.MinRange = 2;
            ui.MouseDoubleClick += new System.Windows.Input.MouseButtonEventHandler(OnMouseDoubleClick);
            ui.PreviewMouseUp += new MouseButtonEventHandler(OnPreviewMouseUp);
        }

        void OnMouseDoubleClick(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            ui.SetSelectedRange(ui.RangeStart, ui.RangeStop);
        }

        void OnPreviewMouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            Update();
        }

        public void Update()
        {
            if (viewTime != null)
            {
                viewTime.SelectionStart = viewTime.TotalDuration * ((double)ui.RangeStartSelected / (double)ui.RangeStop);
                viewTime.SelectionStop = viewTime.TotalDuration * ((double)ui.RangeStopSelected / (double)ui.RangeStop);

                if (OnTimeRangeChanged != null)
                {
                    OnTimeRangeChanged(viewTime);
                }
            }
        }

        
    }
}
