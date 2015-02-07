using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SSIXmlEditor.Extension;

namespace SSIXmlEditor.Controller
{
    class MainController
    {
        private App.Application Application { get; set; }
        private View.IMainView View { get; set; }

        private InputHandler.XmlEditorLogic m_EditorLogik;

        public MainController(App.Application app, View.IMainView view)
        {
            if (null == app)
                throw new ArgumentNullException("app");
            if (null == view)
                throw new ArgumentNullException("view");

            Application = app;
            View = view;
            View.Caption = "SSI/XmlEditor v0.6 (by Johannes Wagner & Benjamin Hrzek, Human Centered Multimedia, University Augsburg http://hcm-lab.de)";
            
            m_EditorLogik = new SSIXmlEditor.InputHandler.XmlEditorLogic(app.GetModel<Model.SSIModel>(), app.ModelRepository);//new Repository.ModelRepository("./ClassDef.xml", app.GetModel<Model.SSIModel>()));

            Application.ActiveDocumentChanged += new EventHandler(Application_ActiveDocumentChanged);

			View.RegisterCommand("New", true);
			View.RegisterCommand("Execute", false);
			View.RegisterCommand("Open", true);
			View.RegisterCommand("Save", false);
			View.RegisterCommand("SaveAs", false);
			View.RegisterCommand("Close", false);
			View.RegisterCommand("Exit", true);
        }

        void Application_ActiveDocumentChanged(object sender, EventArgs e)
        {
            var activeDoc = Application.ActiveDocument;

			if (null == activeDoc)
			{
				View.ActivateCommand("New", true);
				View.ActivateCommand("Execute", false);
				View.ActivateCommand("Open", true);
				View.ActivateCommand("Save", false);
				View.ActivateCommand("SaveAs", false);
				View.ActivateCommand("Close", false);
				View.ActivateCommand("Exit", true);
			}
			else
            {
				m_EditorLogik.InputHandler = activeDoc.InputHandler;

				View.ActivateCommand("New", true);
				View.ActivateCommand("Execute", true);
				View.ActivateCommand("Open", true);
				View.ActivateCommand("Save", true);
				View.ActivateCommand("SaveAs", true);
				View.ActivateCommand("Close", true);
				View.ActivateCommand("Exit", true);
			}            
        }
    }
}
