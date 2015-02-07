//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Text;
//using ICSharpCode.AvalonEdit.Document;
//using System.Text.RegularExpressions;
//using System.ComponentModel;
//using SSIXmlEditor.Visitor;
//using System.Collections;
//using SSIXmlEditor.Attribute;

//namespace SSIXmlEditor
//{
//    public interface ISSIModel
//    {
//        void Accept(IVisitor visitor);
//    }

//    public abstract class ABaseModel : ISSIModel
//    {
//        [Browsable(false)]
//        public int Line { get; set; }
                
//        public abstract void Accept(IVisitor visitor);
//    }

//    public class PipelineModel : ABaseModel
//    {
//        public RegisterModel Register { get; set; }
//        public List<OptionModel> Options { get; set; }
//        public List<SensorModel> Sensors { get; set; }

//        public override void Accept(IVisitor visitor)
//        {
//            visitor.Visit(this);
//        }
//    }

//    [TypeConverter(typeof(ExpandableObjectConverter))]
//    public class RegisterModel : ABaseModel
//    {
//        public List<LoadModel> Load { get; set; }

//        #region IModel Members

//        public override void Accept(IVisitor visitor)
//        {
//            visitor.Visit(this);
//        }

//        #endregion
//    }

//    public class LoadModel : ABaseModel
//    {
//        public string Name { get; set; }

//        #region IModel Members

//        public override void Accept(IVisitor visitor)
//        {
//            visitor.Visit(this);
//        }

//        #endregion
//    }

//    [CreateDependency(typeof(LoadModel), "register", "load", "Library")]
//    public abstract class SSIModel : ABaseModel
//    {
//        [Category("Model")]
//        [DependencyAttributeValue("create")]
//        public string Create { get; set; }
//        [Category("Model")]
//        [DependencyAttributeValue("option")]
//        public string Option { get; set; }

//        [DependencyAttributeValue("name")]
//        [ReadOnly(true)]
//        public string Library { get; set; }

//        protected SSIModel()
//        {
//            Create = String.Empty;
//            Option = String.Empty;
//            Library = String.Empty;
//        }
//    }

//    public class SetModel
//    {
//        public String Name { get; set; }
//        public String Value { get; set; }
//    }

//    public class OptionModel : SSIModel
//    {
//        public List<SetModel> Sets { get; set; }

//        public override void Accept(IVisitor visitor)
//        {
//            visitor.Visit(this);
//        }
//    }

//    //[Dependency(typeof(OptionModel), "option", "Option")]
//    public class SensorModel : SSIModel
//    {
//        [Category("Provider")]
//        public List<ProviderModel> Provider { get; set; }

//        public SensorModel()
//        {

//        }

//        public override void Accept(IVisitor visitor)
//        {
//            visitor.Visit(this);
//        }
//    }

//    [TypeConverter(typeof(ExpandableObjectConverter))]
//    [Dependency(typeof(InputPinModel), "input", "Pin")]
//    public class ProviderModel : ABaseModel
//    {
//        public String Channel { get; set; }
        
//        [DependencyAttributeValue("pin")]
//        public String Pin { get; set; }

//        public override void Accept(IVisitor visitor)
//        {
//            visitor.Visit(this);
//        }

//        public ProviderModel()
//        {
//            Channel = String.Empty;
//            Pin = String.Empty;
//        }

//        public override bool Equals(object obj)
//        {
//            if (!(obj is ProviderModel))
//                return false;

//            var provider = obj as ProviderModel;
//            return (this.Channel.Equals(provider.Channel) &&
//                this.Pin.Equals(provider.Pin) &&
//                this.Line != provider.Line) ;
//        }
//    }

//    [TypeConverter(typeof(ExpandableObjectConverter))]
//    //[Dependency(typeof(OptionModel), "option", "Option")]
//    public class ConsumerModel : SSIModel
//    {
//        public InputPinModel InputPin { get; set; }
        
//        public override void Accept(IVisitor visitor)
//        {
//            visitor.Visit(this);
//        }
//    }

//    [TypeConverter(typeof(ExpandableObjectConverter))]
//    [Dependency(typeof(InputPinModel), "input", "Event")]
//    public class TriggerModel : ConsumerModel
//    {
//        [DependencyAttributeValue("name")]
//        public string Event { get; set; }

//        public override void Accept(IVisitor visitor)
//        {
//            visitor.Visit(this);
//        }
//    }

//    [TypeConverter(typeof(ExpandableObjectConverter))]
//    public class TransformerModel : ConsumerModel  
//    {
//        //public TransformerInputPinModel InputPin { get; set; }
//        public OutputPinModel OutputPin { get; set; }

//        public override void Accept(IVisitor visitor)
//        {
//            visitor.Visit(this);
//        }
//    }

//    [TypeConverter(typeof(ExpandableObjectConverter))]
//    public abstract class APinModel : ABaseModel
//    {
//        public string Pin { get; set; }
//        public string Frame { get; set; }
//        public string Delta { get; set; }

//        public override string ToString()
//        {
//            return Pin;
//        }
//    }

//    [Dependency(typeof(ProviderModel), "provider", "Pin")]
//    [Dependency(typeof(OutputPinModel), "output", "Pin")]
//    public class InputPinModel : APinModel
//    {
//        [DependencyAttributeValue("pin")]
//        public new string Pin { get; set; }

//        [DependencyAttributeValue("frame")]
//        public new string Frame { get; set; }
		
//        [DependencyAttributeValue("delta")]
//        public new string Delta { get; set; }

//        [DependencyAttributeValue("event")]
//        public virtual string Event { get; set; }

//        public override void Accept(IVisitor visitor)
//        {
//            visitor.Visit(this);
//        }
//    }
    
//    public class TransformerInputPinModel : InputPinModel 
//    {
//        [Browsable(false)]
//        public override string Event
//        {
//            get
//            {
//                return base.Event;
//            }
//            set
//            {
//                base.Event = value;
//            }
//        }
//    }

//    [Dependency(typeof(InputPinModel), "input", "Pin")]
//    public class OutputPinModel : APinModel
//    {
//        [DependencyAttributeValue("pin")]
//        public new string Pin { get; set; }

//        public override void Accept(IVisitor visitor)
//        {
//            visitor.Visit(this);
//        }
//    }

//}
