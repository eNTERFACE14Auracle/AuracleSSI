using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using SSIXmlEditor.Document;

namespace SSIXmlEditor.Command
{
    class DocumentOpenCommand : ICommand
    {
        private App.Application m_App;

        public DocumentOpenCommand(App.Application app)
        {
            if (null == app)
                throw new ArgumentNullException("app");

            m_App = app;
        }

        #region ICommand Members

        public void Execute()
        {
            var openDialog = new OpenFileDialog();
            openDialog.Filter = "Pipeline (*.pipeline) |*.pipeline";
            openDialog.RestoreDirectory = true;
            if (openDialog.ShowDialog() == DialogResult.OK)
            {
                var doc = m_App.Documents.Open(openDialog.FileName);
                doc.Activate();
            }
        }

        #endregion
    }
}