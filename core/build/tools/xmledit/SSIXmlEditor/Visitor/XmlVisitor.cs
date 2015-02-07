using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ICSharpCode.AvalonEdit.Document;
using System.Text.RegularExpressions;
using System.ComponentModel;

namespace SSIXmlEditor.Visitor
{
    /*class XmlVisitor : IVisitor
    {
        private Model.IInputHandler m_InputHandler;
        private int m_Line;

        public XmlVisitor(Model.IInputHandler inputHandler, int line)
        {
            if (null == inputHandler)
                throw new ArgumentNullException("inputHandler");

            m_InputHandler = inputHandler;
            m_Line = line;
        }

        #region IVisitor Members

        public void Visit(PipelineModel pipeline)
        {
            
        }

        public void Visit(RegisterModel register)
        {
            if (register.Load != null)
                register.Load.Clear();
            else
                register.Load = new List<LoadModel>();

            Invoke("register", m_Line, (line) =>
            {
                m_Line = line;
                string text = m_InputHandler.GetTextByLine(line).TrimStart();

                if (text.StartsWith("<load"))
                {
                    var load = new LoadModel();
                    load.Accept(this);

                    register.Load.Add(load);
                }
            });
        }

        public void Visit(LoadModel load)
        {
            string text = m_InputHandler.GetTextByLine(m_Line).TrimStart();
            if (!text.StartsWith("<load"))
                return;

            load.Line = m_Line;
            load.Name = getValue("name", text);
        }

        public void Visit(OptionModel option)
        {
            if (m_InputHandler.Lines < m_Line)
                return;

            var text = m_InputHandler.GetTextByLine(m_Line).Trim();
            if (!text.StartsWith("<option"))
                return;

            option.Create = getValue("create", text);
            option.Option = getValue("option", text);
            option.Line = m_Line;

            if (null != option.Sets)
                option.Sets.Clear();

            Invoke("option", m_Line, (line) =>
            {
                if (null == option.Sets)
                    option.Sets = new List<SetModel>();

                var lineText = m_InputHandler.GetTextByLine(line).Trim();
                if (lineText.StartsWith("<set"))
                {
                    var set = new SetModel();
                    set.Name = getValue("name", lineText);
                    set.Value = getValue("value", lineText);

                    option.Sets.Add(set);
                }
            });
        }

        public void Visit(SensorModel sensor)
        {
            if (m_InputHandler.Lines < m_Line)
                return;

            var text = m_InputHandler.GetTextByLine(m_Line).Trim();
            if (!text.StartsWith("<sensor"))
                return;

            //if (!text.EndsWith(">"))
            //    text += m_InputHandler.GetTextByLine(m_Line + 1);

            sensor.Create = getValue("create", text);
            sensor.Option = getValue("option", text);
            sensor.Line = m_Line;

            var type = sensor.GetType();
            var props = type.GetProperties();
            foreach (var prop in props)
            {
                var objs = prop.GetCustomAttributes(typeof(CategoryAttribute), true);
                if (objs.Count() > 0)
                {
                    var catAtt = objs[0] as CategoryAttribute;
                    if (catAtt.Category.Equals("Options"))
                    {
                        var value = getValue(prop.Name, text);
                        if (!string.IsNullOrEmpty(value))
                            prop.SetValue(sensor, value, null);
                    }
                }
            }

            if (sensor.Provider != null)
                sensor.Provider.Clear();


            Invoke("sensor", m_Line, (line) =>
            {
                if (sensor.Provider == null)
                    sensor.Provider = new List<ProviderModel>();

                var lineText = m_InputHandler.GetTextByLine(line).Trim();
                if (lineText.StartsWith("<provider"))
                {
                    m_Line = line;
                    var provider = new ProviderModel();
                    provider.Accept(this);

                    sensor.Provider.Add(provider);
                }
            }); 
        }

        public void Visit(ProviderModel provider)
        {
            if (m_InputHandler.Lines < m_Line)
                return;

            string text = m_InputHandler.GetTextByLine(m_Line).Trim();
            if (!text.StartsWith("<provider"))
                return;

            provider.Channel = getValue("channel", text);
            provider.Pin = getValue("pin", text);
            provider.Line = m_Line;
            
        }

        private string getValue(string attribute, string value)
        {
            var regex = new Regex(String.Format("{0}=\"(?<name>.*?)\"", attribute));
            return regex.Match(value).Groups["name"].Value;
        }


        #endregion

        #region IVisitor Members

        public void Visit(ConsumerModel consumer)
        {
            if (m_InputHandler.Lines < m_Line)
                return;

            var text = m_InputHandler.GetTextByLine(m_Line).Trim();
            if (!text.StartsWith("<consumer"))
                return;

            var type = consumer.GetType();
            var props = type.GetProperties();
            foreach (var prop in props)
            {
                var objs = prop.GetCustomAttributes(typeof(CategoryAttribute), true);
                if (objs.Count() > 0)
                {
                    var catAtt = objs[0] as CategoryAttribute;
                    if (catAtt.Category.Equals("Options"))
                    {
                        var value = getValue(prop.Name, text);
                        if (!string.IsNullOrEmpty(value))
                            prop.SetValue(consumer, value, null);
                    }
                }
            }


            consumer.Create = getValue("create", text);
            consumer.Option = getValue("option", text);
            consumer.Line = m_Line;

            if (null == consumer.InputPin)
                consumer.InputPin = new InputPinModel();

            ++m_Line;
            consumer.InputPin.Accept(this);
        }

        public void Visit(TriggerModel trigger)
        {
            var text = m_InputHandler.GetTextByLine(m_Line).Trim();
            if (!text.StartsWith("<trigger"))
                return;

            trigger.Create = getValue("create", text);
            trigger.Option = getValue("option", text);
            trigger.Line = m_Line;

            var type = trigger.GetType();
            var props = type.GetProperties();
            foreach (var prop in props)
            {
                var objs = prop.GetCustomAttributes(typeof(CategoryAttribute), true);
                if (objs.Count() > 0)
                {
                    var catAtt = objs[0] as CategoryAttribute;
                    if (catAtt.Category.Equals("Options"))
                    {
                        var value = getValue(prop.Name, text);
                        if (!string.IsNullOrEmpty(value))
                            prop.SetValue(trigger, value, null);
                    }
                }
            }

            if (null == trigger.InputPin)
                trigger.InputPin = new InputPinModel();

            Invoke("trigger", m_Line, (line) =>
            {
                m_Line = line;
                text = m_InputHandler.GetTextByLine(line).Trim();
                if (text.StartsWith("<input"))
                    trigger.InputPin.Accept(this);
                else if (text.StartsWith("<event"))
                    trigger.Event = getValue("name", text);
            });
        }

        public void Visit(TransformerModel transformer)
        {
            if (m_InputHandler.Lines < m_Line)
                return;

            var text = m_InputHandler.GetTextByLine(m_Line).Trim();
            if (!text.StartsWith("<transformer"))
                return;

            transformer.Create = getValue("create", text);
            transformer.Option = getValue("option", text);
            transformer.Line = m_Line;

            var type = transformer.GetType();
            var props = type.GetProperties();
            foreach (var prop in props)
            {
                var objs = prop.GetCustomAttributes(typeof(CategoryAttribute), true);
                if (objs.Count() > 0)
                {
                    var catAtt = objs[0] as CategoryAttribute;
                    if (catAtt.Category.Equals("Options"))
                    {
                        var value = getValue(prop.Name, text);
                        if (!string.IsNullOrEmpty(value))
                            prop.SetValue(transformer, value, null);
                    }
                }
            }

            if (null == transformer.InputPin)
                transformer.InputPin = new InputPinModel();
            if (null == transformer.OutputPin)
                transformer.OutputPin = new OutputPinModel();

            Invoke("transformer", m_Line, (line) =>
            {
                m_Line = line;

                text = m_InputHandler.GetTextByLine(line).Trim();
                if(text.StartsWith("<input"))
                    transformer.InputPin.Accept(this);
                else if(text.StartsWith("<output"))
                    transformer.OutputPin.Accept(this);
            });           
            
        }

        public void Visit(InputPinModel pin)
        {
            if (m_InputHandler.Lines < m_Line)
                return;

            var text = m_InputHandler.GetTextByLine(m_Line).Trim();
            if (text.StartsWith("<input"))
            {
                pin.Line = m_Line;
                pin.Pin = getValue("pin", text);
                pin.Event = getValue("event", text);
                pin.Frame = getValue("frame", text);
                pin.Delta = getValue("delta", text);
            }
        }

        public void Visit(OutputPinModel pin)
        {
            if (m_InputHandler.Lines < m_Line)
                return;

            var text = m_InputHandler.GetTextByLine(m_Line).Trim();
            if (text.StartsWith("<output"))
            {
                pin.Line = m_Line;
                pin.Pin = getValue("pin", text);
            }
        }

        #endregion

        private void Invoke(string tag, int startLine, Action<int> act)
        {
            int endLine = startLine;
            endLine = getEndLine(startLine, tag);

            for (int i = startLine; i < endLine; ++i)
            {
                m_Line = i;

                string text = m_InputHandler.GetTextByLine(i);
                if (text.StartsWith("<!--") && !text.EndsWith("-->"))
                {
                    bool bFound = false;
                    for (int k = i+1; k < endLine; ++k)
                    {   
                        m_Line = k;

                        text = m_InputHandler.GetTextByLine(k);
                        if (text.StartsWith("-->") || text.EndsWith("-->"))
                        {
                            bFound = true;
                            
                            m_Line = k + 1;
                            i = m_Line;
                            break;
                        }
                    }

                    if (!bFound)
                        continue;
                }

                act(m_Line);
            }
        }

        private int getEndLine(int startLine, string tag)
        {
            while (startLine <= m_InputHandler.Lines)
            {
                var text = m_InputHandler.GetTextByLine(startLine);
                text = text.TrimStart();
                if (text.StartsWith("</" + tag))
                    break;

                ++startLine;
            }

            return startLine;
        }
    }*/
}
