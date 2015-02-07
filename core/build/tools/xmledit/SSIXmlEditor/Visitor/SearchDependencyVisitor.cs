using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using SSIXmlEditor.Extension;

namespace SSIXmlEditor.Visitor
{
	//class SearchDependencyVisitor : IVisitor
	//{
	//    private ABaseModel m_Depends = null;

	//    private bool m_bSearchOptions = false;

	//    public ABaseModel Dependency { get; private set; }

	//    public SearchDependencyVisitor(ABaseModel depends)
	//    {
	//        m_Depends = depends;
	//        if (depends is OptionModel)
	//            m_bSearchOptions = false;
	//        else if (depends is SensorModel)
	//            m_bSearchOptions = true;
	//        else
	//            throw new ArgumentException("depends");
	//    }

	//    #region IVisitor Members

	//    public void Visit(PipelineModel pipeline)
	//    {
	//        //if (m_bSearchOptions)
	//        //    pipeline.Options.ForEach(opt => { if (Dependency == null) opt.Accept(this); });
	//        //else
	//        //    pipeline.Sensors.ForEach(s => { if (Dependency == null) s.Accept(this); });
	//    }

	//    public void Visit(RegisterModel register)
	//    {

	//    }

	//    public void Visit(LoadModel load)
	//    {

	//    }

	//    public void Visit(OptionModel option)
	//    {
	//        SensorModel sensor = m_Depends as SensorModel;

	//        if (sensor.Option.Equals(option.Option) && sensor.Create.Equals(option.Create))
	//            Dependency = option;
            
	//    }

	//    public void Visit(SensorModel sensor)
	//    {
	//        OptionModel option = m_Depends as OptionModel;

	//        if (sensor.Option.Equals(option.Option) && sensor.Create.Equals(option.Create))
	//            Dependency = sensor;
	//    }

	//    public void Visit(ProviderModel provider)
	//    {

	//    }

	//    #endregion

	//    #region IVisitor Members

	//    public void Visit(ConsumerModel consumer)
	//    {
	//        throw new NotImplementedException();
	//    }

	//    public void Visit(TriggerModel trigger)
	//    {
	//        throw new NotImplementedException();
	//    }

	//    public void Visit(TransformerModel transformer)
	//    {
	//        throw new NotImplementedException();
	//    }

	//    public void Visit(InputPinModel pin)
	//    {
	//        throw new NotImplementedException();
	//    }

	//    public void Visit(OutputPinModel pin)
	//    {
	//        throw new NotImplementedException();
	//    }

	//    #endregion
	//}

    
}
