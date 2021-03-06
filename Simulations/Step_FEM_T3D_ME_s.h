#ifndef __Step_FEM_T3D_ME_s_h__
#define __Step_FEM_T3D_ME_s_h__

#include "Step.h"
#include "Model_FEM_T3D_ME_s.h"

int solve_substep_FEM_T3D_ME_s(void* _self);

// for single object only
class Step_FEM_T3D_ME_s : public Step
{
public:
	typedef Model_FEM_T3D_ME_s::Particle Particle;
	typedef Model_FEM_T3D_ME_s::Element Element;
	typedef Model_FEM_T3D_ME_s::Node Node;

protected:
	Model_FEM_T3D_ME_s *model;

	int init_calculation() override;
	friend int solve_substep_FEM_T3D_ME_s(void *_self);
	int finalize_calculation() override;

	// local damping factor
	double damping_ratio;

public:
	Step_FEM_T3D_ME_s(const char *_name);
	~Step_FEM_T3D_ME_s();

	inline void set_model(Model_FEM_T3D_ME_s &md)
	{
		Step::set_model(md);
		model = &md;
	}

	// Restart from previous step
	inline void set_prev_step(Step_FEM_T3D_ME_s &prev_step)
	{
		Step::set_prev_step(prev_step);
		model = prev_step.model;
	}

	inline void set_damping_ratio(double _ratio) noexcept { damping_ratio = _ratio; }
};

#endif