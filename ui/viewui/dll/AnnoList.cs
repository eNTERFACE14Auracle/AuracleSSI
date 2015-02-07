using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;
using System.Xml;
using System.Xml.Linq;

namespace ssi
{
    public class AnnoList : MyEventList
    {

        enum Type
        {
            EMPTY,
            NTUPLE,
            FLOATS,
            STRING
        }

        private bool loaded = false;
        private String name = null;
        private String filename = null;
        private String filepath = null;

        public bool Loaded
        {
            get { return loaded; }
            set { loaded = value; }
        }

        public string Filename
        {
            get { return filename; }
        }

        public string Filepath
        {
            get { return filepath; }
        }

        public String Name
        {
            get { return name; }
        }

        public AnnoList()
        {
        }

        public AnnoList(String filepath)
        {
            this.filepath = filepath;
            string[] tmp = filepath.Split('\\');
            this.filename = tmp[tmp.Length - 1];
            this.name = this.filename.Split('.')[0];                      
        }

        public static AnnoList LoadFromEventsFile(String filepath)
        {
            AnnoList list = new AnnoList(filepath);

            try
            {

                XDocument doc = XDocument.Load(filepath);
                var events = doc.Descendants("event");

                foreach (var e in events)
                {
                    string label = null;
                    string meta = "";
                    double start = -1;
                    double duration = -1;
                    Type type = Type.EMPTY;

                    var sender_attr = e.Attribute("sender");
                    var event_attr = e.Attribute("event");
                    label = (sender_attr == null ? "" : sender_attr.Value)
                            + "@"
                            + (event_attr == null ? "" : event_attr.Value);
                    var from_attr = e.Attribute("from");
                    if (from_attr != null)
                    {
                        start = double.Parse(from_attr.Value) / 1000.0;
                    }
                    var dur_attr = e.Attribute("dur");
                    if (dur_attr != null)
                    {
                        duration = double.Parse(dur_attr.Value) / 1000.0;
                    }
                    var type_attr = e.Attribute("type");
                    if (type_attr != null)
                    {
                        switch (type_attr.Value)
                        {
                            case "NTUPLE":
                                type = Type.NTUPLE;
                                break;
                            case "FLOATS":
                                type = Type.FLOATS;
                                break;
                            case "STRING":
                                type = Type.STRING;
                                break;
                        }
                    }

                    switch (type)
                    {
                        case Type.NTUPLE:
                            var tuples = doc.Descendants("tuple");
                            foreach (var tuple in tuples)
                            {
                                var string_attr = tuple.Attribute("string");
                                var value_attr = tuple.Attribute("value");
                                meta = meta + ((string_attr == null ? "" : string_attr.Value)
                                    + "="
                                    + (value_attr == null ? "" : value_attr.Value)) + "\n";
                            }
                            break;
                        case Type.STRING:
                        case Type.FLOATS:
                            meta = e.Value == null ? "" : e.Value;
                            break;
                    }

                    list.Add(new AnnoListItem(start, duration, label, meta));
                }

            }
            catch (Exception ex)
            {
                ViewTools.ShowErrorMessage(ex.ToString());
            }

            return list;
        }

        public static AnnoList LoadfromFile(String filepath)
        {
            AnnoList list = new AnnoList(filepath);

            try
            {
                StreamReader sr = new StreamReader (filepath, System.Text.Encoding.Default);
                //list.events = new MyEventList();
                string pattern = "^([0-9]+.[0-9]+|[0-9]+) ([0-9]+.[0-9]+|[0-9]+) .+";
                Regex reg = new Regex(pattern);
                string line = null;
                while ((line = sr.ReadLine()) != null)
                {
                    if (reg.IsMatch(line))
                    {
                        string[] data = line.Split(' ');                        
                        double start = Convert.ToDouble(data[0]);
                        double duration = Convert.ToDouble(data[1]) - Convert.ToDouble(data[0]);
                        string label = data[2];
                        for (int i = 3; i < data.Length; i++)
                        {
                            label += " " + data[i];
                        }
                        AnnoListItem e = new AnnoListItem(start, duration, label);
                        list.Add(e);
                    }
                }
                sr.Close(); ;
                list.loaded = true;
            }
            catch (Exception e)
            {
                Console.WriteLine("Can't read annotation file: " + filepath, e);
            }

            return list;
        }

        public AnnoList saveToFile()
        {
            if (filepath == null)
            {
                filepath = ViewTools.SaveFileDialog("", ".anno");
                if (filepath != null)
                {
                    filename = filepath.Split('.')[0];
                }
            }

            return saveToFile(filepath);
        }

        public AnnoList saveToFile(String _filename)
        {
            try
            {
                StreamWriter sw = new StreamWriter(_filename, false, System.Text.Encoding.Default); 
                
                foreach (AnnoListItem e in this)
                {
                    sw.WriteLine(e.Start.ToString() + " " + e.Stop.ToString() + " " + e.Label);
                }
                sw.Close();

                AnnoList newAnno = new AnnoList(_filename);
                return newAnno;

            }
            catch
            {
                return null;
            }
        }
    }
}
