using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SSIXmlEditor.App;

namespace SSIXmlEditor.Command
{
    class LoadConfigCommand : ICommand
    {
        private Application m_Editor;

        public LoadConfigCommand(Application editor)
        {
            if (null == editor)
                throw new ArgumentNullException("editor");

            m_Editor = editor;
        }

        #region ICommand Members

        public void Execute()
        {
            var config = new IniFile("xmledit.ini");
            var path = config.IniReadValue("env", "ssi_path");
            var pipelineApp = config.IniReadValue("env", "pipeline_app");

            if (pipelineApp.Contains("%"))
            {
                int iStart = pipelineApp.IndexOf('%');
                var key = pipelineApp.Substring(iStart, pipelineApp.LastIndexOf('%') - iStart + 1);
                var _key = key.Substring(1, key.Length - 2);

                pipelineApp = pipelineApp.Replace(key, config.IniReadValue("env", _key));
            }

            Properties.Settings.Default.Path = path;
            Properties.Settings.Default.PipelineApp = pipelineApp;
            
            m_Editor.DllPath = path;
            m_Editor.PipelineApp = pipelineApp;
        }

        #endregion
    }
}
