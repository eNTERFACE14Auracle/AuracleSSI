#pragma once

using namespace System;
using namespace System::Collections;

namespace ssi
{
	class ITransformer;
	class ITransformable;
}

namespace SSINet
{
	public interface class ITransformable
	{
		public:
			operator ssi::ITransformable*();
	};

	public interface class ITransformer
	{
		public:
			operator ssi::ITransformer*();
	};

	ref class ITransformerWrapper : public ITransformer
	{
		private:
			ssi::ITransformer* m_pTransformer;

		public:
			ITransformerWrapper(ssi::ITransformer* pTransformer) { m_pTransformer = pTransformer; }

			virtual operator ssi::ITransformer*() { return m_pTransformer; }
	};
}