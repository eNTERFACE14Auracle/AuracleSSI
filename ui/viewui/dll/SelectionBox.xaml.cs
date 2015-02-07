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
using System.Windows.Shapes;

namespace ssi
{
    /// <summary>
    /// Interaction logic for SelectionBox.xaml
    /// </summary>
    public partial class SelectionBox : Window
    {
        public SelectionBox(string info, string[] comboBoxContent, string textBoxContent)
        {
            InitializeComponent();

            this.infoLabel.Content = info;

            if (comboBoxContent != null)
            {                
                foreach (string item in comboBoxContent)
                {
                    this.comboBox.Items.Add(item);                
                }
                this.comboBox.Visibility = Visibility.Visible;
            }
            else
            {
                this.comboBox.Visibility = Visibility.Hidden;
            }

            if (textBoxContent != null)
            {
                this.textField.Text = textBoxContent;
                this.textField.Visibility = Visibility.Visible;
            }
            else
            {
                this.textField.Visibility = Visibility.Hidden;
            }
        }

        public int ComboBoxResult()
        {
            return this.comboBox.SelectedIndex;
        }

        public string TextFieldResult()
        {
            return this.textField.Text;
        }

        private void okButton_Click(object sender, RoutedEventArgs e)
        {
            if (this.comboBox.SelectedIndex >= 0)
            {
                this.DialogResult = true;
            }
            this.Close();   
        }

        private void cancelButton_Click(object sender, RoutedEventArgs e)
        {
            this.DialogResult = false;
            this.Close();   
        }
    }
}
