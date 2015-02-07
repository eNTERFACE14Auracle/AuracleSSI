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
using System.Collections;

namespace ssi
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        public ViewHandler viewh = null;

        public Window1()
        {
            InitializeComponent();

            this.view.OnHandlerLoaded += viewHandlerLoaded;            
        }

        private void viewHandlerLoaded(ViewHandler handler)
        {
            this.viewh = handler;
            this.viewh.LoadButton.Click += loadButton_Click;
            this.KeyDown += handler.OnKeyDown;
        }

        private void loadButton_Click(object sender, RoutedEventArgs e)
        {
            this.viewh.addFiles();
        }



    }
}
