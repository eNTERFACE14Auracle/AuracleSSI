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
    public delegate void HandlerLoaded (ViewHandler handler);    

    public partial class ViewControl : UserControl
    {
        private ViewHandler handler = null;
        public event HandlerLoaded OnHandlerLoaded;

        public ViewControl()
        {
            InitializeComponent();
            System.Threading.Thread.CurrentThread.CurrentCulture = new System.Globalization.CultureInfo("en-US");
            this.Dispatcher.ShutdownStarted += ControlUnloaded;
        }

        private void ControlLoaded(object sender, RoutedEventArgs e)
        {
            if (this.ActualWidth > 0 && this.handler == null)
            {
                this.handler = new ViewHandler(this);
                if (OnHandlerLoaded != null) {
                    OnHandlerLoaded(this.handler);
                }
            }
        }

        private void ControlUnloaded(object sender, EventArgs e)
        {
            if (this.handler != null)
            {
            }
        }

    }
}
