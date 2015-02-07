using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SSIXmlEditor.App;

namespace SSIXmlEditor.Command
{
    class StartUpCommand : ICommand
    {
        private Application m_App;

        public StartUpCommand(Application app)
        {
            if (null == app)
                throw new ArgumentNullException("app");

            m_App = app;
        }

        #region ICommand Members

        public void Execute()
        {
            m_App.Documents = new SSIXmlEditor.Document.Documents(m_App);
            m_App.RegisterModel<Model.ISSIModules>(new Model.SSIModules());
            m_App.RegisterModel<Model.SSIModel>(new Model.SSIModel());

            m_App.SnippetManager = SSIXmlEditor.Snippets.SnippetManager.Instance;
            m_App.SnippetManager.Service = new Snippets.Service.XmlSnippetService();

            new LoadConfigCommand(m_App).Execute();
            new LoadMetaDataCommand(m_App).Execute();
            new LoadSnippetsCommand(m_App).Execute();

            m_App.Documents.DocumentTemplate = new Document.DocumentTemplate();

            //register commands
            m_App.RegisterCommand("New", new CreateNewDocumentCommand(m_App, new Provider.PipelineFileNameCounterProvider("neu")));
            m_App.RegisterCommand("Open", new DocumentOpenCommand(m_App));
            m_App.RegisterCommand("Save", new DocumentSaveCommand(m_App));
            m_App.RegisterCommand("SaveAs", new DocumentSaveAsCommand(m_App));
            m_App.RegisterCommand("Close", new CloseActiveDocumentCommand(m_App));
            m_App.RegisterCommand("Execute", new ExecutePipelineCommand(m_App));
            m_App.RegisterCommand("Exit", new ShutDownCommand(m_App));
        }

        #endregion
    }
}
