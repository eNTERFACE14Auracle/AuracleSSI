﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SSIXmlEditor.App;

namespace SSIXmlEditor.Command
{
    class LoadMetaDataCommand : ICommand
    {
        private Application m_App;

        public LoadMetaDataCommand(Application app)
        {
            if (null == app)
                throw new ArgumentNullException("editor");

            m_App = app;
        }

        #region ICommand Members

        public void Execute()
        {
            var model = m_App.GetModel<Model.ISSIModules>();
            model.Service = new Service.MetaDataService(m_App.DllPath);
            model.Load();
        }

        #endregion
    }
}
