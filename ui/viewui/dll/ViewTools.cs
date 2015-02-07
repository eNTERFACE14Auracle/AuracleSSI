﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;

namespace ssi
{
    public class ViewTools
    {
        public enum SSI_TYPE : byte
        {
            UNDEF = 0,
            CHAR = 1,
            UCHAR = 2,
            SHORT = 3,
            USHORT = 4,
            INT = 5,
            UINT = 6,
            LONG = 7,
            ULONG = 8,
            FLOAT = 9,
            DOUBLE = 10,
            LDOUBLE = 11,
            STRUCT = 12,
            IMAGE = 13,
            BOOL = 14
        }

        public static readonly uint[] SSI_BYTES = {
            0, sizeof(char), sizeof(char), sizeof(short), sizeof(ushort), sizeof (int), sizeof(uint), sizeof(long), sizeof(ulong), sizeof(float), sizeof(double),sizeof(double), 0, 0, sizeof(bool)
        };
        
        public static readonly string[] SSI_TYPE_NAME = { "UNDEF", "CHAR", "UCHAR", "SHORT", "USHORT", "INT", "UINT", "LONG", "ULONG", "FLOAT", "DOUBLE", "LDOUBLE", "STRUCT", "IMAGE", "BOOL" };

        public static string[] OpenFileDialog(string filter, bool multi)
        {
            Microsoft.Win32.OpenFileDialog dlg = new Microsoft.Win32.OpenFileDialog();
            dlg.Filter = filter;
            dlg.Multiselect = multi;

            Nullable<bool> result = dlg.ShowDialog();

            string[] filenames = null;
            if (result == true)
            {
                filenames = dlg.FileNames;
            }

            return filenames;
        }

        public static string SaveFileDialog(string defaultname, string extension)
        {
            Microsoft.Win32.SaveFileDialog dlg = new Microsoft.Win32.SaveFileDialog();
            dlg.FileName = defaultname;
            dlg.DefaultExt = extension;

            Nullable<bool> result = dlg.ShowDialog();

            string filename = null;
            if (result == true)
            {
                filename = dlg.FileName;
            }

            return filename;
        }

        public static void ShowErrorMessage(string message)
        {
            MessageBox.Show(message, "Error", MessageBoxButton.OK);
        }

        public static string FormatSeconds(double seconds)
        {
            TimeSpan interval = TimeSpan.FromSeconds(seconds);
            string timeInterval = interval.ToString();

            int pIndex = timeInterval.IndexOf(':');
            pIndex = timeInterval.IndexOf('.', pIndex);

            if (pIndex > 0)
            {
                return timeInterval.Substring(0, pIndex + 3);
            }
            else
            {
                return timeInterval;
            }
        } 
    }
}
