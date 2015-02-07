using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Input;
using System.Windows;
using System.Threading;
using System.Windows.Controls;
using System.Windows.Documents;
using System.ComponentModel;
using System.Windows.Media;
using System.IO;

namespace ssi
{       
    public class ViewHandler
    {
        private static ViewTime time = null;

        enum ssi_file_type
        {
            UNKOWN = 0,
            CSV,
            AUDIO,
            VIDEO,
            ANNO,
            STREAM,
            EVENTS
        }
        static readonly string[] SSI_FILE_TYPE_NAME = { "ssi", "audio", "video", "anno", "stream", "events" };

        public static ViewTime Time
        {
            get { return ViewHandler.time; }            
        }

        private TimeTrack time_track = null;
        private MediaList media_list = new MediaList();
        public Cursor cursor = null;

        private List<ISignalTrack> signal_tracks = new List<ISignalTrack>();
        private List<Signal> signals = new List<Signal>();        

        private List<AnnoTrack> anno_tracks = new List<AnnoTrack>();
        private List<AnnoList> annos = new List<AnnoList>();
        private AnnoList current_anno = null;

        private ViewControl view;

        public Button LoadButton
        {
            get { return this.view.navigator.loadButton; }
        }

        public ViewHandler(ViewControl view)            
        {
            this.view = view;                

            this.view.annoListControl.annoDataGrid.SelectionChanged += annoDataGrid_SelectionChanged;
            this.view.annoListControl.editButton.Click += editAnnoButton_Click;

            this.view.navigator.newAnnoButton.Click += newAnnoButton_Click;
            this.view.navigator.saveAnnoButton.Click += saveAnnoButton_Click;
            this.view.navigator.saveAnnoAsButton.Click += saveAnnoAsButton_Click;
            this.view.navigator.clearButton.Click += clearButton_Click;
            this.view.navigator.jumpFrontButton.Click += jumpFrontButton_Click;
            this.view.navigator.playButton.Click += playButton_Click;
            this.view.navigator.jumpEndButton.Click += jumpEndButton_Click;

            //AnnoTrack.OnTrackPlay += playTrackHandler;
            AnnoTrack.OnTrackChange += changeAnnoTrackHandler;
            AnnoTrack.OnTrackSegmentChange += changeAnnoTrackSegmentHandler;
            SignalTrack.OnChange += changeSignalTrackHandler;            

            ViewHandler.time = new ViewTime();
            this.view.trackControl.SizeChanged += new SizeChangedEventHandler(OnTrackControlSizeChanged);
            time.SelectionInPixel = this.view.trackControl.ActualWidth;
            this.view.trackControl.timeTrackControl.rangeSlider.ViewTime = time;            

            this.view.trackControl.timeTrackControl.rangeSlider.OnTimeRangeChanged += this.view.trackControl.timeTrackControl.timeTrack.timeRangeChanged;
            this.view.trackControl.timeTrackControl.rangeSlider.Update();           

            this.time_track = this.view.trackControl.timeTrackControl.timeTrack;

            this.media_list.OnMediaPlay += mediaPlayHandler;
            this.view.Drop += viewControl_Drop;
           
            initCursor();
        }

        public void OnKeyDown(object sender, KeyEventArgs e)
        {
            AnnoTrack.OnKeyDownHandler(sender, e);
            //SignalTrackStatic.OnSignalTrackKeyDown(sender, e);
        }

        void OnTrackControlSizeChanged(object sender, SizeChangedEventArgs e)
        {
            ViewHandler.time.SelectionInPixel = this.view.trackControl.ActualWidth;
            this.view.trackControl.timeTrackControl.rangeSlider.Update();
        }

        public AnnoList getCurrentAnno()
        {
            return current_anno;
        }

        public Signal getCurrentSignal()
        {
            return SignalTrack.SelectedSignal;
        }

        public void clear()
        {
            Stop();

            this.view.signalNameLabel.Text = "";
            this.view.signalNameLabel.ToolTip = "";
            this.view.signalBytesLabel.Text = "";
            this.view.signalDimLabel.Text = "";
            this.view.signalSrLabel.Text = "";
            this.view.signalTypeLabel.Text = "";
            //this.view.annoNameLabel.Text = ""; 
            this.view.signalValueLabel.Text = "";
            this.view.positionLabel.Text = "00:00:00";
            this.view.navigator.playButton.IsEnabled = false;

            this.cursor.X = 0;            
            setAnnoList(null);

            this.view.trackControl.annoTrackControl.clear();            
            this.view.trackControl.signalTrackControl.clear();
            this.view.videoControl.clear ();

            this.anno_tracks.Clear();
            this.annos.Clear ();          
            this.media_list.clear();

            Time.TotalDuration = 0;

            this.view.trackControl.timeTrackControl.rangeSlider.Update();

        }        

        void mediaPlayHandler(MediaList videos, MediaPlayEventArgs e)
        {
            double pos = ViewHandler.Time.PixelFromTime(e.pos);
            cursor.X = pos;            
            double time = Time.TimeFromPixel(pos);
            this.view.positionLabel.Text = ViewTools.FormatSeconds(time);
        }

        void annoDataGrid_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ListView grid = (ListView)sender;

            if (grid.SelectedIndex >= 0 && grid.SelectedIndex < grid.Items.Count)
            {
                AnnoListItem item = current_anno[grid.SelectedIndex];
                moveCursorTo (item.Start);
            }
        }

        void onCursorChange(double pos)
        {
            Signal signal = getCurrentSignal();
            if (signal != null)
            {
                double time = Time.TimeFromPixel(pos);
                this.view.signalValueLabel.Text = signal.Value(time).ToString();
            }
        }

        void moveCursorTo(double seconds)
        {
            double pos = ViewHandler.Time.PixelFromTime(seconds);
            cursor.X = pos;
            //this.view.trackControl.scrollViewer.ScrollToHorizontalOffset(Math.Max(0, pos - this.view.trackControl.scrollViewer.ActualWidth / 2));
            double time = Time.TimeFromPixel(pos);
            this.view.positionLabel.Text = ViewTools.FormatSeconds(time);            
        }

        public void newAnno()
        {
            AnnoList anno = new AnnoList();
            addAnno(anno);
        }

        public void addAnno(AnnoList anno)
        {            
            AnnoTrack track = this.view.trackControl.annoTrackControl.addAnnoTrack(anno);
            this.view.trackControl.timeTrackControl.rangeSlider.OnTimeRangeChanged += track.timeRangeChanged;

            this.anno_tracks.Add(track);
            this.annos.Add(anno);            
            
            track.timeRangeChanged (ViewHandler.Time);
        }

        private void loadAnno(string filename)
        {
            AnnoList anno = AnnoList.LoadfromFile(filename);
            if (anno != null)
            {
                annos.Add(anno);
                setAnnoList(anno);
                addAnno(anno);
            }
        }

        private void loadEvents(string filename)
        {
            AnnoList anno = AnnoList.LoadFromEventsFile(filename);
            if (anno != null)
            {
                annos.Add(anno);
                setAnnoList(anno);
                addAnno(anno);
            }
        }

        private void loadStream(string filename)
        {
            Signal signal = Signal.LoadStreamFile(filename);
            if (signal != null && signal.loaded)
            {
                addSignal(signal);
            }
        }

        private void loadCSV(string filename)
        {
            Signal signal = Signal.LoadCSVFile(filename);
            if (signal != null && signal.loaded)
            {
                addSignal(signal);                
            }
        }

        private void loadWav(string filename)
        {
            Signal signal = Signal.LoadWaveFile(filename);
            if (signal != null && signal.loaded)
            {
                addSignal(signal);
            }
        }

        private void addSignal(Signal signal)
        {
            ISignalTrack track = this.view.trackControl.signalTrackControl.addSignalTrack(signal);
            this.view.trackControl.timeTrackControl.rangeSlider.OnTimeRangeChanged += track.timeRangeChanged;           

            this.signals.Add(signal);
            this.signal_tracks.Add(track);            

            double duration = signal.number / signal.rate;   
            if (duration > ViewHandler.Time.TotalDuration)
            {
                ViewHandler.Time.TotalDuration = duration;
                this.view.trackControl.timeTrackControl.rangeSlider.Update();
            }
            else
            {
                track.timeRangeChanged(ViewHandler.Time);
            }
            //updateTimeRange(duration);
            //track.timeRangeChanged(ViewHandler.Time);        
        }

        private void updateTimeRange(double duration)
        {
            if (duration > ViewHandler.Time.TotalDuration)
            {
                ViewHandler.Time.TotalDuration = duration;
                this.view.trackControl.timeTrackControl.rangeSlider.Update();
            }            
        }

        private void loadMedia(string filename, bool is_video)
        {
            double pos = ViewHandler.Time.TimeFromPixel(cursor.X);
            IMedia media = media_list.addMedia(filename, pos);
            this.view.videoControl.addMedia(media, is_video);
            this.view.navigator.playButton.IsEnabled = true;            
            //updateTimeRange(media.GetLength ());
        }        

        private void setAnnoList(AnnoList anno)
        {
            current_anno = anno;
            view.annoListControl.annoDataGrid.ItemsSource = anno;            
        }

        void jumpFrontButton_Click(object sender, RoutedEventArgs e)
        {
            bool is_playing = IsPlaying ();
            Stop();
            moveCursorTo(0);
            if (is_playing)
            {
                Play();
            }
        }

        void jumpEndButton_Click(object sender, RoutedEventArgs e)
        {
            Stop();
            moveCursorTo(ViewHandler.Time.TotalDuration);
        }

        void playButton_Click(object sender, RoutedEventArgs e)
        {
            if (IsPlaying ())
            {
                Stop();                    
            }
            else
            {
                Play();                    
            }
        }

        public void Stop()
        {
            if (IsPlaying ())
            {
                media_list.stop();
                this.view.navigator.playButton.Content = "Play";
            }
        }

        public bool IsPlaying()
        {
            return media_list.IsPlaying;
        }

        public void Play ()
        {
            Stop();
            
            double pos = 0;
            AnnoListItem item = null;
            bool loop = false;
          
            pos = cursor.X;
            double from = ViewHandler.Time.TimeFromPixel(pos);
            double to = ViewHandler.time.TotalDuration;
            item = new AnnoListItem(from, to, "");
            cursor.X = pos;

            try
            {
                media_list.play(item, loop);
                this.view.navigator.playButton.Content = "Stop";
            }
            catch (Exception e)
            {
                System.Console.WriteLine(e.ToString());
            }
            
        }

        private void changeAnnoTrackHandler(AnnoTrack track, EventArgs e)
        {
           // this.view.annoNameLabel.Text = track.AnnoList.Filename;
           // this.view.annoNameLabel.ToolTip = track.AnnoList.Filepath;            
            setAnnoList(track.AnnoList);
        }

        private void changeAnnoTrackSegmentHandler(AnnoTrackSegment segment, EventArgs e)
        {
           // this.view.annoNameLabel.Text = track.AnnoList.Filename;
           // this.view.annoNameLabel.ToolTip = track.AnnoList.Filepath;
           // setAnnoList(track.AnnoList);

            foreach (AnnoListItem item in view.annoListControl.annoDataGrid.Items)
            {
                if (segment.Item.Start == item.Start)
                {
                    view.annoListControl.annoDataGrid.SelectedItem = item;
                    view.annoListControl.annoDataGrid.ScrollIntoView(view.annoListControl.annoDataGrid.SelectedItem);
                    break;
                }
            }
        }

        private void changeSignalTrackHandler(ISignalTrack track, EventArgs e)
        {
            Signal signal = track.getSignal();

            if (signal != null)
            {
                this.view.signalNameLabel.Text = signal.Filename;                
                this.view.signalNameLabel.ToolTip = signal.Filepath;
                this.view.signalBytesLabel.Text = signal.bytes + " bytes";
                this.view.signalDimLabel.Text = signal.dim.ToString();
                this.view.signalSrLabel.Text = signal.rate + " Hz";
                this.view.signalTypeLabel.Text = ViewTools.SSI_TYPE_NAME[(int)signal.type];
                this.view.signalValueLabel.Text = signal.Value(time.SelectionStart).ToString ();
            }
        }

        public void saveAnno(string filepath)
        {
            if (this.current_anno != null)
            {
                if (filepath != null)
                {                    
                    this.current_anno.saveToFile(filepath);
                }
                else
                {
                    this.current_anno.saveToFile();
                }
            }
        }
        
        private void trackGrid_MouseDown(object sender, MouseButtonEventArgs e)
        {
            bool is_playing = IsPlaying ();
            if (is_playing)
            {
                Stop();
            }

            double pos = e.GetPosition(this.view.trackControl.trackGrid).X;                
            cursor.X = pos;                
            media_list.move(ViewHandler.Time.TimeFromPixel(pos));
            double time = Time.TimeFromPixel(pos); 
            this.view.positionLabel.Text = ViewTools.FormatSeconds(time);

            if (is_playing)
            {
                Play();
            }
        }

        private void trackGrid_MouseMove(object sender, MouseEventArgs e)
        {
            if (!IsPlaying ())
            {
                if (e.LeftButton == MouseButtonState.Pressed)
                {
                    double pos = e.GetPosition(this.view.trackControl.trackGrid).X;
                    cursor.X = pos;                    
                    media_list.move(ViewHandler.Time.TimeFromPixel(pos));
                    view.viewGrid.UpdateLayout();
                    double time = Time.TimeFromPixel(pos);
                    this.view.positionLabel.Text = ViewTools.FormatSeconds(time);
                }
            }
        }

        private void initCursor ()
        {
            this.view.trackControl.signalTrackControl.MouseDown += trackGrid_MouseDown;
            this.view.trackControl.annoTrackControl.MouseDown += trackGrid_MouseDown;            
            this.view.trackControl.annoTrackControl.MouseMove += trackGrid_MouseMove;

            AdornerLayer cursorLayer = view.trackControl.adornerLayer.AdornerLayer;
            cursor = new Cursor(this.view.trackControl.trackGrid, Brushes.Red, 1.5);
            cursorLayer.Add(cursor);

            cursor.OnCursorChange += onCursorChange;
        }

        private void viewControl_Drop(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {                
                string[] filenames = e.Data.GetData(DataFormats.FileDrop, true) as string[];                
                if (filenames != null)
                {
                    this.view.Cursor = Cursors.Wait;
                    loadFiles(filenames);
                    this.view.Cursor = Cursors.Arrow;
                }
            }
        }

        private void loadFiles(string[] filenames)
        {           
            for (int i = 0; i < filenames.Length; i++)
            {
                string filename = filenames[i];
               
                FileAttributes attr = File.GetAttributes(filename);
                if ((attr & FileAttributes.Directory) == FileAttributes.Directory)
                {
                    string[] subfilenames = Directory.GetFiles(filename);
                    loadFiles(subfilenames);
                }
                else
                {
                    if (!loadFromFile(filename))
                    {
                        //ViewTools.ShowErrorMessage("could not load " + filenames[i]);
                    }
                }
            }
        }

        public bool loadFromFile(string filename)
        {
            
            bool loaded = false;

            ssi_file_type ftype = ssi_file_type.UNKOWN;

            int index = filename.LastIndexOf('.');
            if (index > 0)
            {
                string type = filename.Substring(index + 1);
                switch (type)
                {
                    case "avi":
                    case "wmv":
                    case "mp4":
                        ftype = ssi_file_type.VIDEO;
                        break;

                    case "csv":                    
                        ftype = ssi_file_type.CSV;
                        break;

                    case "wav":
                        ftype = ssi_file_type.AUDIO;
                        break;

                    case "anno":
                        ftype = ssi_file_type.ANNO;
                        break;

                    case "stream":
                        ftype = ssi_file_type.STREAM;
                        break;
                    
                    case "events":
                        ftype = ssi_file_type.EVENTS;
                        break;
                }
            }
                        
            switch (ftype)
            {
                case ssi_file_type.VIDEO:
                    loadMedia(filename, true);
                    loaded = true;
                    break;

                case ssi_file_type.CSV:
                    loadCSV(filename);
                    loaded = true;
                    break;

                case ssi_file_type.AUDIO:
                    loadWav(filename);
                    loadMedia(filename, false);
                    loaded = true;
                    break;

                case ssi_file_type.ANNO:
                    loadAnno(filename);
                    loaded = true;
                    break;

                case ssi_file_type.STREAM:
                    loadStream(filename);
                    loaded = true;
                    break;

                case ssi_file_type.EVENTS:
                    loadEvents(filename);
                    loaded = true;
                    break;
            }
      
            return loaded;
        }

        private void editAnnoButton_Click(object sender, RoutedEventArgs e)
        {
            string label = view.annoListControl.editTextBox.Text;
            foreach (AnnoListItem item in view.annoListControl.annoDataGrid.SelectedItems)
            {
                item.Label = label;
            }
            //view.annoListControl.annoDataGrid.editLabel(label);
            
        }

        public void saveAnnoAs()
        {
            if (this.current_anno != null)
            {
                string filename = ViewTools.SaveFileDialog(this.current_anno.Filepath, ".anno");
                saveAnno(filename);
            }
        }

        public void addFiles ()
        {
            string[] filenames = ViewTools.OpenFileDialog("Viewable files (*.stream,*.anno,*.wav,*.avi,*.wmv)|*.stream;*.anno;*.wav;*.avi;*.wmv|Signal files (*.stream)|*.stream|Annotation files (*.anno)|*.anno|Wave files (*.wav)|*.wav|Video files(*.avi,*.wmv,*.mp4)|*.avi;*.wmv|All files (*.*)|*.*", true);
            if (filenames != null)
            {
                this.view.Cursor = Cursors.Wait;
                loadFiles(filenames);
                this.view.Cursor = Cursors.Arrow;
            }
        }

        private void clearButton_Click(object sender, RoutedEventArgs e)
        {
            clear();
        }

        private void newAnnoButton_Click(object sender, RoutedEventArgs e)
        {
            newAnno();
        }

        private void saveAnnoButton_Click(object sender, RoutedEventArgs e)
        {
            saveAnno(null);
        }

        private void saveAnnoAsButton_Click(object sender, RoutedEventArgs e)
        {
            saveAnnoAs();
        }



        
    }
}
