using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using System.Windows.Shapes;
using System.Windows.Media;
using System.Windows;
using System.Windows.Input;
using AC.AvalonControlsLibrary.Controls;

namespace ssi
{
    public delegate void SignalTrackChangeEventHandler(SignalTrack track, EventArgs e);    

    public partial class SignalTrackStatic : Panel
    {
        public static readonly SolidColorBrush[] brushes = {
            new SolidColorBrush (Colors.Blue),
            new SolidColorBrush (Colors.Green),
            new SolidColorBrush (Colors.Orange),
            new SolidColorBrush (Colors.Red),
            new SolidColorBrush (Colors.DarkBlue),
            new SolidColorBrush (Colors.DarkGreen),
            new SolidColorBrush (Colors.Purple),
            new SolidColorBrush (Colors.Brown)
        };
        public static readonly Pen[] pens = {
            new Pen(brushes[0], 1.0),
            new Pen(brushes[1], 1.0),
            new Pen(brushes[2], 1.0),
            new Pen(brushes[3], 1.0),
            new Pen(brushes[4], 1.0),
            new Pen(brushes[5], 1.0),
            new Pen(brushes[6], 1.0),
            new Pen(brushes[7], 1.0)
        };

        static protected SignalTrack selected_track = null;
        static public event SignalTrackChangeEventHandler OnChange;
        static public int brushCounter = 0;

        public static Signal SelectedSignal
        {
            get {
                if (SignalTrackStatic.selected_track != null)
                    return SignalTrackStatic.selected_track.getSignal();
                else 
                    return null;
            }
        }       

        static protected void SelectTrack(SignalTrack t)
        {
            UnselectTrack();
            selected_track = t;
            t.select(true);

            if (OnChange != null)
            {
                OnChange(selected_track, null);
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
    }

    public class SignalTrack : SignalTrackStatic, ISignalTrack
    {              
        private Signal signal = null;
        private double signal_from_in_sec = 0;
        private double signal_to_in_sec = 0;
        private Signal resampled = null;
        private double render_width;
        private bool is_selected = false;
        private bool resample = false;
        private int zoomLevel = 0;
        private int zoomOffset = 0;
        public int brushOffset = 0;

        public SignalTrack(Signal signal)
        {
            for (int i = 0; i < brushes.Length; i++)
            {
                brushes[i].Freeze();
                pens[i].Freeze();
            }
            this.signal = signal;
            brushOffset = brushCounter++ % brushes.Length;

            this.MouseWheel += new MouseWheelEventHandler(OnSignalTrackMouseWheel);

            SignalTrack.SelectTrack(this);
        }

        public void zoomReset()
        {
            zoomOffset = 0;
            zoomLevel = 0;
            this.InvalidateVisual();
        }

        public void changeColour()
        {
            brushOffset = ++brushOffset % brushes.Length;
            InvalidateVisual();
        }
        
        public void zoom(int level, int offset)
        {
            zoomLevel += level;
            zoomOffset += offset;
            this.InvalidateVisual();
        }

        void OnSignalTrackMouseWheel(object sender, MouseWheelEventArgs e)
        {
            if (Keyboard.IsKeyDown(Key.LeftAlt) || Keyboard.IsKeyDown(Key.RightAlt))
            {
                if (e.Delta < 0)
                {
                    if (Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.LeftCtrl))
                    {
                        zoom(-3, 0);
                    }
                    else
                    {
                        zoom(-1, 0);
                    }                    
                }
                else
                {
                    if (Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.LeftCtrl))
                    {
                        zoom(3, 0);
                    }
                    else
                    {
                        zoom(1, 0);
                    }  
                }
            }
            else
            {
                if (e.Delta < 0)
                {
                    if (Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.LeftCtrl)) 
                    {
                        zoom(0,6);
                    }
                    else 
                    {
                        zoom(0,1);
                    }
                }
                else
                {
                    if (Keyboard.IsKeyDown(Key.LeftAlt) || Keyboard.IsKeyDown(Key.RightAlt))
                    {
                        zoom(0,-6);
                    }
                    else
                    {
                        zoom(0,-1);
                    }
                }
            }
        }

        public Signal getSignal ()
        {
            return signal;            
        }

        public void clear()
        {
            this.Children.Clear();
            signal = null;
        }

        public void select(bool flag)
        {
            this.is_selected = flag;
            if (flag)
            {
                this.Background = System.Windows.Media.Brushes.WhiteSmoke;                

            }
            else
            {
                this.Background = System.Windows.Media.Brushes.Transparent; // System.Windows.Media.Brushes.FloralWhite;
                
            }
        }

        protected static void Paint(DrawingContext dc, SignalTrack track, Signal signal, uint dimension, double height, uint width, bool isAudio)
        {
            if (signal.number == 0 || signal.data == null || height == 0 || width == 0)
            {
                return;
            }

            if (isAudio)
            {
                for (uint d = dimension; d <= dimension; d++)
                {
                    float zoomFactor = track.zoomLevel * ((signal.max[d] - signal.min[d]) / 10);
                    float zoomOffset = track.zoomOffset * ((signal.max[d] - signal.min[d]) / 10);
                    float minVal = signal.min[d] + zoomFactor;
                    float maxVal = signal.max[d] - zoomFactor;
                    float offset = minVal;
                    float scale = (float)height / (maxVal - offset);
                    offset += zoomOffset;

                    Point from = new Point();
                    Point to = new Point();

                    if (width == signal.number)
                    {
                        for (int i = 0; i < signal.number; i++)
                        {
                            from.X = to.X = i;
                            from.Y = height - (signal.data[i * signal.dim + d] - offset) * scale;
                            to.Y = height - (-signal.data[i * signal.dim + d] - offset) * scale;
                            dc.DrawLine(pens[track.brushOffset % pens.Length], from, to);
                        }
                    }
                    else
                    {                        
                        float step = (float)width / (float)signal.number;
                        for (int i = 0; i < signal.number; i++)
                        {
                            from.X = to.X = (uint)i * step;
                            from.Y = height - (signal.data[i * signal.dim + d] - offset) * scale;
                            to.Y = height - (-signal.data[i * signal.dim + d] - offset) * scale;
                            dc.DrawLine(pens[track.brushOffset % pens.Length], from, to);
                        }
                    } 
                }
            }
            else
            {
                for (uint d = dimension; d <= dimension; d++)
                {
                    float zoomFactor = track.zoomLevel * ((signal.max[d] - signal.min[d]) / 10);
                    float zoomOffset = track.zoomOffset * ((signal.max[d] - signal.min[d]) / 100);
                    float minVal = signal.min[d] + zoomFactor;
                    float maxVal = signal.max[d] - zoomFactor;
                    float offset = minVal;
                    float scale = (float)height / (maxVal - offset);
                    offset += zoomOffset;

                    Point from = new Point(0, height - (signal.data[d] - offset) * scale);
                    Point to = new Point();
                    if (width == signal.number)
                    {
                        for (int i = 0; i < signal.number; i++)
                        {
                            float value = signal.data[i * signal.dim + d];

                            to.X = i;
                            to.Y = height - (value - offset) * scale;
                            
                            dc.DrawLine(pens[track.brushOffset % pens.Length], from, to);
                            from.X = to.X;
                            from.Y = to.Y;
                        }
                    }
                    else
                    {
                        float step = (float)width / (float)signal.number;
                        for (int i = 0; i < signal.number; i++)
                        {
                            float value = signal.data[i * signal.dim + d];

                            to.X = (uint)i * step;
                            to.Y = height - (value - offset) * scale;
                            dc.DrawLine(pens[track.brushOffset % pens.Length], from, to);
                            from.X = to.X;
                            from.Y = to.Y;
                        }
                    }
                }
            }
        }

        protected override void OnRender(DrawingContext dc)
        {
            base.OnRender(dc);

            if (signal != null && signal.loaded && render_width > 0)
            {
                double height = this.ActualHeight;
                uint len = (uint)(this.render_width + 0.5);
                if (resampled == null || resampled.number != len || resample)
                {
                    resampled = new Signal(this.signal, len, signal_from_in_sec, signal_to_in_sec);
                    resample = false;
                }

                SignalTrack.Paint(dc, this, resampled, signal.ShowDim, this.ActualHeight, len, signal.IsAudio);
            }
        }

        protected override void OnMouseDown(MouseButtonEventArgs e)
        {
            base.OnMouseDown(e);

            // change track
            if (selected_track != this)
            {
                SignalTrack.SelectTrack(this);
            }
        }

        public void timeRangeChanged(ViewTime time)
        {
            double pixel = time.SelectionInPixel;
            this.signal_from_in_sec = time.SelectionStart;
            this.signal_to_in_sec = time.SelectionStop;
            this.render_width = pixel;
            this.Width = pixel;
            this.resample = true;
            this.InvalidateVisual();
        }
    }
}
