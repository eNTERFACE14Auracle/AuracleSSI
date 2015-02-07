using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace SSIXmlEditor
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.ThreadException += new System.Threading.ThreadExceptionEventHandler(Application_ThreadException);
            AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler(CurrentDomain_UnhandledException);
            var app = App.Application.Instance;
            app.Init(new Command.StartUpCommand(app));
            app.MainView = new MainForm(app);
            
            Application.Run(app.MainView as Form);
        }

        static void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            MessageBox.Show(e.ExceptionObject.ToString() + Environment.NewLine + (e.ExceptionObject as Exception).StackTrace);            
        }

		static void Application_ThreadException(object sender, System.Threading.ThreadExceptionEventArgs e)
		{
			MessageBox.Show(e.Exception.Message + Environment.NewLine + e.Exception.StackTrace);
		}
    }
}
