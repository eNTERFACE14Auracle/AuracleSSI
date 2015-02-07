using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using System.IO;
using System.Xml;
using System.Text.RegularExpressions;
using System.Windows;

namespace ssi
{
    public class ModelItem
    {
        int version;

        string[] classes;
        public string Classes
        {
            get {
                StringBuilder sb = new StringBuilder();
                bool first = true;
                foreach (string c in classes)
                {
                    sb.Append((first ? "" : "\n") + c);                    
                    first = false;
                }
                return sb.ToString (); 
            }
        }

        string[] users;
        public string Users
        {
            get
            {
                StringBuilder sb = new StringBuilder();
                bool first = true;
                foreach (string c in users)
                {
                    sb.Append((first ? "" : "\n") + c);
                    first = false;
                }
                return sb.ToString();
            }
        }

        string path;
        public string Path
        {
            get { return path; }
        }

        string dir;
        public string Dir
        {
            get { return dir; }
        }

        string name;
        public string Name
        {
            get { return name; }
        }

        string date;
        public string Date
        {
            get { return date; }            
        }

        static public ModelItem Load(string dir)
        {
            ModelItem model = null;

            string[] files = Directory.GetFiles(dir, "*.trainer");           
            if (files.Length > 0)
            {                                
                model = new ModelItem();
                model.dir = dir;
                model.path = files[0].Substring(0, files[0].LastIndexOf('.'));
                model.name = model.path.Substring(model.path.LastIndexOf('\\') + 1);                               
                model.date = dir.Substring (dir.LastIndexOf ('\\') + 1);                                 
                try
                {
                    XmlTextReader xml = new XmlTextReader(files[0]);

                    xml.Read();
                    xml.Read();
                    xml.Read();
                    if (xml.Name == "trainer" && xml.HasAttributes)
                    {
                        model.version = int.Parse(xml.GetAttribute("ssi-v"));
                    }
                    else
                    {
                        throw new Exception("tag <trainer> or attribute <ssi-v> not found");
                    }

                    xml.Read();
                    xml.Read();
                    if (xml.Name == "classes" && xml.HasAttributes)
                    {
                        int size = int.Parse(xml.GetAttribute("size"));
                        xml.Read();
                        model.classes = new string[size];
                        for (int i = 0; i < size; i++)
                        {
                            xml.Read();
                            if (xml.Name == "item" && xml.HasAttributes)
                            {
                                model.classes[i] = xml.GetAttribute("name");
                            }
                            else
                            {
                                throw new Exception("tag <item> or attribute <name> not found");
                            }
                            xml.Read();
                        }
                        xml.Read();
                    }
                    else
                    {
                        throw new Exception("tag <classes> or attribute <size> not found");
                    }

                    xml.Read();
                    xml.Read();
                    if (model.version > 1)
                    {
                        if (xml.Name == "users" && xml.HasAttributes)
                        {
                            int size = int.Parse(xml.GetAttribute("size"));
                            xml.Read();
                            model.users = new string[size];
                            for (int i = 0; i < size; i++)
                            {
                                xml.Read();
                                if (xml.Name == "item" && xml.HasAttributes)
                                {
                                    model.users[i] = xml.GetAttribute("name");
                                }
                                else
                                {
                                    throw new Exception("tag <item> or attribute <name> not found");
                                }
                                xml.Read();
                            }
                        }
                        else
                        {
                            throw new Exception("tag <classes> or attribute <size> not found");
                        }
                    }

                    xml.ReadEndElement();

                    xml.Close();
                }
                catch (Exception e)
                {
                    MessageBox.Show(e.ToString());
                }
            }
            return model;
        }
        
    }

    public class ModelList : ObservableCollection<ModelItem>
    {
    }

}
