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
using System.IO;
using System.Text.RegularExpressions;
using System.Runtime.InteropServices;
using Microsoft.Win32;
using System.Diagnostics;
using System.Security.Principal;

namespace ssi
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        static string X86_DIRECTORY = "Win32";
        static string X64_DIRECTORY = "x64";
        static string VISUALSTUDIO_VERSION = "vc10";
        static string PATH_VARIABLE = "PATH";

        static string SSI_BIN_DIRECTORY = "bin";
        static string SSI_LIBRARY_VARIABLE = "SSI_LIBS";        
        static string SSI_LIBRARY_DIRECTORY = "libs";
        static string SSI_INCLUDE_VARIABLE = "SSI_INCLUDE";
        static string SSI_INCLUDE_DIRECTORY = "include";
        static string SSI_CORE_DIRECTORY = "core";
        static string SSI_PLUGINS_DIRECTORY = "plugins";
        static string SSI_PIPELINE_EXTENSION = ".pipeline";
        static string SSI_PIPELINE_EXECUTABLE = "xmlpipe.exe";

       /* static string OCV_BIN_DIRECTORY = "vc10\\bin";
        static string OCV_LIBRARY_DIRECTORY = "vc10\\lib";
        static string OCV_INCLUDE_DIRECTORY = "build\\include";
        static string OCV_LIBRARY_VARIABLE = "OCV243_LIBS";
        static string OCV_INCLUDE_VARIABLE = "OCV243_INCLUDE";
        static string OCV_VERSION = "243";

        string ocv_directory;
        static string OCV_INFO_SELECT_DIRECTORY = "Please select root folder of your OpenCV 2.4.3 installation.";
        static string OCV_INFO_WRONG_DIRECTORY = "Sorry, but this doesn't look like a valid OpenCV 2.4.3 installation.";
        static string OCV_INFO_WRONG_VERSION = "Sorry, but this doesn't look like OpenCV 2.4.3.";
        static string OCV_INFO_OK = "Ok, looks like a valid OpenCV 2.4.3 installation."; */

        public Window1()
        {
            InitializeComponent();

            ComboBoxPlatform.Items.Add(X86_DIRECTORY + " (32-bit Version, runs on 32-bit and 64-bit System)");
            ComboBoxPlatform.Items.Add(X64_DIRECTORY + " (64-bit Version, runs only on 64-bit Systems)");

            ButtonCancel.Click += new RoutedEventHandler(ButtonCancel_Click);
            ButtonApply.Click += new RoutedEventHandler(ButtonApply_Click);
            ButtonUndo.Click += new RoutedEventHandler(ButtonUndo_Click);

            if (IsElevated() == false)
            {
                CheckBoxFileAssociation.IsEnabled = false;
            }

            //LabelOCVInfo.Content = OCV_INFO_SELECT_DIRECTORY;
            //ocv_directory = null;
        }

        void ButtonUndo_Click(object sender, RoutedEventArgs e)
        {
            this.Cursor = Cursors.Wait;
            Apply(true);
            this.Cursor = Cursors.Arrow;
        }

        void ButtonApply_Click(object sender, RoutedEventArgs e)
        {
            this.Cursor = Cursors.Wait;
            Apply(false);
            this.Cursor = Cursors.Arrow;
        }

        void ButtonCancel_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        void Log(string text)
        {
            TextBoxLog.Text += text + Environment.NewLine;
        }

        void Apply(bool undo)
        {
            /*if (CheckBoxOCVInclude.IsChecked == true || CheckBoxOCVInclude.IsChecked == true || CheckBoxOCVPath.IsChecked == true)
            {
                if (!CheckOCVPath(ocv_directory))
                {
                    MessageBoxResult result = MessageBox.Show("No valid OpenCV installation found, do you really want to continue?", "Warning", MessageBoxButton.YesNo, MessageBoxImage.Warning);
                    if (result == MessageBoxResult.No)
                    {
                        return;
                    }
                }
            }*/

            string flag = undo ? "UNDO" : "APPLY";

            if (CheckBoxPath.IsChecked == true)
            {
                Log(Environment.NewLine + ">> " + flag + " " + CheckBoxPath.Content);
                if (SSIApplyPath(undo))
                {
                    Log(">> SUCCESS");
                }
                else
                {
                    Log(">> ERROR");
                }
            }
            if (CheckBoxInclude.IsChecked == true)
            {
                Log(Environment.NewLine + ">> " + flag + " " + CheckBoxInclude.Content);
                if (SSIApplyInclude(undo))
                {
                    Log(">> SUCCESS");
                }
                else
                {
                    Log(">> ERROR");
                }
            }
            if (CheckBoxLibrary.IsChecked == true) 
            {
                Log(Environment.NewLine + ">> " + flag + " " + CheckBoxLibrary.Content);
                if (SSIApplyLibrary(undo))
                {
                    Log(">> SUCCESS");
                }
                else
                {
                    Log(">> ERROR");
                }
            }
            /*if (CheckBoxOCVPath.IsChecked == true)
            {
                Log(Environment.NewLine + ">> " + flag + " " + CheckBoxOCVPath.Content);
                if (OCVApplyPath(undo))
                {
                    Log(">> SUCCESS");
                }
                else
                {
                    Log(">> ERROR");
                }
            }
            if (CheckBoxOCVLibrary.IsChecked == true)
            {
                Log(Environment.NewLine + ">> " + flag + " " + CheckBoxOCVLibrary.Content);
                if (OCVApplyLibrary(undo))
                {
                    Log(">> SUCCESS");
                }
                else
                {
                    Log(">> ERROR");
                }
            }
            if (CheckBoxOCVInclude.IsChecked == true)
            {
                Log(Environment.NewLine + ">> " + flag + " " + CheckBoxOCVInclude.Content);
                if (OCVApplyInclude(undo))
                {
                    Log(">> SUCCESS");
                }
                else
                {
                    Log(">> ERROR");
                }
            }*/

            if (CheckBoxFileAssociation.IsChecked == true)
            {
                string exe = Directory.GetCurrentDirectory() + "\\" + SSIApplyPlatform(SSI_BIN_DIRECTORY) + "\\" + SSI_PIPELINE_EXECUTABLE;
                string icon = Directory.GetCurrentDirectory() + "\\core\\build\\tools\\ui.ico";

                Log(Environment.NewLine + ">> associate " + SSI_PIPELINE_EXTENSION + " with " + exe);

                FileAssociation.Associate(SSI_PIPELINE_EXTENSION, "xmlpipe", "ssi pipeline", icon, exe);
            }
        }
         
        private string SSIApplyPlatform(string dir)
        {
            if (ComboBoxPlatform.SelectedIndex == 1)
            {
                dir += "\\" + X64_DIRECTORY + "\\" + VISUALSTUDIO_VERSION;
            }
            else
            {
                dir += "\\" + X86_DIRECTORY + "\\" + VISUALSTUDIO_VERSION;
            }

            return dir;
        }

        /*private string OCVApplyPlatform(string dir)
        {
            if (ComboBoxPlatform.SelectedIndex == 1)
            {
                dir += "\\build\\" + X64_DIRECTORY;
            } else {
                dir += "\\build\\" + X86_DIRECTORY;
            }

            return dir;
        }*/

        private bool SSIApplyInclude(bool undo)
        {
            if (undo)
            {
                Environment.SetEnvironmentVariable(SSI_INCLUDE_VARIABLE, null, EnvironmentVariableTarget.User);
                return true;
            }

            string dir = Directory.GetCurrentDirectory() + "\\" + SSI_CORE_DIRECTORY + "\\" + SSI_INCLUDE_DIRECTORY;
            TextBoxLog.Text += "core include path: " + dir + Environment.NewLine;

            if (!Directory.Exists(dir))
            {
                Log("path not found: " + dir);
                return false;
            }

            string dir_plugins = Directory.GetCurrentDirectory() + "\\" + SSI_PLUGINS_DIRECTORY;
            TextBoxLog.Text += "plugins include path: " + dir_plugins + Environment.NewLine;

            if (!Directory.Exists(dir_plugins))
            {
                Log("path not found: " + dir_plugins);
                return false;
            }

            /*string dir_plugins = "";
            string lookup = Directory.GetCurrentDirectory() + "\\" + SSI_PLUGINS_DIRECTORY;
            if (Directory.Exists(lookup))
            {
                string[] subdirs = Directory.GetDirectories(lookup);
                foreach (string subdir in subdirs)
                {
                    string d = subdir + "\\" + SSI_INCLUDE_DIRECTORY;
                    if (Directory.Exists(d))
                    {
                        dir_plugins += ";" + d;
                    }
                }
            }

            TextBoxLog.Text += "tools include path: " + dir_plugins + Environment.NewLine;
            */

            Log("set $" + SSI_INCLUDE_VARIABLE + ": " + dir + ";" + dir_plugins);
            Environment.SetEnvironmentVariable(SSI_INCLUDE_VARIABLE, dir + ";" + dir_plugins, EnvironmentVariableTarget.User);

            return true;
        }

        private bool SSIApplyLibrary(bool undo)
        {
            if (undo)
            {
                Environment.SetEnvironmentVariable(SSI_LIBRARY_VARIABLE, null, EnvironmentVariableTarget.User);
                return true;
            }

            string dir = Directory.GetCurrentDirectory() + "\\" + SSIApplyPlatform(SSI_LIBRARY_DIRECTORY);
            TextBoxLog.Text += "library path: " + dir + Environment.NewLine;

            if (!Directory.Exists(dir))
            {
                Log("path not found: " + dir);
                return false;
            }

            Log("set $" + SSI_LIBRARY_VARIABLE + ": " + dir);
            Environment.SetEnvironmentVariable(SSI_LIBRARY_VARIABLE, dir, EnvironmentVariableTarget.User);

            return true;
        }

        private bool SSIApplyPath(bool undo)
        {
            string dir = Directory.GetCurrentDirectory() + "\\" + SSIApplyPlatform (SSI_BIN_DIRECTORY);
            TextBoxLog.Text += "bin path: " + dir + Environment.NewLine;

            if (!Directory.Exists(dir))
            {
                Log ("path not found: " + dir);
                return false;
            }

            string path = Environment.GetEnvironmentVariable(PATH_VARIABLE, EnvironmentVariableTarget.User);
            if (path == null)
            {
                path = "";
            }
            Log("current $" + PATH_VARIABLE + ": " + path);
            bool contains = PathContainsDir(path, dir);
            if (!undo && !contains)
            {
                if (path.Length == 0)
                {
                    path = dir;
                }
                else
                {
                    path += (path.Length > 0 && path[path.Length - 1] == ';' ? "" : ";") + dir;
                }
            }
            else if (undo && contains)
            {
                path = PathRemoveDir(path, dir);
            }            
            Log("new $" + PATH_VARIABLE + ": " + path);
            Environment.SetEnvironmentVariable(PATH_VARIABLE, path, EnvironmentVariableTarget.User);

            return true;

        }

        /*private bool OCVApplyLibrary(bool undo)
        {
            if (undo)
            {
                Environment.SetEnvironmentVariable(OCV_LIBRARY_VARIABLE, null, EnvironmentVariableTarget.User);
                return true;
            }

            string dir = OCVApplyPlatform (ocv_directory) + "\\" + OCV_LIBRARY_DIRECTORY;
            TextBoxLog.Text += "library path: " + dir + Environment.NewLine;

            if (!Directory.Exists(dir))
            {
                Log("path not found: " + dir);
                return false;
            }

            Log("set $" + OCV_LIBRARY_VARIABLE + ": " + dir);
            Environment.SetEnvironmentVariable(OCV_LIBRARY_VARIABLE, dir, EnvironmentVariableTarget.User);

            return true;
        }

        private bool OCVApplyInclude(bool undo)
        {
            if (undo)
            {
                Environment.SetEnvironmentVariable(OCV_INCLUDE_VARIABLE, null, EnvironmentVariableTarget.User);
                return true;
            }

            string dir = ocv_directory + "\\" + OCV_INCLUDE_DIRECTORY;
            TextBoxLog.Text += "include path: " + dir + Environment.NewLine;

            if (!Directory.Exists(dir))
            {
                Log("path not found: " + dir);
                return false;
            }

            Log("set $" + OCV_INCLUDE_VARIABLE + ": " + dir);
            Environment.SetEnvironmentVariable(OCV_INCLUDE_VARIABLE, dir, EnvironmentVariableTarget.User);

            return true;
        }

        private bool OCVApplyPath(bool undo)
        {
            if (ocv_directory == null)
            {
                Log("path not selected");
                return false;
            }

            string dir = OCVApplyPlatform(ocv_directory) + "\\" + OCV_BIN_DIRECTORY;
            TextBoxLog.Text += "bin path: " + dir + Environment.NewLine;

            if (!Directory.Exists(dir))
            {
                Log("path not found: " + dir);
                return false;
            }

            string path = Environment.GetEnvironmentVariable(PATH_VARIABLE, EnvironmentVariableTarget.User);
            if (path == null)
            {
                path = "";
            }
            Log("current $" + PATH_VARIABLE + ": " + path);
            bool contains = PathContainsDir(path, dir);
            if (!undo && !contains)
            {
                if (path.Length == 0)
                {
                    path = dir;
                }
                else
                {
                    path += (path.Length > 0 && path[path.Length - 1] == ';' ? "" : ";") + dir;
                }
            }
            else if (undo && contains)
            {
                path = PathRemoveDir(path, dir);
            }
            Log("new $" + PATH_VARIABLE + ": " + path);
            Environment.SetEnvironmentVariable(PATH_VARIABLE, path, EnvironmentVariableTarget.User);

            return true;

        }*/

        bool PathContainsDir(string path, string dir)
        {           
            string dirWithSlash = dir + "\\";
            string[] dirs = path.Split(new char[] { ';' });
            foreach (string d in dirs) 
            {
                string dReplace = d.Replace('/', '\\');
                if (dir.Equals(dReplace) || dirWithSlash.Equals(dReplace))
                {
                    return true;
                }
            }
            return false;
        }

        string PathRemoveDir(string path, string dir)
        {
            string dirWithSlash = dir + "\\";
            string[] dirs = path.Split(new char[] { ';' });
            string newPath = "";
            foreach (string d in dirs)
            {
                string dReplace = d.Replace('/', '\\');
                if (! (dir.Equals(dReplace) || dirWithSlash.Equals(dReplace)))
                {
                    newPath += d + ";";
                }
            }
            if (newPath.Length > 0 && newPath[newPath.Length - 1] == ';')
            {
                newPath = newPath.Substring(0, newPath.Length - 1);
            }

            return newPath;
        }

        private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
        {
            Process.Start(new ProcessStartInfo(e.Uri.AbsoluteUri));
            e.Handled = true;

        }

        /*private void ButtonOCVPath_Click(object sender, RoutedEventArgs e)
        {
            FolderControl fc = new FolderControl();
            fc.folderControlChanged += OnFolderControlChanged;
            Nullable<bool> result = fc.ShowDialog();
            if (result != false)
            {
                this.TextBoxOCVPath.Text = ocv_directory;
            }
        }

        private void OnFolderControlChanged(string directory)
        {
            ocv_directory = directory;
            CheckOCVPath(ocv_directory);
        }

        private bool CheckOCVPath(string directory)
        {
            if (!Directory.Exists(directory + "\\" + OCV_INCLUDE_DIRECTORY + "\\opencv")
                && !Directory.Exists(directory + "\\" + OCV_INCLUDE_DIRECTORY + "\\opencv2")
                && !Directory.Exists(OCVApplyPlatform(directory) + "\\" + OCV_BIN_DIRECTORY)
                && !Directory.Exists(OCVApplyPlatform(directory) + "\\" + OCV_LIBRARY_DIRECTORY)
                )
            {
                LabelOCVInfo.Content = OCV_INFO_WRONG_DIRECTORY;
                LabelOCVInfo.Foreground = Brushes.Red;
                return false;
            }

            if (!File.Exists(OCVApplyPlatform(directory) + "\\" + OCV_BIN_DIRECTORY + "\\opencv_core" + OCV_VERSION + ".dll")
                && !File.Exists(OCVApplyPlatform(directory) + "\\" + OCV_LIBRARY_DIRECTORY + "\\opencv_core" + OCV_VERSION + ".bin")
                && !File.Exists(OCVApplyPlatform(directory) + "\\" + OCV_BIN_DIRECTORY + "\\opencv_highgui" + OCV_VERSION + ".dll")
                && !File.Exists(OCVApplyPlatform(directory) + "\\" + OCV_LIBRARY_DIRECTORY + "\\opencv_highgui" + OCV_VERSION + ".bin")
                && !File.Exists(OCVApplyPlatform(directory) + "\\" + OCV_BIN_DIRECTORY + "\\opencv_imgproc" + OCV_VERSION + ".dll")
                && !File.Exists(OCVApplyPlatform(directory) + "\\" + OCV_LIBRARY_DIRECTORY + "\\opencv_imgproc" + OCV_VERSION + ".bin")
                )
            {
                LabelOCVInfo.Content = OCV_INFO_WRONG_VERSION;
                LabelOCVInfo.Foreground = Brushes.Red;
                return false;
            }

            LabelOCVInfo.Content = OCV_INFO_OK;
            LabelOCVInfo.Foreground = Brushes.Green;

            return true;
        }

        private void CheckBoxOCV_Checked(object sender, RoutedEventArgs e)
        {
            if (ocv_directory == null)
            {
                LabelOCVInfo.Content = OCV_INFO_SELECT_DIRECTORY;
                LabelOCVInfo.Foreground = Brushes.Red;
            }
            else
            {
                CheckOCVPath(ocv_directory);
            }
        }

        private void CheckBoxOCV_Unchecked(object sender, RoutedEventArgs e)
        {
            if (CheckBoxOCVInclude.IsChecked == false && CheckBoxOCVLibrary.IsChecked == false && CheckBoxOCVPath.IsChecked == false)
            {
                LabelOCVInfo.Content = OCV_INFO_SELECT_DIRECTORY;
                LabelOCVInfo.Foreground = Brushes.Black;
            }
        }*/

        static internal bool IsElevated()
        {
            WindowsIdentity id = WindowsIdentity.GetCurrent();
            WindowsPrincipal p = new WindowsPrincipal(id);
            return p.IsInRole(WindowsBuiltInRole.Administrator);
        }

        
    }    
}
