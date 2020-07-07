#include "Simulations_pcp.h"

#include "Model_T2D_ME_s.h"
#include "Step_T2D_ME_s.h"

#include "Model_T2D_ME_s_hdf5_utilities.h"

#include "TimeHistory_T2D_ME_s_complete.h"

int TimeHistory_T2D_ME_s_complete::init()
{
	if (is_init)
		return 0;

	const char* res_file_type = res_file->get_type();
	if (!strcmp(res_file_type, "ResultFile_XML"))
	{

	}
	else if (!strcmp(res_file_type, "ResultFile_hdf5"))
	{
		ResultFile_hdf5& rf = *static_cast<ResultFile_hdf5*>(res_file);
		hid_t th_grp_id = rf.get_time_history_grp_id();
		th_id = rf.create_group(th_grp_id, name.c_str());
	}
	else // unknown file type
	{
		return -1;
	}

	is_init = true;
	return 0;
}

void TimeHistory_T2D_ME_s_complete::close()
{
	const char* res_file_type = res_file->get_type();
	if (!strcmp(res_file_type, "ResultFile_XML"))
	{

	}
	else if (!strcmp(res_file_type, "ResultFile_hdf5"))
	{
		ResultFile_hdf5& rf = *static_cast<ResultFile_hdf5*>(res_file);
		if (is_init)
		{
			rf.write_attribute(th_id, "output_num", output_id);
			rf.close_group(th_id);
		}
	}

	is_init = false;
}

int time_history_output_func_t2d_me_s_to_xml_res_file(TimeHistory &_self)
{
	TimeHistory_T2D_ME_s_complete &th = static_cast<TimeHistory_T2D_ME_s_complete &>(_self);
	Step_T2D_ME_s& step = static_cast<Step_T2D_ME_s&>(th.get_step());
	Model_T2D_ME_s& model = static_cast<Model_T2D_ME_s&>(step.get_model());
	ResultFile_XML &rf = static_cast<ResultFile_XML &>(*th.res_file);
	std::fstream &file = rf.get_file();
	
	char str_buffer[512];
#define str_buffer_len (sizeof(str_buffer) / sizeof(str_buffer[0]))

	// time history
	const char *time_history_info = ""
		"<TimeHistory>\n"
		"    <substep_num> %zu </substep_num>\n"
		"    <total_substep_num> %zu </total_substep_num>\n"
		"    <current_time> %16.10e </current_time>\n"
		"    <total_time> %16.10e </total_time>\n";
	snprintf(str_buffer, str_buffer_len, time_history_info,
		step.get_substep_index(), step.get_total_substep_index(),
		step.get_current_time(), step.get_total_time());
	file.write(str_buffer, strlen(str_buffer));

	// output rigid body data
	const RigidCircle::State &state
		= model.get_rigid_circle().get_state();
	const char *rigid_body_info = ""
			"    <RigidCircle>\n"
			"        <x> %16.10e </x>\n"
			"        <y> %16.10e </y>\n"
			"        <theta> %16.10e </theta>\n"
			"        <vx> %16.10e </vx>\n"
			"        <vy> %16.10e </vy>\n"
			"        <w> %16.10e </w>\n"
			"        <rfx> %16.10e </rfx>\n"
			"        <rfy> %16.10e </rfy>\n"
			"        <rm> %16.10e </rm>\n"
			"    </RigidCircle>\n";
	snprintf(str_buffer, str_buffer_len, rigid_body_info,
			 state.cen_x, state.cen_y, state.theta,
			 state.vx, state.vy, state.w, 
			 state.rfx, state.rfy, state.rm);
	file.write(str_buffer, strlen(str_buffer));

	// output material points data
	const char *material_point_info = ""
		"    <MaterialPointObject>\n"
		"        <pcl_num> %zu </pcl_num>\n";
	snprintf(str_buffer, str_buffer_len, material_point_info, model.pcl_num);
	file.write(str_buffer, strlen(str_buffer));
	// field data: x, y, vol
	file << "        <field>\n"
			"        <!-- x, y, vol, s11, s22, s12 -->\n";
	for (size_t pcl_id = 0; pcl_id < model.pcl_num; ++pcl_id)
	{
		Model_T2D_ME_s::Particle &pcl = model.pcls[pcl_id];
		file << "        ";
		snprintf(str_buffer, str_buffer_len, "%16.10e", pcl.x);
		file << str_buffer << ", ";
		snprintf(str_buffer, str_buffer_len, "%16.10e", pcl.y);
		file << str_buffer << ", ";
		snprintf(str_buffer, str_buffer_len, "%16.10e", pcl.m / pcl.density);
		file << str_buffer << ", ";
		snprintf(str_buffer, str_buffer_len, "%16.10e", pcl.s11);
		file << str_buffer << ", ";
		snprintf(str_buffer, str_buffer_len, "%16.10e", pcl.s22);
		file << str_buffer << ", ";
		snprintf(str_buffer, str_buffer_len, "%16.10e", pcl.s12);
		file << str_buffer << "\n";
	}
	file << "        </field>\n"
			"    </MaterialPointObject>\n";

	file << "</TimeHistory>\n";
	return 0;
}

int time_history_output_func_t2d_me_s_to_hdf5_res_file(TimeHistory &_self)
{
	TimeHistory_T2D_ME_s_complete &th = static_cast<TimeHistory_T2D_ME_s_complete &>(_self);
	Step_T2D_ME_s &step = static_cast<Step_T2D_ME_s &>(th.get_step());
	Model_T2D_ME_s &md = static_cast<Model_T2D_ME_s &>(step.get_model());
	ResultFile_hdf5 &rf = static_cast<ResultFile_hdf5 &>(*th.res_file);

	char frame_name[30];
	snprintf(frame_name, 30, "frame_%zu", th.output_id);
	hid_t frame_grp_id = rf.create_group(th.th_id, frame_name);
	
	rf.write_attribute(frame_grp_id, "current_time", step.get_current_time());
	rf.write_attribute(frame_grp_id, "total_time", step.get_total_time());
	rf.write_attribute(frame_grp_id, "substep_num", step.get_substep_index());
	rf.write_attribute(frame_grp_id, "total_substep_num", step.get_total_substep_index());

	using Model_T2D_ME_s_hdf5_utilities::time_history_complete_output_to_hdf5_file;
	time_history_complete_output_to_hdf5_file(md, rf, frame_grp_id);

	rf.close_group(frame_grp_id);
	++th.output_id;
	return 0;
}