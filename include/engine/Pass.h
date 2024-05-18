#pragma once
#include "Graphics.h"
#include <memory>
#include <vector>
namespace Rgph{
	class Pass
	{
	public:
		Pass(const std::string& szPassName) noexcept;
		virtual void Execute(Graphics& gfx) const noexcept(!IS_DEBUG) = 0;
		virtual void Reset() noexcept(!IS_DEBUG);
		const std::string& GetName() const noexcept;
		const std::vector<std::unique_ptr<class Sink>>& GetSinks() const;
		class Source& GetSource(const std::string& registeredName) const;
		class Sink& GetSink(const std::string& name) const;
		void SetSinkLinkage(const std::string& registeredName, const std::string& target);
		virtual void Finalize();
		virtual ~Pass();
	protected:
		void RegisterSink(std::unique_ptr<Sink> sink);
		void RegisterSource(std::unique_ptr<Source> source);
	private:
		std::vector<std::unique_ptr<class Sink>> m_sinks;
		std::vector<std::unique_ptr<class Source>> m_sources;
		std::string m_PassName;
	};
}