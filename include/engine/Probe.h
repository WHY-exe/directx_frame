#pragma once
namespace DCBuf {
	class Buffer;
}
class Probe
{
public:
	void SetTechnique(class Technique* tech_in);
	void SetStep(class Step* step_in);
	virtual bool VisitBuffer(class DCBuf::Buffer& material_data) = 0;
	virtual ~Probe() {};
protected:
	virtual void OnSetTechnique() {};
	virtual void OnSetStep() {};
protected:
	class Technique* m_pTech;
	class Step* m_pStep;
};

